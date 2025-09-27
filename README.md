# Cmake-linux0.11

这是一个使用 CMake 包装（驱动）原始 linux-0.11 源码的构建工程。

仓库保留了原始的 Makefile、汇编与链接脚本，同时提供了 CMake 的封装：你可以通过 CMake 构建“原始（legacy）”Makefile 目标（生成传统的 Image 文件），也可以选择基于 CMake 的“native”目标来单独构建静态库或本地可执行文件用于调试与分析。

下文包含快速上手、依赖、常见命令与目录说明，便于在不同平台（Linux / WSL / Cygwin / MinGW）下编译与运行此经典内核镜像。

## 主要特性

- 保留并兼容原始 linux-0.11 的构建流程（Makefile、boot/setup、tools/build 等）。
- 提供 CMake 包装：更容易在现代环境中进行部分或全部子模块的构建（例如 `native-kernel`、`native-boot`、`native-all`）。

## 先决条件（推荐）

在主机或开发环境中需要以下工具（具体名称与安装方法随平台而异）：

- CMake >= 3.15
- make（或等效的 Unix Make 工具）
- GNU 工具链（gcc、ld、as 等）
- 8086/16-bit 汇编器/链接器（原始 Makefile 期望 `as86` / `ld86`；在现代系统上可以安装 binutils 或寻找等效工具包）
- qemu（用于启动生成的 `Image` 镜像进行测试，例如 `qemu-system-i386`）

提示：在 Windows 上推荐使用 WSL 或 Cygwin/MinGW 环境来满足上述依赖（仓库中已有 `cmake-build-debug-wsl` 和 `cmake-build-debug-cygwin` 等输出目录示例）。

## 快速开始（Unix / WSL / Cygwin）

1. 创建构建目录并运行 CMake：

```powershell
mkdir build
cd build
cmake ..
```

2. 使用 legacy Makefile 目标来生成传统的磁盘镜像 `Image`：

```powershell
# 使用 CMake 的 wrapper target：
cmake --build . --target legacy-image -j$(nproc)

# 或者直接回到源目录运行原始 Makefile（等效）：
make Image
```

3. 运行生成的镜像（使用 QEMU）：

```powershell
qemu-system-i386 -fda Image -boot a -m 16

# 或者：
qemu-system-i386 -hda Image -boot c -m 16
```

> 注意：`tools/build` 程序会把 `boot/bootsect`、`boot/setup`、`tools/system` 等合并并输出为 `Image`，这与原始 Makefile 的 `Image` 目标一致。

## 使用 CMake 的 native 目标（可选）

CMake 同时提供部分模块的 native 构建（便于本机调试）：

- `native-all`：构建所有可通过 CMake 单独构建的子项目（例如 `native-kernel`、`native-boot`、`native-lib` 等，具体在 configure 时可见）
- `native-kernel`：生成 `libkernel.a`（在构建目录中）
- `native-boot`：生成 boot 相关的本地对象（如果可用）

示例：

```powershell
cmake --build . --target native-all -j$(nproc)
```

这些目标不会自动产生传统的 `Image` 文件（因为 `Image` 由 legacy 工具链/脚本生成），但对调试或静态分析很有用。

## 清理

```powershell
cmake --build . --target legacy-clean
# 或
make clean
```

## 目录概览（仓库重要目录）

- `boot/`：引导扇区与 setup 程序（bootsect.s、setup.s、head.s）
- `kernel/`：内核源码与驱动、数学库等
- `fs/`：文件系统实现（read_write、inode、super 等）
- `init/`：用户空间初始化（init 程序）
- `lib/`：内核用到的库函数（malloc、string 等）
- `mm/`：内存管理代码
- `tools/`：工具程序，如 `build.c`，负责把各部分打包成 `Image`
- `include/`：头文件和配置

## 常见问题与排查建议

- 如果出现 "as86: command not found" 或类似汇编器缺失错误：请安装对应的 8086 汇编/链接工具，或在 WSL 上安装兼容的包。某些发行版可通过 `apt install bin86`、`apt install nasm`（并调整 Makefile）获得支持。
- 如果 `Image` 未生成：检查 `tools/build` 是否成功编译并可执行，查看 `make Image` 的输出错误信息。
- 在 Windows 上直接使用原始 Makefile 可能会因为工具链差异失败，推荐：WSL 或在 Cygwin 环境中编译，或使用 CMake 的 native 目标作局部验证。

## 代码风格与格式

仓库包含 `.clang-format`，项目历史使用内核风格（制表符缩进等）。提交前尽量保持与现有文件一致的风格。

## 贡献

欢迎提交 issue 或 PR：

- 修复构建脚本以支持现代工具链（例如使用 nasm/ld 的替代流程）
- 在 README 或文档中补充更多平台（Windows、Mac、不同发行版）的构建说明

在进行重大改动前请先开 issue 讨论设计与兼容性。

## 许可与致谢

该仓库基于经典的 linux-0.11 源码进行组织与封装。请遵循原始源码的许可要求（如适用）并在分发或重用时保留原作者声明。

---

如果你希望我：

1. 把 README 翻译成英文版本；
2. 添加一个示例脚本（PowerShell/WSL）一键构建并运行 QEMU；
3. 自动检测并在 Windows 上给出更精确的依赖安装命令（基于 WSL / Chocolatey / Scoop）；

告诉我你想要的项，我会继续补充并把对应文件添加到仓库中。
