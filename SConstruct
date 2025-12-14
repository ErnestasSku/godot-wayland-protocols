#!/usr/bin/env python
import os

env = Environment(ENV=os.environ)

platform = ARGUMENTS.get("platform", "linux")
arch = ARGUMENTS.get("arch", "x86_64")
target = ARGUMENTS.get("target", "template_debug")

env = SConscript("godot-cpp/SConstruct", exports={"platform": platform, "arch": arch, "target": target})
env.Append(CPPPATH=["include", "include/generated/", "include/workspace/"])

# godot-cpp files, from submodule
env.Append(LIBPATH=["godot-cpp/bin"])
env.Append(LIBS=[f"godot-cpp.{platform}.{target}.{arch}"])

env.ParseConfig("pkg-config --cflags --libs wayland-client")

# Generate protocols
protocols = Glob("protocols/*.xml")

generated_headers = []
generated_sources = []

for xml in protocols:
    name = os.path.splitext(os.path.basename(str(xml)))[0]

    header = f"include/generated/{name}.h"
    code = f"src/generated/{name}.c"

    header_out = env.Command(
        header,
        xml,
        f"wayland-scanner client-header $SOURCE $TARGET"
    )

    code_out = env.Command(
        code,
        xml,
        f"wayland-scanner private-code $SOURCE $TARGET"
    )

    generated_headers.append(header)
    generated_sources.append(code)


build_dir = f"build/{platform}/{target}/{arch}"
env.VariantDir(build_dir, ".", duplicate=0)

def collect_sources(root, extensions):
    result = []
    for dirpath, dirnames, filenames in os.walk(root):
        if "generated" in dirpath:
            continue
        for f in filenames:
            if any(f.endswith(ext) for ext in extensions):
                result.append(os.path.join(dirpath, f))
    return result

sources = collect_sources("src", [".cpp", ".c"])
sources += generated_sources

if target == "template_debug":
    env.Append(CXXFLAGS=["-g", "-std=c++20"])
    env.Append(CFLAGS=["-g", "-std=c11"])
else:
    env.Append(CXXFLAGS=["-O2", "-std=c++20"])
    env.Append(CFLAGS=["-O2", "-std=c11"])

# output name
lib_name = f"bin/wayland_protocols.{platform}.{target}.{arch}"
if platform == "linux":
    lib_name += ".so"

library = env.SharedLibrary(lib_name, sources)

Default(library)

env.Tool('compilation_db')
env.CompilationDatabase('compile_commands.json')
