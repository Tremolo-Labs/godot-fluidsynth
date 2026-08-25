#!/usr/bin/env python
import os
import sys
import subprocess

from methods import print_error


libname = "gdmidiplayer"
projectdir = "project"

localEnv = Environment(tools=["default"], PLATFORM="")

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env["api_version"] = ARGUMENTS.get("api_version", "4.7")
env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

env.Append(CPPPATH=["src/"])


# =============================================================================
# Fluidsynth Configuration
# =============================================================================

def configure_fluidsynth_pkgconfig(env):
    """Configure fluidsynth using pkg-config (Linux)."""
    try:
        cflags = subprocess.check_output(
            ["pkg-config", "--cflags", "fluidsynth"],
            stderr=subprocess.DEVNULL
        ).decode().strip()
        libs = subprocess.check_output(
            ["pkg-config", "--libs", "fluidsynth"],
            stderr=subprocess.DEVNULL
        ).decode().strip()

        env.MergeFlags(cflags)
        env.MergeFlags(libs)
        print("Found fluidsynth via pkg-config")
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False


def configure_fluidsynth_vcpkg(env):
    """Configure fluidsynth using vcpkg (Windows/macOS)."""
    vcpkg_root = os.environ.get("VCPKG_ROOT", "")
    if not vcpkg_root:
        return False

    platform = env.get("platform", "")
    arch = env.get("arch", "")

    # Determine vcpkg triplet
    if platform == "windows":
        if arch == "x86_64":
            triplet = "x64-windows-static"
        elif arch == "x86_32":
            triplet = "x86-windows-static"
        else:
            return False
    elif platform == "macos":
        # For universal builds, prefer arm64 headers (they're the same)
        triplet = "arm64-osx"
    else:
        return False

    installed_path = os.path.join(vcpkg_root, "installed", triplet)
    include_path = os.path.join(installed_path, "include")
    lib_path = os.path.join(installed_path, "lib")

    if not os.path.isdir(include_path):
        print(f"vcpkg include path not found: {include_path}")
        return False

    env.Append(CPPPATH=[include_path])
    env.Append(LIBPATH=[lib_path])

    if platform == "windows":
        # Static linking on Windows
        env.Append(LIBS=["fluidsynth", "glib-2.0", "intl", "iconv"])
    elif platform == "macos":
        # On macOS with universal builds, we need both architectures
        arm64_lib = os.path.join(vcpkg_root, "installed", "arm64-osx", "lib")
        x64_lib = os.path.join(vcpkg_root, "installed", "x64-osx", "lib")
        env.Append(LIBPATH=[arm64_lib, x64_lib])
        env.Append(LIBS=["fluidsynth"])

    print(f"Found fluidsynth via vcpkg ({triplet})")
    return True


def configure_fluidsynth_fallback(env):
    """Fallback: assume standard system paths."""
    env.Append(LIBS=["fluidsynth"])
    print("Using fluidsynth fallback (standard system paths)")


# Try configuration methods in order of preference
platform = env.get("platform", "")

if platform == "linux":
    if not configure_fluidsynth_pkgconfig(env):
        configure_fluidsynth_fallback(env)
elif platform in ["windows", "macos"]:
    if not configure_fluidsynth_vcpkg(env):
        if not configure_fluidsynth_pkgconfig(env):
            configure_fluidsynth_fallback(env)
else:
    # Android, iOS, web - not yet supported
    print(f"WARNING: Fluidsynth not configured for platform '{platform}'")
    print("         Build may fail due to missing fluidsynth dependency")
    configure_fluidsynth_fallback(env)


# =============================================================================
# Build Configuration
# =============================================================================

sources = Glob("src/*.cpp")

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)

copy = env.Install("{}/bin/{}/".format(projectdir, env["platform"]), library)

default_args = [library, copy]
Default(*default_args)
