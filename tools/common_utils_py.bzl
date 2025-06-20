"Defines macros for particular build options that we choose."

load("@pip//:requirements.bzl", "requirement")
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")
load("@rules_python//python:defs.bzl", "py_test")
load("@rules_python//python:packaging.bzl", "py_wheel")
load("//tools:common_utils_cc.bzl", "default_copts", "include_dirs")

def py_include_dirs():
    return include_dirs() + [
        "-Ipybind",
    ]

def py_default_copts():
    return default_copts() + []

def py_default_deps():
    return select({
        "@platforms//os:linux": [
            # "@local_cuda//:cuda_runtime", # Example if bindings directly use CUDA runtime
            # "@local_cuda//:cusolver",     # Example if bindings directly use cuSOLVER
        ],
        "//conditions:default": [],
    })

def common_utils_pybind_extension(
        name,
        copts = [],
        features = [],
        linkopts = [],
        tags = [],
        deps = [],
        **kwargs):
    """ A wrapper around pybind_extension that adds default options to copts and deps.

    See documentation for pybind_extension for more details.
    """
    pybind_extension(
        name = name,
        copts = py_default_copts() + py_include_dirs() + copts,
        features = features,
        linkopts = linkopts,
        tags = tags,
        deps = py_default_deps() + deps,
        **kwargs
    )

def common_utils_py_wheel(name, twine = None, publish_args = [], **kwargs):
    """ A wrapper around py_wheel that adds default options to requires.

    If no platform argument is provided in kwargs, then a default option is put in.
    If no python tag is provided, a default one is provided.
    See documentation for pybind_wheel for more details.
    Args:
        name: The name of the wheel.
        twine: The twine command to use for publishing.
        publish_args: Additional arguments to pass to the publish command.
        **kwargs: Additional arguments to pass to py_wheel
    """
    requires = kwargs.pop("requires", [])
    requires += [
        "numpy>=2",
        "setuptools",
    ]
    platform = kwargs.pop("platform", select({
        "@platforms//os:linux": "manylinux2014_x86_64",
        "//conditions:default": "any",  # Or specific non-Linux platform
    }))
    python_tag = kwargs.pop(
        "python_tag",
        "py3",
    )
    py_wheel(
        name = name,
        twine = twine,
        publish_args = publish_args,
        platform = platform,
        requires = requires,
        python_tag = python_tag,
        **kwargs
    )

def py_test_default_deps():
    return [requirement("numpy")]

def common_utils_py_test(*args, **kwargs):
    """ A wrapper around py_test that adds default options to deps.

    See documentation for py_test for more details.
    Args:
        *args: Additional arguments to pass to py_test
        **kwargs: Additional keyword arguments to pass to py_test
    """
    deps = kwargs.pop("deps", [])
    deps += py_test_default_deps()
    return py_test(
        deps = deps,
        *args,
        **kwargs
    )
