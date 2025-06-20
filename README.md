# CommonUtils

Collects useful common cpp utilities based on the ones found in Drake's common and essentials.

# Install

If you're getting red squiggles in vscode try running `bazel run @hedron_compile_commands//:refresh_all`

You need to have `nvidia-toolkit` and `nvidia-drivers` installed. Make sure you can `echo $CUDA_PATH` to a `cuda` folder and that you can call `nvidia-smi`.

### TODO

#### Automate Setup

Need to have

1. bazelisk/bazel
1. poetry
1. clang
1. buildifier
