cmake_minimum_required(VERSION 3.22)
project(expressions VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)

set(KYDEPS_BOILERPLATE_CLANG_TIDY OFF)
set(KYDEPS_TARGETS glog GTest)

include(FetchContent)
FetchContent_Declare(
    kydeps_boilerplate
    GIT_REPOSITORY https://github.com/kydeps/boilerplate.git
    GIT_TAG feature/kydeps)
FetchContent_MakeAvailable(kydeps_boilerplate)
KyDepsAddBoilerplate()

add_subdirectory(src)
