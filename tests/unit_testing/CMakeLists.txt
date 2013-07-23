DaxConfigureCuda(REQUIRED)

# include directories
include_directories(${Uniform_SOURCE_DIR})

# tests
add_executable(point2d_test_serial point2d_test.cxx ../../Point2D.cxx)
add_test(point2d_test_serial COMMAND ./point2d_test_serial)

add_executable(randompoints_serial ./randompoints_test.cxx ../RandomPoints2D.cxx ../RandomPoints2D.h ../../Point2D.cxx ./stdnormal_test.cxx)
add_test(randompoints_serial COMMAND ./randompoints_serial)

add_executable(randompoints_default_serial ./randompoints_default_test.cxx ../RandomPoints2D.cxx ../../Point2D.cxx ./stdnormal_test.cxx)
add_test(randompoints_default_serial command ./randompoints_default_serial)

add_executable(gridspace_serial ./gridspace_test.cxx)
add_test(gridspace_serial COMMAND ./gridspace_serial)

# daxlocator_output_test
configure_file(daxlocator_output_test.cxx ${CMAKE_CURRENT_BINARY_DIR}/daxlocator_output_test.cu COPYONLY)
file(COPY ./daxlocator_correct_output.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(daxlocator_output_test_serial ./daxlocator_output_test.cxx ../RandomPoints2D.cxx ../../Point2D.cxx ../../DaxLocator.cxx ../../ExecLocator.cxx)
cuda_add_executable(daxlocator_output_test_cuda
                    ${CMAKE_CURRENT_BINARY_DIR}/daxlocator_output_test.cu
                    ../RandomPoints2D.cxx
                    ../../Point2D.cxx
                    ${CMAKE_CURRENT_BINARY_DIR}/../../DaxLocator.cu
                    ${CMAKE_CURRENT_BINARY_DIR}/../../ExecLocator.cu)

add_test(daxlocator_output_serial ./daxlocator_output_test_serial)
add_test(daxlocator_output_cuda ./daxlocator_output_test_cuda)

# execlocate_output_test
configure_file(execlocate_output_test.cxx ${CMAKE_CURRENT_BINARY_DIR}/execlocate_output_test.cu COPYONLY)
file(COPY ./execlocate_correct_output.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(execlocate_output_test_serial ./execlocate_output_test.cxx ../RandomPoints2D.cxx ../../Point2D.cxx ../../DaxLocator.cxx ../../ExecLocator.cxx)
cuda_add_executable(execlocate_output_test_cuda
                    ${CMAKE_CURRENT_BINARY_DIR}/execlocate_output_test.cu
                    ../RandomPoints2D.cxx
                    ../../Point2D.cxx
                    ${CMAKE_CURRENT_BINARY_DIR}/../../DaxLocator.cu
                    ${CMAKE_CURRENT_BINARY_DIR}/../../ExecLocator.cu)

add_test(execlocate_output_test_serial ./execlocate_output_test_serial)
add_test(execlocate_output_test_cuda ./execlocate_output_test_cuda)
