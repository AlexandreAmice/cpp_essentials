"Defines macros for adding copts to libraries."

load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("@rules_cuda//cuda:defs.bzl", "cuda_binary", "cuda_library", "cuda_test")

cpp_version = "c++20"

def default_copts():
    return [
        "-std=" + cpp_version,
        "-Wall",
        "-Wextra",
        "-Werror",
        "-Wno-unused-parameter",
        "-Wno-missing-field-initializers",
    ]

def cuda_host_default_copts():
    return [
        # "-v",
        "-std=" + cpp_version,  # ensure this is the same as in cuda_device_default_copts
        "-Wall",
        "-Wextra",
        "-Werror",
        "-Wno-unused-parameter",
        "-Wno-missing-field-initializers",
        "-Wno-tautological-compare",  # ignore tautological compare warngings which happens due to expansion of templates in nvcc.
    ]

def cuda_device_default_copts():
    return [
        "-Wno-deprecated-gpu-targets",  # Suppress gpu offline compilation deprecation notice.
        "--std=" + cpp_version,  # ensure this is the same as in cuda_device_default_copts
        "--diag-suppress=20012",  # Suppress warning about __host__ on defaulted function. Eigen Build noise
        "--diag-suppress=128",  # Suppress warning about __host__ on defaulted function. fmt noise
        "--diag-suppress=2417",  # Suppress warning about __host__ on defaulted function. fmt noise
        "--diag-suppress=20014",  # Suppress warning calling __host__ from __host__ __device__ function. Eigen noise when using Eigen functions in host code from cu files.
    ]

def include_dirs():
    return [
        "-Isrc",  # Adjust this to your needs
    ]

def cuda_include_dirs():
    return [
        "-Isrc",  # Adjust this to your needs
    ]

def default_test_deps():
    return [
        "@googletest//:gtest_main",  # Link against Google Test
    ]

def default_cuda_deps():
    return [
        # "@local_cuda//:cuda_runtime",
        "@cuda//:cuda_runtime",
        "//utils:cuda_utils",
    ]

def default_cuda_test_deps():
    return default_cuda_deps() + default_test_deps()

def common_utils_cc_library(name, copts = [], **kwargs):
    include_prefix = kwargs.pop("include_prefix", "cpp_essentials/utils")
    cc_library(
        name = name,
        copts = default_copts() + include_dirs() + copts,
        # Done so downstream libraries will include this library via angled brackets
        include_prefix = include_prefix,
        **kwargs
    )

def common_utils_cc_binary(name, copts = [], **kwargs):
    cc_binary(
        name = name,
        copts = default_copts() + include_dirs() + copts,
        **kwargs
    )

def common_utils_cc_test(name, copts = [], deps = [], srcs = [], size = None, **kwargs):
    if size == None:
        size = "small"
    if not srcs:
        srcs = ["test/%s.cc" % name]
    cc_test(
        name = name,
        srcs = srcs,
        size = size,
        copts = default_copts() + include_dirs() + copts,
        deps = default_test_deps() + deps,
        **kwargs
    )

def common_utils_cuda_library(name, host_copts = [], device_copts = [], deps = [], **kwargs):
    # cuda library has no concept of implementation deps see here: https://github.com/bazel-contrib/rules_cuda/blob/main/cuda/private/rules/cuda_library.bzl
    cuda_library(
        name = name,
        host_copts = cuda_host_default_copts() + cuda_include_dirs() + host_copts,
        copts = cuda_device_default_copts() + device_copts,
        deps = default_cuda_deps() + deps,
        **kwargs
    )

def common_utils_cuda_binary(name, host_copts = [], device_copts = [], deps = [], **kwargs):
    cuda_binary(
        name = name,
        host_copts = cuda_host_default_copts() + cuda_include_dirs() + host_copts,
        copts = cuda_device_default_copts() + device_copts,
        deps = default_cuda_deps() + deps,
        **kwargs
    )

def common_utils_cuda_test(name, host_copts = [], device_copts = [], deps = [], **kwargs):
    srcs = kwargs.pop("srcs", ["test/%s.cu" % name])

    cuda_test(
        name = name,
        srcs = srcs,
        host_copts = cuda_host_default_copts() + cuda_include_dirs() + host_copts,
        copts = cuda_device_default_copts() + device_copts,
        deps = default_cuda_test_deps() + deps,
        **kwargs
    )
