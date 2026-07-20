import os
import shutil
from pathlib import Path

Import("env")

env["ENV"]["PYTHONIOENCODING"] = "utf-8"
env["ENV"]["PYTHONUTF8"] = "1"
os.environ["PYTHONIOENCODING"] = "utf-8"
os.environ["PYTHONUTF8"] = "1"

platform = env.PioPlatform()

def get_package_dir(name):
    try:
        return platform.get_package_dir(name)
    except KeyError:
        return None

pkg_dir = get_package_dir("riscv32-esp-elf") or get_package_dir("toolchain-riscv32-esp")

if not pkg_dir:
    home = Path.home()
    for candidate in (
        home / ".platformio" / "packages" / "riscv32-esp-elf",
        home / ".platformio" / "packages" / "toolchain-riscv32-esp",
        home / ".platformio" / "tools" / "toolchain-riscv32-esp",
    ):
        if candidate.exists():
            pkg_dir = str(candidate)
            break

# If the installed package is missing headers (seen on Windows ZIP installs), copy our local full toolchain.
def has_headers(path):
    if not path:
        return False
    return os.path.isdir(os.path.join(path, "include")) or os.path.isdir(os.path.join(path, "riscv32-esp-elf", "include"))

def bin_path(path):
    direct = os.path.join(path, "bin")
    nested = os.path.join(path, "riscv32-esp-elf", "bin")
    if os.path.isdir(nested):
        return nested
    return direct

if not has_headers(pkg_dir):
    fallback = os.path.join(env.subst("$PROJECT_DIR"), ".pio-riscv-toolchain")
    core_dir = os.environ.get("PLATFORMIO_CORE_DIR", os.path.expanduser("~/.platformio"))
    src = os.path.join(core_dir, "packages", "toolchain-riscv32-esp")
    if os.path.isdir(src):
        if os.path.isdir(fallback):
            shutil.rmtree(fallback)
        shutil.copytree(src, fallback)
        pkg_dir = fallback

if pkg_dir:
    compiler_bin = bin_path(pkg_dir)
    env.PrependENVPath("PATH", compiler_bin)
    os.environ["PATH"] = compiler_bin + os.pathsep + os.environ.get("PATH", "")
    print(f"add_riscv_path: using {compiler_bin}")
