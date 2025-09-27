cc_binary(
    name = "build",
    srcs = ["build.c"],
    includes = ["../include"],
)
cc_library(
    name = "boot",
    srcs = ["bootsect.s", "head.s", "setup.s"],
    includes = ["../include"],
    visibility = ["//visibility:public"],
)

