# set the version and the name
cmake_minimum_required(VERSION 2.8)
project(SRBenchMark)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -O3 -fPIC -Wall -g -Wno-unused-result -Wno-unused-but-set-variable -Wno-sign-compare")

message (${CMAKE_CXX_FLAGS_RELEASE})
include_directories(./include)
include_directories(./include/arma)
include_directories(./include/google)

# include the whole file
aux_source_directory(./core DIR_SRCS)

# add the executable files
add_executable(SRBenchMark ${DIR_SRCS})

target_link_libraries(SRBenchMark
    lapack
    blas
    pthread)
