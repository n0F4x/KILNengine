# How to build the project

## Prerequisites

Make sure you have the following programs installed on your machine

-   Clang (at least version 20)
-   Ninja
-   CMake (at least version 4.1)
-   [Conan](https://docs.conan.io/2/installation.html)

## Setup

### Create a Conan [profile](https://docs.conan.io/2/reference/config_files/profiles.html#profiles)

#### Example Conan profile for Windows:

```ini
[settings]
os=Windows
arch=x86_64
build_type=Debug
compiler=clang
compiler.cppstd=23
compiler.version=20

[conf]
tools.build:compiler_executables={'c':'clang.exe','cpp':'clang++.exe'}
tools.cmake.cmaketoolchain:generator=Ninja
user.kiln-engine:dev=True
user.kiln-engine:debug=True
user.kiln-engine:enable_tests=True
user.kiln-engine:enable_examples=True

[buildenv]
PATH+=(path)C:/Program Files/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin
PATH+=(path)C:/Program Files/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja
PATH+=(path)C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/Llvm/x64/bin

[platform_tool_requires]
cmake/[>=4.1]
```

#### Example Conan profile for MSYS2 (Linux is very similar):

```ini
[settings]
os=Windows
os.subsystem=msys2
arch=x86_64
build_type=Debug
compiler=clang
compiler.cppstd=23
compiler.version=20
compiler.libcxx=libstdc++11

[conf]
tools.build:compiler_executables={'c':'C:/msys64/ucrt64/bin/clang.exe','cpp':'C:/msys64/ucrt64/bin/clang++.exe'}
tools.cmake.cmaketoolchain:generator=Ninja
user.kiln-engine:dev=True
user.kiln-engine:debug=True
user.kiln-engine:enable_tests=True
user.kiln-engine:enable_examples=True

[buildenv]
PATH+=(path)C:/msys64/ucrt64/bin

[platform_tool_requires]
cmake/[>=4.1]
```

### Install the dependencies

Run `conan install . -b=missing --profile:host=<custom_profile> --profile:build=<custom_profile>`.

This will produce a _CMakeUserPresets.json_ file inside the working directory that can be used to build the project (with your IDE).

## Caveats

### Conan dev variables

Please use `user.kiln-engine:dev=True` in your profile, otherwise you won't be able to properly build and contribute to development. \
`debug`, `enable_tests` and `enable_examples` are used for setting the `${project_prefix}X` CMake variable (where X is either `DEBUG`, `ENABLE_TESTS` or `ENABLE_EXAMPLES`).

### Project local Conan profile

You can place your Conan profile file inside the project directory (e.g. _./.conan2/profiles/customprofile_).
Use the relative path to your profile when executing Conan commands.

### CLion set toolchain

CLion is a bit weird when it comes to build profiles using cmake-presets.
If you can't change the toolchain for the profile, make a copy of it.
This way, you are allowed to configure it.

### Test the library as a package

It is important that the library can also be consumed as a third-party package.

Run `conan export .` to export the package to the local cache.

Run `conan test kiln-engine/<version> -b=missing --profile:host=<custom_test_profile> --profile:build=<custom_test_profile>` to test the package in the local cache.

Make sure not to declare `user.kiln-engine:dev` as `True` in your Conan profile used for testing.
