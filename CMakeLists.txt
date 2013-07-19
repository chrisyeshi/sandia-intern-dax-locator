cmake_minimum_required(VERSION 2.8)

project(uniform CXX)

find_package(Dax REQUIRED)

DaxConfigureCuda(REQUIRED)
DaxConfigureSerial(REQUIRED)

enable_testing()

add_subdirectory(tests)
add_subdirectory(temp)

add_executable(locate ./locate.cxx ./tests/RandomPoints2D.cxx ./Point2D.cxx)

add_executable(daxlocate_serial ./daxlocate.cxx ./tests/RandomPoints2D.cxx ./Point2D.cxx ./BinPoints.h ./DaxLocator.cxx ./ExecLocator.cxx ./BinPoints.cxx)
cuda_add_executable(daxlocate_cuda ./daxlocate.cxx ./tests/RandomPoints2D.cxx ./Point2D.cxx ./BinPoints.h ./DaxLocator.cxx ./ExecLocator.cxx ./BinPoints.cxx)

add_executable(execlocate_serial ./execlocate.cxx ./tests/RandomPoints2D.cxx ./Point2D.cxx ./DaxLocator.cxx ./ExecLocator.cxx ./BinPoints.cxx)
