from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
import os
import sys
import subprocess
import glob


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            subprocess.check_call(["cmake", "--version"])
        except OSError:
            raise RuntimeError("CMake must be installed to build the extension")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        # Use the parent directory where the main CMakeLists.txt file is located
        sourcedir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

        cmake_args = [
            "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" + extdir,
            "-DBUILD_PYTHON_WRAPPER=OFF",
            "-DBUILD_PYTHON3_WRAPPER=OFF",
            "-DBUILD_TESTS=OFF",
            "-DBUILD_BENCHMARKS=OFF",
            "-DBUILD_SHARED_LIBS=ON",
            "-DPYTHON_EXECUTABLE=" + sys.executable,
            "-DCMAKE_BUILD_TYPE=Release",
        ]

        # Configuration
        build_temp = os.path.join(
            os.path.dirname(os.path.abspath(__file__)), "build_temp"
        )
        if not os.path.exists(build_temp):
            os.makedirs(build_temp)

        # Set environment variables for build
        env = os.environ.copy()

        # Run CMake to configure and build
        subprocess.check_call(
            ["cmake", sourcedir] + cmake_args, cwd=build_temp, env=env
        )
        subprocess.check_call(
            ["cmake", "--build", ".", "--config", "Release"], cwd=build_temp, env=env
        )

        # Find and copy the shared library
        print("Searching for library files...")
        current_dir = os.path.dirname(os.path.abspath(__file__))

        if sys.platform == "darwin":
            lib_name = "libsoftposit.dylib"
        else:
            lib_name = "libsoftposit.so"

        # Look in build directory
        print(f"Looking for {lib_name}...")
        result = subprocess.run(
            ["find", "..", "-name", lib_name], capture_output=True, text=True
        )
        lib_paths = result.stdout.strip().split("\n")

        if lib_paths and lib_paths[0]:
            lib_path = lib_paths[0]
            print(f"Found library at: {lib_path}")
            print(f"Copying to {current_dir}")
            subprocess.check_call(["cp", lib_path, current_dir])
        else:
            raise RuntimeError(
                f"Could not find {lib_name} in any of the expected locations"
            )

        print(f"Build complete, files in {extdir}")


setup(
    name="softposit",
    version="1.0.0",
    # author="SoftPosit Team",
    # author_email="softposit@gmail.com",
    description="Python bindings for SoftPosit - Posit number arithmetic library",
    long_description=open(
        os.path.join(os.path.dirname(os.path.abspath(__file__)), "README.md")
    ).read(),
    long_description_content_type="text/markdown",
    url="https://github.com/Posit-Foundation/soft-posit-cpp",
    ext_modules=[CMakeExtension("_softposit")],
    py_modules=["softposit"],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    python_requires=">=3.6",
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "Topic :: Scientific/Engineering :: Mathematics",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
    ],
)
