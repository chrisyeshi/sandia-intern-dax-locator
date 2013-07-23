cmake_minimum_required(VERSION 2.8)

project(Uniform CXX)

find_package(Dax REQUIRED)

DaxConfigureCuda(REQUIRED)
DaxConfigureSerial(REQUIRED)

enable_testing()

add_subdirectory(tests)
add_subdirectory(temp)

# include directories
include_directories(${Uniform_SOURCE_DIR})

# copy .cxx files to .cu files for cuda compile
configure_file(daxlocate.cxx ${CMAKE_CURRENT_BINARY_DIR}/daxlocate.cu COPYONLY)
configure_file(DaxLocator.cxx ${CMAKE_CURRENT_BINARY_DIR}/DaxLocator.cu COPYONLY)
configure_file(ExecLocator.cxx ${CMAKE_CURRENT_BINARY_DIR}/ExecLocator.cu COPYONLY)
configure_file(execlocate.cxx ${CMAKE_CURRENT_BINARY_DIR}/execlocate.cu COPYONLY)

add_executable(locate ./locate.cxx ./tests/RandomPoints2D.cxx ./Point2D.cxx)

add_executable(daxlocate_serial ./daxlocate.cxx ./tests/RandomPoints2D.cxx ./Point2D.cxx ./DaxLocator.cxx ./ExecLocator.cxx)
cuda_add_executable(daxlocate_cuda ${CMAKE_CURRENT_BINARY_DIR}/daxlocate.cu
                                   ./tests/RandomPoints2D.cxx
                                   ./Point2D.cxx
                                   ${CMAKE_CURRENT_BINARY_DIR}/DaxLocator.cu
                                   ${CMAKE_CURRENT_BINARY_DIR}/ExecLocator.cu)

add_executable(execlocate_serial ./execlocate.cxx ./tests/RandomPoints2D.cxx ./Point2D.cxx ./DaxLocator.cxx ./ExecLocator.cxx)
cuda_add_executable(execlocate_cuda ${CMAKE_CURRENT_BINARY_DIR}/execlocate.cu
                                    ./tests/RandomPoints2D.cxx
                                    ./Point2D.cxx
                                    ${CMAKE_CURRENT_BINARY_DIR}/DaxLocator.cu
                                    ${CMAKE_CURRENT_BINARY_DIR}/ExecLocator.cu)
