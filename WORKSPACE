# WORKSPACE - 项目元数据与快速上手说明
# 目的：为协作者提供一个简单易读的文件，说明本仓库的构建方式、依赖与常见运行命令。
# 注意：本仓库主要使用 CMake/原始 Makefile 构建 linux-0.11 镜像；本文件不是 Bazel 的 WORKSPACE 文件。

project: Cmake-linux0.11
summary: CMake wrapper for the original linux-0.11 sources. Supports legacy Makefile "Image" target and optional native CMake targets for per-module builds.

maintainers:
  - YearsAlso
  - contact: （在此处填写邮箱或其他联系方式）

# 先决条件（推荐）
# - CMake >= 3.15
# - make
# - GNU toolchain: gcc, ld, as
# - 8086/16-bit 汇编/链接工具（原始 Makefile 可能期望 as86/ld86，或 bin86）
# - qemu-system-i386（用于运行 Image）

quick-start:
  - mkdir build && cd build
  - cmake ..
  - cmake --build . --target legacy-image -j$(nproc)    # 生成 Image
  - qemu-system-i386 -fda Image -boot a -m 16           # 在 QEMU 中启动

notes:
  - legacy-image: CMake 提供的 wrapper，会调用仓库根目录的原始 Makefile 以生成传统的 Image 镜像。
  - native targets: 可用的 CMake 目标包括 native-kernel / native-boot / native-all（用于生成本机静态库或工具，便于调试），它们不会直接产出 Image。
  - 在 Windows 上建议使用 WSL 或 Cygwin 来满足工具链要求；直接在原生 Windows 环境下使用原始 Makefile 常有兼容性问题。

troubleshooting:
  - as86: command not found -> 在目标环境安装相应的 8086 汇编工具，或修改 Makefile 使用 nasm/binutils 等现代工具。
  - Image 未生成 -> 检查 tools/build 是否编译成功并可执行，查看 make 输出里具体的错误。

related:
  - README.md: 请先阅读仓库根目录下的 `README.md`，其中包含更详细的构建与运行示例。

changelog: |
  - 2025-09-27: 初始填充 WORKSPACE，补充快速上手与平台注意事项。
