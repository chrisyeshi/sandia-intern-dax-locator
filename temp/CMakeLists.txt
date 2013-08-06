cmake_minimum_required(VERSION 2.8)

project(Temp CXX)

find_package(Dax REQUIRED)

DaxConfigureSerial(REQUIRED)

add_executable(uniformgrid_numberofcells ./uniformgrid_numberofcells_test.cxx)
add_executable(executionObject ./executionObject.cxx)
add_executable(reducekeys ./reducekey.cxx)
