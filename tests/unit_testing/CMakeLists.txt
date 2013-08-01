DaxConfigureCuda(REQUIRED)

# include directories
include_directories(${Uniform_SOURCE_DIR})

# tests
add_executable(point2d_test_serial point2d_test.cxx ../../Point2D.cxx)
add_test(point2d_test_serial COMMAND ./point2d_test_serial)

add_executable(randompoints_serial ./randompoints_test.cxx ../RandomPoints2D.cxx ../../Point2D.cxx ./stdnormal_test.cxx)
add_test(randompoints_serial COMMAND ./randompoints_serial)

add_executable(randompoints_3d_serial ./randompoints_3d_test.cxx ../RandomPoints3D.cxx ../../Point3D.cxx ../../Point2D.cxx ./stdnormal_test.cxx)
add_test(randompoints_3d_serial command ./randompoints_3d_serial)

add_executable(randompoints_default_serial ./randompoints_default_test.cxx ../RandomPoints2D.cxx ../../Point2D.cxx ./stdnormal_test.cxx)
add_test(randompoints_default_serial command ./randompoints_default_serial)

add_executable(randompoints_3d_default_serial ./randompoints_3d_default_test.cxx ../RandomPoints3D.cxx ../../Point3D.cxx ../../Point2D.cxx ./stdnormal_test.cxx)
add_test(randompoints_3d_default_serial command ./randompoints_3d_default_serial)

add_executable(gridspace_serial ./gridspace_test.cxx)
add_test(gridspace_serial COMMAND ./gridspace_serial)

# daxlocator_output_test
file(COPY ./daxlocator_correct_output.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(daxlocator_output_test_serial ./daxlocator_output_test.cxx
               ../RandomPoints3D.cxx ../../Point3D.cxx ../../Point2D.cxx ../../PointLocator.cxx ../../PointLocatorExec.cxx Help.cxx)

add_test(pointlocator_output_serial ./daxlocator_output_test_serial)

# daxlocate_2d_test
file(COPY ./daxlocate_2d_correct_output.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(daxlocate_2d_test_serial ./daxlocator_2d_test.cxx
               ../RandomPoints2D.cxx ../../Point2D.cxx ../../PointLocator.cxx ../../PointLocatorExec.cxx Help.cxx)

add_test(pointlocate_2d_test_serial command ./daxlocate_2d_test_serial)

# execlocate_output_test
file(COPY ./execlocate_correct_output.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(execlocate_output_test_serial ./execlocate_output_test.cxx
               ../RandomPoints3D.cxx ../../Point3D.cxx ../../Point2D.cxx ../../PointLocator.cxx ../../PointLocatorExec.cxx Help.cxx)

add_test(pointlocateexec_output_test_serial ./execlocate_output_test_serial)

# execlocate_2d_test
file(COPY ./execlocate_2d_correct_output.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(execlocate_2d_test_serial ./execlocate_2d_test.cxx
               ../RandomPoints2D.cxx ../../Point2D.cxx ../../PointLocator.cxx ../../PointLocatorExec.cxx Help.cxx)

add_test(pointlocateexec_2d_test_serial command ./execlocate_2d_test_serial)

# divisions test
add_executable(divisions_test ./divs_test.cxx ../../PointLocator.cxx ../../PointLocatorExec.cxx)
add_test(divisions_test command ./divisions_test)

# triangle mesh 2d test
file(COPY ./triangle_mesh_2d_correct_output.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(triangle_mesh_2d_test ./trianglemesh_2d_test.cxx ../TriangleMesh2DGenerator.cxx ../../Point2D.cxx Help.cxx ../../PointLocator.cxx ../../PointLocatorExec.cxx)
add_test(triangle_mesh_2d_test command ./triangle_mesh_2d_test)

# explicit to implicit indexing test
add_executable(explicit2implicitindex_test_serial ./explicit2implicitindex_test.cxx)
add_test(explicit2implicitindex_test_serial command ./explicit2implicitindex_test_serial)

##############################################################################
#
#
#
#
# If cuda is enabled
#
#
#
#
###############################################################################
if (BUILD_CUDA)
    configure_file(daxlocator_output_test.cxx ${CMAKE_CURRENT_BINARY_DIR}/daxlocator_output_test.cu COPYONLY)
    configure_file(daxlocator_2d_test.cxx ${CMAKE_CURRENT_BINARY_DIR}/daxlocator_2d_test.cu COPYONLY)
    configure_file(execlocate_output_test.cxx ${CMAKE_CURRENT_BINARY_DIR}/execlocate_output_test.cu COPYONLY)
    configure_file(execlocate_2d_test.cxx ${CMAKE_CURRENT_BINARY_DIR}/execlocate_2d_test.cu COPYONLY)
    configure_file(explicit2implicitindex_test.cxx ${CMAKE_CURRENT_BINARY_DIR}/explicit2implicitindex_test.cu COPYONLY)

    cuda_add_executable(daxlocator_output_test_cuda
                        ${CMAKE_CURRENT_BINARY_DIR}/daxlocator_output_test.cu
                        ../RandomPoints3D.cxx
                        ../../Point3D.cxx
                        ../../Point2D.cxx
                        ${CMAKE_CURRENT_BINARY_DIR}/../../PointLocator.cu
                        ${CMAKE_CURRENT_BINARY_DIR}/../../PointLocatorExec.cu
                        Help.cxx)
    cuda_add_executable(daxlocate_2d_test_cuda
                        ${CMAKE_CURRENT_BINARY_DIR}/daxlocator_2d_test.cu
                        ../RandomPoints2D.cxx
                        ../../Point2D.cxx
                        ${CMAKE_CURRENT_BINARY_DIR}/../../PointLocator.cu
                        ${CMAKE_CURRENT_BINARY_DIR}/../../PointLocatorExec.cu
                        Help.cxx)
    cuda_add_executable(execlocate_output_test_cuda
                        ${CMAKE_CURRENT_BINARY_DIR}/execlocate_output_test.cu
                        ../RandomPoints3D.cxx
                        ../../Point3D.cxx
                        ../../Point2D.cxx
                        ${CMAKE_CURRENT_BINARY_DIR}/../../PointLocator.cu
                        ${CMAKE_CURRENT_BINARY_DIR}/../../PointLocatorExec.cu
                        Help.cxx)
    cuda_add_executable(execlocate_2d_test_cuda ${CMAKE_CURRENT_BINARY_DIR}/execlocate_2d_test.cu
                        ../RandomPoints2D.cxx ../../Point2D.cxx
                        ${CMAKE_CURRENT_BINARY_DIR}/../../PointLocator.cu
                        ${CMAKE_CURRENT_BINARY_DIR}/../../PointLocatorExec.cu
                        Help.cxx)
    cuda_add_executable(explicit2implicitindex_test_cuda ${CMAKE_CURRENT_BINARY_DIR}/explicit2implicitindex_test.cu)

    add_test(pointlocator_output_cuda command ./daxlocator_output_test_cuda)
    add_test(pointlocate_2d_test_cuda command ./daxlocate_2d_test_cuda)
    add_test(pointlocateexec_output_test_cuda command ./execlocate_output_test_cuda)
    add_test(pointlocateexec_2d_test_cuda command ./execlocate_2d_test_cuda)
    add_test(explicit2implicitindex_test_cuda command ./explicit2implicitindex_test_cuda)
endif (BUILD_CUDA)
