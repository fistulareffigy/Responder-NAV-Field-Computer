from pathlib import Path

Import("env")

# Keep local account and checkout paths out of compiler diagnostics embedded in
# release binaries. Forward slashes match the paths emitted by GCC on Windows.
home = Path.home().as_posix()
project = Path(env.subst("$PROJECT_DIR")).resolve().as_posix()
flags = []
for source, replacement in ((project, "."), (home, "~")):
    flags.extend(
        (
            f"-ffile-prefix-map={source}={replacement}",
            f"-fmacro-prefix-map={source}={replacement}",
            f"-fdebug-prefix-map={source}={replacement}",
        )
    )

env.Append(CCFLAGS=flags)
