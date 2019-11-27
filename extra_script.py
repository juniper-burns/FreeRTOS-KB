Import("env")


env.Append(
    LINKFLAGS=[
        "-static",
        "-Xlinker",
        "--gc-sections", "-Xlinker",
        "--defsym=__stack_size=1K",
        "-O0",
        "-g3",
        "-ffunction-sections",
        "-fdata-sections",
        #"--specs=nano.specs",
        "-nostartfiles",
        "-L../"
        "-L./"
        ]
)