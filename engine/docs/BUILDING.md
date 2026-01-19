# How to build the project

## Prerequisites

Make sure you have the following programs installed on your machine

-   MSVC (at least version v195)
-   Ninja
-   CMake (at least version 4.1)
-   [Conan](https://docs.conan.io/2/installation.html)

## Setup

### Create a Conan [profile](https://docs.conan.io/2/reference/config_files/profiles.html#profiles)

Example Conan profile:

```
[settings]
arch=x86_64
build_type=Debug
compiler=msvc
compiler.cppstd=23
compiler.runtime=dynamic
compiler.version=195
os=Windows

[conf]
tools.cmake.cmaketoolchain:generator=Ninja
user.kiln-engine:dev=True
user.kiln-engine:enable_tests=True
user.kiln-engine:enable_examples=True

[buildenv]
PATH+=(path)C:/Users/Name/AppData/Local/Programs/CLion/bin/cmake/win/x64/bin
PATH+=(path)C:/Users/Name/AppData/Local/Programs/CLion/bin/ninja/win/x64
```

### Install the dependencies

Run `conan install . -b=missing --profile:host=customprofile --profile:build=customprofile`.

This will produce a _CMakeUserPresets.json_ file inside the working directory that can be used to build the project (with your IDE).

## Caveats

### Project local Conan profile

You can place your Conan profile file inside the project directory (e.g. _./.conan2/profiles/customprofile_).
Use the relative path to your profile when executing Conan commands.

### Test the library as a package

It is important that the library can also be consumed as a third-party package.

Run `conan export .` to export the package to the local cache.

Run `conan test kiln-engine/version --profile:host=customtestprofile --profile:build=customtestprofile` to test the package in the local cache.

Make sure not to declare `user.kiln-engine:dev` as `True` in your Conan profile used for testing.
