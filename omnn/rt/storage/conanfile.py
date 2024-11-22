from conan import ConanFile

class OpenMindStorageConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "hiredis/1.1.0"
    generators = "CMakeDeps"
    default_options = {
        "hiredis/*:shared": True,
        "hiredis/*:ssl": True
    }

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="lib", src="lib")
        self.copy("*.so*", dst="lib", src="lib")
