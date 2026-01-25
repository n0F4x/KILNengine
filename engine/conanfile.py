from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.scm import Version


class DataDrivenGameEngineRecipe(ConanFile):
    name = "kiln-engine"
    version = "0.0.0"
    package_type = "library"

    # Optional metadata
    url = "https://github.com/nutellis/KILNengine"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        # TODO: "visibility": ["default", "hidden", "internal", "protected"],  # controls -fvisibility
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        # TODO: "visibility": "hidden",
    }
    implements = ["auto_shared_fpic"]
    exports_sources = (
        "lib/*",
    )

    project_prefix = "KILN_"

    @property
    def _dev(self):
        return bool(self.conf.get(f"user.{self.name}:dev", default=False))

    @property
    def _enable_tests(self):
        return self._dev and bool(self.conf.get(f"user.{self.name}:enable_tests", default=False))

    @property
    def _enable_examples(self):
        return self._dev and bool(self.conf.get(f"user.{self.name}:enable_examples", default=False))

    def validate(self):
        check_min_cppstd(self, "23")

        supported_msvc_version = 195
        if (self.settings.compiler != "msvc"
                or self.settings.compiler.version != supported_msvc_version):
            raise ConanInvalidConfiguration(f"Only MSVC with version {supported_msvc_version} is supported")

        if self.conf.get("tools.cmake.cmaketoolchain:generator") != "Ninja":
            raise ConanInvalidConfiguration("Only Ninja is supported as a generator")

        if bool(self.conf.get(f"user.{self.name}:enable_tests", default=False)) and not self._dev:
            raise ConanInvalidConfiguration(
                f"'user.{self.name}:enable_tests' requires 'user.{self.name}:dev'"
            )

        if bool(self.conf.get(f"user.{self.name}:enable_examples", default=False)) and not self._dev:
            raise ConanInvalidConfiguration(
                f"'user.{self.name}:enable_examples' requires 'user.{self.name}:dev'"
            )

    def build_requirements(self):
        self.tool_requires("cmake/[>=4.1]")

    def requirements(self):
        self.requires("fmt/12.1.0")

        if self._enable_tests:
            self.test_requires("catch2/3.12.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        # CMakeDeps
        deps = CMakeDeps(self)
        deps.generate()

        # CMakeToolChain
        tc = CMakeToolchain(self)

        if self._dev:
            tc.cache_variables[self.project_prefix + "ENABLE_TESTS"] = self._enable_tests
            tc.cache_variables[self.project_prefix + "ENABLE_EXAMPLES"] = self._enable_examples

        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_script_folder=None if self._dev else "lib")
        cmake.build()
        if self._enable_tests and not self.conf.get("tools.build:skip_test", default=False):
            self.run("tests", cwd="tests")

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "kiln")
        self.cpp_info.set_property("cmake_target_name", "kiln::engine")

        if self.settings.compiler == "msvc":
            self.cpp_info.cxxflags.append("/Zc:preprocessor")

        version = Version(self.version)
        self.cpp_info.defines.append(f"{self.project_prefix}VERSION_MAJOR={version.major}")
        self.cpp_info.defines.append(f"{self.project_prefix}VERSION_MINOR={version.minor}")
        self.cpp_info.defines.append(f"{self.project_prefix}VERSION_PATCH={version.patch}")
