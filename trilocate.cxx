#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>

#include <dax/CellTag.h>

#include "CellLocatorExec.h"
#include "CellLocator.h"
#include "tests/TriangleMesh2DGenerator.h"
#include "Offset2CountFunctor.h"

using namespace dax::cont;

// main
int main(void)
{
    // first generate the triangle mesh
    TriangleMesh2DGenerator generator;
    generator.setExtent(0, 5.99, 0, 5.99);
    generator.setDivisions(3, 3);
    generator.generate();

    // points and connections
    std::vector<dax::Vector2> points2d = generator.getDaxPoints();
    std::vector<dax::Id> connections = generator.getDaxConnections();

    // translate to dax::Vector3
    std::vector<dax::Vector3> points(points2d.size());
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        dax::Vector2 point2d = points2d[i];
        dax::Vector3 point = dax::make_Vector3(point2d[0], point2d[1], 0.f);
        points[i] = point;
    }

    // using CellLocator for constructing the search structure
    CellLocator<dax::CellTagTriangle> locator;
    locator.setAutomatic(false);
    locator.setDimensions(2, 2, 1);
    locator.setBounds(6, 6, 0);
    locator.setPoints(points);
    locator.setConnections(connections);
    locator.build();

    // output and print for debugging
    // print
    std::cout.precision(2);
    std::cout << std::fixed;
    
    // sort cells and sort buckets
    std::vector<dax::Id> sortTriangles = locator.getSortCellIds();
    // print
    std::cout << std::setw(10) << "Sort Tri: ";
    for (int i = 0; i < sortTriangles.size(); ++i)
        std::cout << std::setw(3) << sortTriangles[i] << ",";
    std::cout << std::endl;

    // reduct buckets -- final format
    std::vector<dax::Id> triangleStarts = locator.getCellStarts();
    std::vector<int> triangleCounts = locator.getCellCounts();
    // print
    std::cout << std::setw(10) << "Starts: ";
    for (int i = 0; i < triangleStarts.size(); ++i)
        std::cout << std::setw(3) << triangleStarts[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Counts: ";
    for (int i = 0; i < triangleCounts.size(); ++i)
        std::cout << std::setw(3) << triangleCounts[i] << ",";
    std::cout << std::endl;

    return 0;
}
