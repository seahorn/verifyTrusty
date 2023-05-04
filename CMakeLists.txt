cmake_minimum_required(VERSION 3.18.4)
project(verifyTrusty C CXX)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvmir)
include(LLVMIRUtil)
include(CTest)

# Enable CTest
enable_testing()

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR )
  message (FATAL_ERROR
    "In-source builds are not allowed. Please clean your source tree and try again.")
endif()

# Default is release with debug info
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

set(SEAHORN_ROOT "/usr" CACHE PATH "Path to SeaHorn installation")
set(SEA_LINK "llvm-link" CACHE STRING "Path to llvm-link")
set(LLVMIR_LINK ${SEA_LINK})
set(SEA_OPT "${SEAHORN_ROOT}/bin/seaopt" CACHE STRING  "Path to seaopt binary")
set(SEA_PP "${SEAHORN_ROOT}/bin/seapp" CACHE STRING  "Path to seapp binary")
set(LLVMIR_OPT ${SEA_OPT})
set(CPPSTDLIB "libc++" CACHE STRING "C++ std library to use")

set(TRUSTY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/trusty)
set(TRUSTY_MOD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/trusty_mod)
set(TRUSTY_TARGET "arm32" CACHE STRING "Trusty target for verification")
set(EXTERNAL_ROOT ${TRUSTY_ROOT}/external)
option(HANDLE_TYPE_IS_PTR "Build SHADOW MEM versions of jobs" OFF)
# set C++ stdlib
string(APPEND CMAKE_CXX_FLAGS " -stdlib=${CPPSTDLIB}")
set (CMAKE_CXX_STANDARD 17)

# Set gcc-toolchain directory
if (${TRUSTY_TARGET} STREQUAL "arm32")
  set(TARGET_ARCH "arm")
elseif(${TRUSTY_TARGET} STREQUAL "x86_64")
  set(TARGET_ARCH "${TRUSTY_TARGET}")
else() # arm64 = aarch64
  set(TARGET_ARCH "aarch64")
endif()

set(SEA_LIB ${CMAKE_CURRENT_SOURCE_DIR}/seahorn/lib)

configure_file(verify.py.in verify @ONLY)
set(VERIFY_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/verify-trusty.sh)
set(VERIFY_CMD ${CMAKE_CURRENT_BINARY_DIR}/verify)

include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/include)

include_directories(seahorn/trusty_common/include)
# TODO: Don't include at global level
# I don't get why we have two directories trusty_common and trusty
# and why is trusty included as an implicit dep of trusty_common
if (HANDLE_TYPE_IS_PTR)
  include_directories(seahorn/trusty/user/base/lib/libc-trusty-ptr)
else ()
  include_directories(seahorn/trusty/user/base/lib/libc-trusty)
endif ()
# Include header's directories
include_directories(${TRUSTY_ROOT}/hardware/libhardware/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/interface/storage/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/interface/hwrng/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/interface/keymaster/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/interface/hwkey/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/include/uapi)
include_directories(${TRUSTY_ROOT}/trusty/user/base/include/shared)
if (HANDLE_TYPE_IS_PTR)
  include_directories(${TRUSTY_MOD_ROOT}/trusty/user/base/include/user)
else ()
  include_directories(${TRUSTY_ROOT}/trusty/user/base/include/user)
endif ()
include_directories(${TRUSTY_ROOT}/trusty/user/base/lib/libc-trusty/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/lib/libstdc++-trusty/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/lib/libcxxabi-trusty/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/lib/hwkey/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/lib/keymaster/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/lib/rng/include)
include_directories(${TRUSTY_ROOT}/trusty/user/base/lib/storage/include)
include_directories(${TRUSTY_ROOT}/trusty/kernel/include/uapi/)
include_directories(${TRUSTY_ROOT}/trusty/kernel/lib/ubsan/include)
include_directories(${TRUSTY_ROOT}/system/gatekeeper/include/)
include_directories(${TRUSTY_ROOT}/external/lk/include/shared/)
include_directories(${TRUSTY_ROOT}/external/lk/include/uapi/)


include_directories(${SEAHORN_ROOT}/include)
add_subdirectory(seahorn)
