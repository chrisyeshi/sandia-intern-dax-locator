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

# include directories
include_directories(${Uniform_SOURCE_DIR})

add_executable(locate ./locate.cxx ./tests/RandomPoints2D.cxx ./Point2D.cxx)
add_executable(pointlocate_serial ./pointlocate.cxx ./tests/RandomPoints3D.cxx ./Point3D.cxx ./Point2D.cxx ./PointLocator.cxx ./PointLocatorExec.cxx)
add_executable(pointlocateexec_serial ./pointlocateexec.cxx ./tests/RandomPoints3D.cxx ./Point3D.cxx ./Point2D.cxx ./PointLocator.cxx ./PointLocatorExec.cxx)
add_executable(trilocate_serial ./trilocate.cxx ./tests/TriangleMesh2DGenerator.cxx ./Point2D.cxx)
add_executable(celllocate_serial ./celllocateexec.cxx)

# cuda executables
if (BUILD_CUDA)
    DaxConfigureCuda(REQUIRED)

    # copy .cxx files to .cu files for cuda compile
    configure_file(pointlocate.cxx ${CMAKE_CURRENT_BINARY_DIR}/pointlocate.cu COPYONLY)
    configure_file(PointLocator.cxx ${CMAKE_CURRENT_BINARY_DIR}/PointLocator.cu COPYONLY)
    configure_file(PointLocatorExec.cxx ${CMAKE_CURRENT_BINARY_DIR}/PointLocatorExec.cu COPYONLY)
    configure_file(pointlocateexec.cxx ${CMAKE_CURRENT_BINARY_DIR}/pointlocateexec.cu COPYONLY)
    configure_file(trilocate.cxx ${CMAKE_CURRENT_BINARY_DIR}/trilocate.cu COPYONLY)

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
    cuda_add_executable(trilocate_cuda ${CMAKE_CURRENT_BINARY_DIR}/trilocate.cu
                                       ./tests/TriangleMesh2DGenerator.cxx
                                       ./Point2D.cxx)

endif(BUILD_CUDA)
