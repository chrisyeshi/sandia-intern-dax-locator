DaxConfigureCuda(REQUIRED)

add_executable(point2d_test_serial point2d_test.cxx ../../Point2D.cxx)
add_test(point2d_test_serial COMMAND ./point2d_test_serial)

add_executable(randompoints_serial ./randompoints_test.cxx ../RandomPoints2D.cxx ../RandomPoints2D.h ../../Point2D.cxx ./stdnormal_test.cxx)
add_test(randompoints_serial COMMAND ./randompoints_serial)

add_executable(randompoints_default_serial ./randompoints_default_test.cxx ../RandomPoints2D.cxx ../../Point2D.cxx ./stdnormal_test.cxx)
add_test(randompoints_default_serial command ./randompoints_default_serial)

add_executable(gridspace_serial ./gridspace_test.cxx)
add_test(gridspace_serial COMMAND ./gridspace_serial)

file(COPY ./daxlocator_correct_output.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_executable(daxlocator_output_test_serial ./daxlocator_output_test.cxx ../RandomPoints2D.cxx ../../Point2D.cxx ../../DaxLocator.cxx)
add_test(daxlocator_output_serial ./daxlocator_output_test_serial)

cuda_add_executable(daxlocator_output_test_cuda ./daxlocator_output_test.cxx ../RandomPoints2D.cxx ../../Point2D.cxx ../../DaxLocator.cxx)
add_test(daxlocator_output_cuda ./daxlocator_output_test_cuda)

add_executable(uniformgrid_numberofcells_test_serial ./uniformgrid_numberofcells_test.cxx)
