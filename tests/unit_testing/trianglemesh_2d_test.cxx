#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

#include "tests/TriangleMesh2DGenerator.h"
#include "tests/unit_testing/Help.h"

int main(void)
{
    // generate a grid like triangle mesh with 3x4
    TriangleMesh2DGenerator generator;
    generator.setExtent(0, 3, 0, 4);
    generator.setDivisions(3, 4);
    generator.generate();

    // outputs
    std::vector<Point2D> points = generator.getPoints();
    std::vector<int> connections = generator.getConnections();

    // print
    std::stringstream ss;
    ss.precision(4);
    ss << std::fixed;
    // points x coordinate
    ss << std::setw(10) << "Count: " << points.size() << std::endl;
    ss << std::setw(10) << "Index: ";
    for (int i = 0; i < points.size(); ++i)
        ss << std::setw(6) << i << ", ";
    ss << std::endl;
    ss << std::setw(10) << "Pts X: ";
    for (unsigned int i = 0; i < points.size(); ++i)
        ss << std::setw(6) << points[i].x() << ", ";
    ss << std::endl;
    // points y coordinate
    ss << std::setw(10) << "Pts Y: ";
    for (unsigned int i = 0; i < points.size(); ++i)
        ss << std::setw(6) << points[i].y() << ", ";
    ss << std::endl;
    // connections
    ss << std::setw(10) << "Connect:" << std::endl;
    for (unsigned int i = 0; i < connections.size(); ++i)
    {
        ss << std::setw(3) << connections[i];
        if ((i + 1) % 6 == 0)
            ss << std::endl;
        else
            ss << ",";
    }

    // compare output to the correct output file
    help::printCompare(ss.str(), "triangle_mesh_2d_correct_output.txt");

    return 0;
}
