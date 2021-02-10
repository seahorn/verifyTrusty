cmake_minimum_required(VERSION 3.18.4)
project(verifyTrusty C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvmir)
include(LLVMIRUtil)
include(CTest)
include(JSONParser.cmake)

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

set(TRUSTY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/trusty)
set(TRUSTY_BUILD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/build-root)
set(EXTERNAL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(SEA_LIB ${CMAKE_CURRENT_SOURCE_DIR}/seahorn/lib)

configure_file(verify.py.in verify @ONLY)
set(VERIFY_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/verify-trusty.sh)
set(VERIFY_CMD ${CMAKE_CURRENT_BINARY_DIR}/verify)

include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/include)

include_directories(seahorn/include)

# MACRO(HEADER_DIRECTORIES return_list dir)
#     FILE(GLOB_RECURSE new_list ${dir}/*.h)
#     SET(dir_list "")
#     FOREACH(file_path ${new_list})
#         GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
#         SET(dir_list ${dir_list} ${dir_path})
#     ENDFOREACH()
#     LIST(REMOVE_DUPLICATES dir_list)
#     SET(${return_list} ${dir_list})
# ENDMACRO()
# HEADER_DIRECTORIES(header_dir_list "trusty")
# list(LENGTH header_dir_list header_dir_list_count)
# include_directories(${header_dir_list})
include_directories(trusty/device/arm/generic-arm64/include/user)
include_directories(trusty/device/x86/generic-x86_64/include/user)
include_directories(trusty/user/app/storage)
include_directories(trusty/user/base/interface/storage/include)
include_directories(trusty/user/base/include/uapi)
include_directories(trusty/user/base/include/shared)
include_directories(trusty/user/base/include/user)
include_directories(trusty/kernel/include/uapi/)
include_directories(external/lk/include/shared/)
include_directories(external/lk/include/uapi/)
include_directories(${SEAHORN_ROOT}/include)
add_subdirectory(seahorn)