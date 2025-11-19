#!/usr/bin/env python
import os

env = Environment(ENV=os.environ)

platform = ARGUMENTS.get("platform", "linux")
arch = ARGUMENTS.get("arch", "x86_64")
target = ARGUMENTS.get("target", "template_debug")  

env = SConscript("godot-cpp/SConstruct", exports={"platform": platform, "arch": arch, "target": target})

env.Append(CPPPATH=["src/", "include/", "include/generated/", "godot-cpp/include", "godot-cpp/gen/include"])

# godot-cpp files, from submodule
env.Append(LIBPATH=["godot-cpp/bin"])
env.Append(LIBS=[f"godot-cpp.{platform}.{target}.{arch}"])

env.ParseConfig("pkg-config --cflags --libs wayland-client")

sources = Glob("src/*.cpp") + Glob("src/*.c")  

if target == "template_debug":
    env.Append(CXXFLAGS=["-g", "-std=c++20"])  
    env.Append(CFLAGS=["-g", "-std=c11"])      
else:
    env.Append(CXXFLAGS=["-O2", "-std=c++20"]) 
    env.Append(CFLAGS=["-O2", "-std=c11"])     


# output name
lib_name = f"bin/wayland-protocols.{platform}.{target}.{arch}"
if platform == "linux":
    lib_name += ".so"

library = env.SharedLibrary(lib_name, sources)

Default(library)
