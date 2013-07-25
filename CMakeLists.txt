cmake_minimum_required(VERSION 2.8)

project(Uniform CXX)

find_package(Dax REQUIRED)

set(BUILD_CUDA ON CACHE BOOL "Build serial executables only?")
set(BUILD_TESTS ON CACHE BOOL "Build tests?")

DaxConfigureSerial(REQUIRED)

# enable testing
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif (BUILD_TESTS)

add_subdirectory(temp)

# include directories
include_directories(${Uniform_SOURCE_DIR})

add_executable(locate ./locate.cxx ./tests/RandomPoints2D.cxx ./Point2D.cxx)
add_executable(daxlocate_serial ./daxlocate.cxx ./tests/RandomPoints3D.cxx ./Point3D.cxx ./Point2D.cxx ./DaxLocator.cxx ./ExecLocator.cxx)
add_executable(execlocate_serial ./execlocate.cxx ./tests/RandomPoints3D.cxx ./Point3D.cxx ./Point2D.cxx ./DaxLocator.cxx ./ExecLocator.cxx)

# cuda executables
if (BUILD_CUDA)
    DaxConfigureCuda(REQUIRED)

    # copy .cxx files to .cu files for cuda compile
    configure_file(daxlocate.cxx ${CMAKE_CURRENT_BINARY_DIR}/daxlocate.cu COPYONLY)
    configure_file(DaxLocator.cxx ${CMAKE_CURRENT_BINARY_DIR}/DaxLocator.cu COPYONLY)
    configure_file(ExecLocator.cxx ${CMAKE_CURRENT_BINARY_DIR}/ExecLocator.cu COPYONLY)
    configure_file(execlocate.cxx ${CMAKE_CURRENT_BINARY_DIR}/execlocate.cu COPYONLY)

    cuda_add_executable(daxlocate_cuda ${CMAKE_CURRENT_BINARY_DIR}/daxlocate.cu
                                       ./tests/RandomPoints3D.cxx
                                       ./Point3D.cxx
                                       ./Point2D.cxx
                                       ${CMAKE_CURRENT_BINARY_DIR}/DaxLocator.cu
                                       ${CMAKE_CURRENT_BINARY_DIR}/ExecLocator.cu)
    cuda_add_executable(execlocate_cuda ${CMAKE_CURRENT_BINARY_DIR}/execlocate.cu
                                        ./tests/RandomPoints3D.cxx
                                        ./Point3D.cxx
                                        ./Point2D.cxx
                                        ${CMAKE_CURRENT_BINARY_DIR}/DaxLocator.cu
                                        ${CMAKE_CURRENT_BINARY_DIR}/ExecLocator.cu)
endif(BUILD_CUDA)
