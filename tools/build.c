/*
 *  linux/tools/build.c
 *
 *  (C) 1991  Linus Torvalds
 */

/*
 * This file builds a disk-image from three different files:
 *
 * - bootsect: max 510 bytes of 8086 machine code, loads the rest
 * - setup: max 4 sectors of 8086 machine code, sets up system parm
 * - system: 80386 code for actual system
 *
 * It does some checking that all files are of the correct type, and
 * just writes the result to stdout, removing headers and padding to
 * the right amount. It also writes some system data to stderr.
 */

/*
 * Changes by tytso to allow root device specification
 */

#include <fcntl.h>
#include <linux/fs.h>
#include <stdio.h>  /* fprintf */
#include <stdlib.h> /* contains exit */
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h> /* unistd.h needs this */
#include <unistd.h>    /* contains read/write */

#define MINIX_HEADER 32
#define GCC_HEADER 1024

#define SYS_SIZE 0x2000

#define DEFAULT_MAJOR_ROOT 3
#define DEFAULT_MINOR_ROOT 6

/* max nr of sectors of setup: don't change unless you also change
 * bootsect etc */
#define SETUP_SECTS 4

#define STRINGIFY(x) #x

/*
 * 打印错误信息并退出程序
 * @param str: 要打印的错误信息字符串
 */
void die(char* str) {
	fprintf(stderr, "%s\n", str);
	exit(1);
}

/*
 * 打印程序使用方法并退出
 */
void usage(void) {
	die("Usage: build bootsect setup system [rootdev] [> image]");
}

/*
 * 程序主函数，用于构建磁盘镜像文件
 * @param argc: 命令行参数个数
 * @param argv: 命令行参数数组
 * @return: 成功返回0，失败返回1
 */
int main(int argc, char** argv) {
	int i, c, id;
	char buf[1024];
	char major_root, minor_root;
	struct stat sb;

	/* 检查参数数量是否正确 */
	if ((argc != 4) && (argc != 5))
		usage();

	/* 处理根设备参数 */
	if (argc == 5) {
		if (strcmp(argv[4], "FLOPPY")) {
			/* 如果不是"FLOPPY"，则获取设备文件的主次设备号 */
			if (stat(argv[4], &sb)) {
				perror(argv[4]);
				die("Couldn't stat root device.");
			}
			major_root = MAJOR(sb.st_rdev);
			minor_root = MINOR(sb.st_rdev);
		} else {
			/* 如果是"FLOPPY"，则设置主次设备号为0 */
			major_root = 0;
			minor_root = 0;
		}
	} else {
		/* 使用默认的根设备号 */
		major_root = DEFAULT_MAJOR_ROOT;
		minor_root = DEFAULT_MINOR_ROOT;
	}

	fprintf(stderr, "Root device is (%d, %d)\n", major_root, minor_root);

	/* 检查根设备主设备号是否合法 */
	if ((major_root != 2) && (major_root != 3) &&
	    (major_root != 0)) {
		fprintf(stderr, "Illegal root device (major = %d)\n",
		        major_root);
		die("Bad root device --- major #");
	}

	/* 初始化缓冲区 */
	for (i = 0; i < sizeof buf; i++)
		buf[i] = 0;

	/* 处理bootsect文件 */
	if ((id = open(argv[1], O_RDONLY, 0)) < 0)
		die("Unable to open 'boot'");
	if (read(id, buf, MINIX_HEADER) != MINIX_HEADER)
		die("Unable to read header of 'boot'");

	/* 验证bootsect文件的Minix头部 */
	if (((long*)buf)[0] != 0x04100301)
		die("Non-Minix header of 'boot'");
	if (((long*)buf)[1] != MINIX_HEADER)
		die("Non-Minix header of 'boot'");
	if (((long*)buf)[3] != 0)
		die("Illegal data segment in 'boot'");
	if (((long*)buf)[4] != 0)
		die("Illegal bss in 'boot'");
	if (((long*)buf)[5] != 0)
		die("Non-Minix header of 'boot'");
	if (((long*)buf)[7] != 0)
		die("Illegal symbol table in 'boot'");

	i = read(id, buf, sizeof buf);
	fprintf(stderr, "Boot sector %d bytes.\n", i);

	/* 检查bootsect大小是否为512字节 */
	if (i != 512)
		die("Boot block must be exactly 512 bytes");

	/* 检查boot标志 */
	if ((*(unsigned short*)(buf + 510)) != 0xAA55)
		die("Boot block hasn't got boot flag (0xAA55)");

	/* 设置根设备信息到bootsect */
	buf[508] = (char)minor_root;
	buf[509] = (char)major_root;

	i = write(1, buf, 512);
	if (i != 512)
		die("Write call failed");
	close(id);

	/* 处理setup文件 */
	if ((id = open(argv[2], O_RDONLY, 0)) < 0)
		die("Unable to open 'setup'");
	if (read(id, buf, MINIX_HEADER) != MINIX_HEADER)
		die("Unable to read header of 'setup'");

	/* 验证setup文件的Minix头部 */
	if (((long*)buf)[0] != 0x04100301)
		die("Non-Minix header of 'setup'");
	if (((long*)buf)[1] != MINIX_HEADER)
		die("Non-Minix header of 'setup'");
	if (((long*)buf)[3] != 0)
		die("Illegal data segment in 'setup'");
	if (((long*)buf)[4] != 0)
		die("Illegal bss in 'setup'");
	if (((long*)buf)[5] != 0)
		die("Non-Minix header of 'setup'");
	if (((long*)buf)[7] != 0)
		die("Illegal symbol table in 'setup'");

	/* 读取并写入setup文件内容 */
	for (i = 0; (c = read(id, buf, sizeof buf)) > 0; i += c)
		if (write(1, buf, c) != c)
			die("Write call failed");
	close(id);

	/* 检查setup文件大小是否超过限制 */
	if (i > SETUP_SECTS * 512)
		die("Setup exceeds " STRINGIFY(SETUP_SECTS) " sectors - rewrite build/boot/setup");
	fprintf(stderr, "Setup is %d bytes.\n", i);

	/* 填充setup到指定扇区大小 */
	for (c = 0; c < sizeof(buf); c++)
		buf[c] = '\0';
	while (i < SETUP_SECTS * 512) {
		c = SETUP_SECTS * 512 - i;
		if (c > sizeof(buf))
			c = sizeof(buf);
		if (write(1, buf, c) != c)
			die("Write call failed");
		i += c;
	}

	/* 处理system文件 */
	if ((id = open(argv[3], O_RDONLY, 0)) < 0)
		die("Unable to open 'system'");
	if (read(id, buf, GCC_HEADER) != GCC_HEADER)
		die("Unable to read header of 'system'");

	/* 验证system文件的GCC头部 */
	if (((long*)buf)[5] != 0)
		die("Non-GCC header of 'system'");

	/* 读取并写入system文件内容 */
	for (i = 0; (c = read(id, buf, sizeof buf)) > 0; i += c)
		if (write(1, buf, c) != c)
			die("Write call failed");
	close(id);

	fprintf(stderr, "System is %d bytes.\n", i);

	/* 检查system文件大小是否超过限制 */
	if (i > SYS_SIZE * 16)
		die("System is too big");

	return (0);
}