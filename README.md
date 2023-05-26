# Verifying TEE applications with SeaHorn

![os](https://img.shields.io/badge/os-linux-orange?logo=linux)

This is a custom branch to host a comparison between a `fake` and a `vMock`.
Specific instructions are [here](#benchmark).
However, the project must be setup so we continue with usual steps.



Wiki and useful background materials: https://github.com/agurfinkel/verifyTrusty/wiki

## Background
Trusted Execution Environment(TEE) provides physically separate hardware for storing and processing sensitive data
With TEEs, even a compromised OS cannot access and leak sensitive data
Applications running on TEEs are juicy attack targets. The goal of this project is to apply formal verification techniques on applications running on TEEs with the state-of-the-art framework [SeaHorn](https://github.com/seahorn/seahorn).

## Setup
All harnesses and stubs within this repository depend on the *Trusty* repository. To run verification jobs locally, follow steps below to install/build missing dependencies and trusty:

#### Dependencies
- `clang-10.0` and `llvm-link-10.0`
- [Repo](https://source.android.com/setup/build/downloading#installing-repo)
- SeaHorn, use [docker image](http://seahorn.github.io/seahorn/install/docker/2018/02/24/seahorn-with-docker.html) or [build from source](http://seahorn.github.io/seahorn/install/2016/10/14/install-seahorn.html) then set `$SEA` or `$SEAHORN` environment variable to `<path_to_build_dir>/run/bin/sea` executable.
- If `libc++-10-dev` is not available, you can use `GNU libstdc++` by adding the following option to `cmake`
 ```
 -DCPPSTDLIB="libstdc++"
 ```
#### Install and build trusty

1. Clone this repository (change your diretory into verifyTrusty)

2. [download and install trusty](https://source.android.com/security/trusty/download-and-build):
    ```
    mkdir trusty && cd trusty && \
    repo init -u https://github.com/seahorn/verifyTrusty.git -b master && \
    repo sync -j32 && cd .. \
    ```

3. Using CMake to build LLVM assembly:
    ```
    mkdir build && cd build && cmake \
   -DSEA_LINK=llvm-link-14 \
   -DCMAKE_C_COMPILER=clang-14 \
   -DCMAKE_CXX_COMPILER=clang++-14 \
   -DSEAHORN_ROOT=<SEAHORN_ROOT> -DTRUSTY_TARGET=<TRUSTY_TARGET> \
   ../ -GNinja
    ```
    Note that, the *trusty target* now supports `arm32`, `arm64`, and `x86_64`. If LLVM bitcode generation is successful, you should see `<BC_FILE_NAME>.ir.bc` files under `seahorn/jobs/<job_name>/llvm-ir/<job_name>.ir`.
4. Compile
    ```
    ninja
    ```
    or
    ```
    cmake --build .
    ```
5. Verify as unit test
    ```
    ninja test
    ```
    or
    ```
    cmake --build . --target test
    ```
6. Run individual file manually
    ```
    ./verify [option] <BC_FILE_NAME>
    ```

### Current examples (under `seahorn/jobs/`)
1. `storage_ipc_port_create_destroy` simple example that shows `SeaHorn` can
   model simple ipc functions in the `storage` app like `ipc_port_create` and
   `ipc_port_destroy`; this example also shows that stubbing of handles table
   (`seahorn/lib/handle_table.c`) works.

    - Verification command: `./verify seahorn/jobs/storage_ipc_port_create_destroy`
    - Expected output: `unsat`, meaning no `sassert` is not violated.

2. `storage_ipc_indirect_handlers` the `storage` application use function
   pointers extensively for port/channel event handlers. This example
   demonstrates that `SeaHorn` can model this programming pattern by applying
   its function devirtualization pass.

    - Verification command: `./verify seahorn/jobs/storage_ipc_indirect_handlers`
    - Expected output: `unsat`, meaning no `sassert` is not violated.

3. `storage_ipc_msg_buffer` test potential buffer overflow on `msg_buf` by stubbing `realloc`.

    - Verification command: `./verify seahorn/jobs/storage_ipc_msg_buffer`
    - Expected output: `unsat`, meaning no overflow is not possible. 
    - Try removing `return ERR_NOT_ENOUGH_BUFFER` block on line `150` in
      `ipc.c`, and rebuild the verification example. Doing so should
      result in `sat` because now overflow is possible.



<a name="benchmark"></a>
### Fake env vs vMock env benchmark

#### For fake 

1. Use CMake to build LLVM assembly:
    ```
    mkdir build_fake && cd build_fake && cmake \
   -DSEA_LINK=llvm-link-14 \
   -DCMAKE_C_COMPILER=clang-14 \
   -DCMAKE_CXX_COMPILER=clang++-14 \
   -DSEAHORN_ROOT=<SEAHORN_ROOT> -DTRUSTY_TARGET=x86_64 \
   -DHANDLE_TYPE_IS_PTR=OFF
   ../ -GNinja
   ```
   
1. Compile and verify (in the `build_fake` dir) 
   ```
   ninja && ./verify seahorn/storage/jobs/ipc/msg_buffer/
   ```
#### For vMock

1. Use CMake to build LLVM assembly:
    ```
    mkdir build_vmock && cd build_vmock && cmake \
   -DSEA_LINK=llvm-link-14 \
   -DCMAKE_C_COMPILER=clang-14 \
   -DCMAKE_CXX_COMPILER=clang++-14 \
   -DSEAHORN_ROOT=<SEAHORN_ROOT> -DTRUSTY_TARGET=x86_64 \
   -DHANDLE_TYPE_IS_PTR=ON
   ../ -GNinja
   ```
   
1. Compile and verify (in the `build_vmock` dir) 
   ```
   ninja && ./verify seahorn/storage/jobs/ipc/ipc_unit
   ```

