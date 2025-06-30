"""Generate .pyi stub files for the common_utils_bindings module using mypy.stubgen."""

import sys

from mypy import stubgen


def generate_stubs(module_dir):
    """Generate .pyi stub files for the common_utils_bindings module."""
    # Add the module directory to Python path
    sys.path.insert(0, module_dir)

    # Configure stubgen options
    options = ["--module", "cpp_essentials", "--output", module_dir, "--verbose"]

    # Generate stubs
    stubgen.main(options)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(sys.argv)
        print("Usage: python generate_stubs.py <module_directory>")
        sys.exit(1)

    generate_stubs(sys.argv[1])
