#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>

#include "CellLocator.h"
#include "tests/TriangleMesh2DGenerator.h"
#include "CountOverlappingBuckets.h"
#include "Offset2CountFunctor.h"
#include "Coarse2ImplicitFunctor.h"

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
    CellLocator locator;
    locator.setExtent(0, 6, 0, 6, 0, 0);
    locator.setDivisions(2, 2, 1);
    locator.setPoints(points);
    locator.setConnections(connections);
    locator.build();

    // output and print for debugging
    // overlapping bucket counts
    std::vector<int> overlappingBucketCounts = locator.getOverlapBucketCounts();
    // print
    std::cout.precision(2);
    std::cout << std::fixed;
    std::cout << std::setw(10) << "No. Val: "
              << overlappingBucketCounts.size() << std::endl;
    std::cout << std::setw(10) << "Overlap: ";
    for (unsigned int i = 0; i < overlappingBucketCounts.size(); ++i)
        std::cout << std::setw(3) << overlappingBucketCounts[i] << ",";
    std::cout << std::endl;
    
    // cell ids and overlapping bucket ids
    std::vector<dax::Id> triangleIds = locator.getCellIds();
    std::vector<dax::Id> overlappingBucketIds = locator.getOverlapBucketIds();
    // print
    std::cout << std::setw(10) << "Triangle: ";
    for (int i = 0; i < triangleIds.size(); ++i)
        std::cout << std::setw(3) << triangleIds[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Buckets: ";
    for (int i = 0; i < overlappingBucketIds.size(); ++i)
        std::cout << std::setw(3) << overlappingBucketIds[i] << ",";
    std::cout << std::endl;

    // sort cells and sort buckets
    std::vector<dax::Id> sortTriangles = locator.getSortCellIds();
    std::vector<dax::Id> sortBuckets = locator.getSortBucketIds();
    // print
    std::cout << std::setw(10) << "Sort Bkt: ";
    for (int i = 0; i < sortBuckets.size(); ++i)
        std::cout << std::setw(3) << sortBuckets[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Sort Tri: ";
    for (int i = 0; i < sortTriangles.size(); ++i)
        std::cout << std::setw(3) << sortTriangles[i] << ",";
    std::cout << std::endl;

    // reduct buckets -- final format
    std::vector<int> uniqueBucketIds = locator.getUniqueBucketIds();
    std::vector<int> bucketTriangleCounts = locator.getBucketCellCounts();
    std::vector<dax::Id> triangleStarts = locator.getCellStarts();
    std::vector<int> triangleCounts = locator.getCellCounts();
    // print
    std::cout << std::setw(10) << "Unique: ";
    for (int i = 0; i < uniqueBucketIds.size(); ++i)
        std::cout << std::setw(3) << uniqueBucketIds[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Counts: ";
    for (int i = 0; i < bucketTriangleCounts.size(); ++i)
        std::cout << std::setw(3) << bucketTriangleCounts[i] << ",";
    std::cout << std::endl;
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
