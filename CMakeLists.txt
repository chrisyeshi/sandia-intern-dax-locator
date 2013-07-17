cmake_minimum_required(VERSION 2.8)

project(uniform CXX)

find_package(Dax REQUIRED)

DaxConfigureCuda(REQUIRED)
DaxConfigureSerial(REQUIRED)

enable_testing()

add_subdirectory(tests)

add_executable(locate ./locate.cxx ./tests/RandomPoints2D.cxx ./Point2D.cxx)

add_executable(daxlocate ./daxlocate.cxx ./tests/RandomPoints2D.cxx ./Point2D.cxx ./BinPoints.h ./DaxLocator.cxx)
