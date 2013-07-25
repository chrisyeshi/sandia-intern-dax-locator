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
add_executable(pointlocate_serial ./pointlocate.cxx ./tests/RandomPoints3D.cxx ./Point3D.cxx ./Point2D.cxx ./PointLocator.cxx ./PointLocatorExec.cxx)
add_executable(pointlocateexec_serial ./pointlocateexec.cxx ./tests/RandomPoints3D.cxx ./Point3D.cxx ./Point2D.cxx ./PointLocator.cxx ./PointLocatorExec.cxx)

# cuda executables
if (BUILD_CUDA)
    DaxConfigureCuda(REQUIRED)

    # copy .cxx files to .cu files for cuda compile
    configure_file(pointlocate.cxx ${CMAKE_CURRENT_BINARY_DIR}/pointlocate.cu COPYONLY)
    configure_file(PointLocator.cxx ${CMAKE_CURRENT_BINARY_DIR}/PointLocator.cu COPYONLY)
    configure_file(PointLocatorExec.cxx ${CMAKE_CURRENT_BINARY_DIR}/PointLocatorExec.cu COPYONLY)
    configure_file(pointlocateexec.cxx ${CMAKE_CURRENT_BINARY_DIR}/pointlocateexec.cu COPYONLY)

    cuda_add_executable(pointlocate_cuda ${CMAKE_CURRENT_BINARY_DIR}/pointlocate.cu
                                       ./tests/RandomPoints3D.cxx
                                       ./Point3D.cxx
                                       ./Point2D.cxx
                                       ${CMAKE_CURRENT_BINARY_DIR}/PointLocator.cu
                                       ${CMAKE_CURRENT_BINARY_DIR}/PointLocatorExec.cu)
    cuda_add_executable(pointlocateexec_cuda ${CMAKE_CURRENT_BINARY_DIR}/pointlocateexec.cu
                                        ./tests/RandomPoints3D.cxx
                                        ./Point3D.cxx
                                        ./Point2D.cxx
                                        ${CMAKE_CURRENT_BINARY_DIR}/PointLocator.cu
                                        ${CMAKE_CURRENT_BINARY_DIR}/PointLocatorExec.cu)
endif(BUILD_CUDA)
