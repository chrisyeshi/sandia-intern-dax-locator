add_executable(point2d_test_serial point2d_test.cxx ../../Point2D.cxx)
add_test(point2d_test_serial COMMAND ./point2d_test_serial)

add_executable(randompoints_serial ./randompoints_test.cxx ../RandomPoints2D.cxx ../RandomPoints2D.h ../../Point2D.cxx ./stdnormal_test.cxx)
add_test(randompoints_serial COMMAND ./randompoints_serial)

add_executable(randompoints_default_serial ./randompoints_default_test.cxx ../RandomPoints2D.cxx ../../Point2D.cxx ./stdnormal_test.cxx)
add_test(randompoints_default_serial command ./randompoints_default_serial)

add_executable(gridspace_serial ./gridspace_test.cxx)
add_test(gridspace_serial COMMAND ./gridspace_serial)
