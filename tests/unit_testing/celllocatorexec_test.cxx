#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>

#include <dax/CellTag.h>
#include <dax/cont/testing/TestingGridGenerator.h>
#include <dax/cont/Scheduler.h>
#include <dax/exec/WorkletMapField.h>

#include "CellLocator.h"

using namespace dax::cont;

// worklet for querying points
struct TestWorklet : dax::exec::WorkletMapField
{
    // signatures
    typedef void ControlSignature(ExecObject(), Field(In),
                                  Field(Out), Field(Out), Field(Out));
    typedef void ExecutionSignature(_1, _2, _3, _4, _5);

    // overload operator()
    template <typename ExecType>
    DAX_EXEC_EXPORT
    void operator()(const ExecType& locator,
                    const dax::Vector3& point,
                    dax::Id& bucketId,
                    int& bucketCellCount,
                    dax::Id& cellId) const
    {
        bucketId = locator.getBucketId(point);
        bucketCellCount = locator.getBucketCellCount(bucketId);
        cellId = locator.findCell(point);
    }
};

template <class CellType>
void testCellType(dax::Id correctBucketIds[],
                  int correctCellCounts[],
                  dax::Id correctCellIds[])
{
    std::cout.precision(2);
    std::cout << std::fixed;

    // First, generate some test data
    const dax::Id DIM = 4;
    typedef dax::cont::UnstructuredGrid<CellType> GridType;
    dax::cont::testing::TestGrid<GridType> gridGen(DIM);
    GridType grid = gridGen.GetRealGrid();

    // CellLocator
    CellLocator<CellType> locator;
    locator.setAutomatic(false);
    locator.setDimensions(2, 2, 2);
    locator.setBounds(3.01, 3.01, 3.01);
    locator.setPoints(grid.GetPointCoordinates());
    locator.setConnections(grid.GetCellConnections());
    locator.build();
    
    // test points for querying
    std::vector<dax::Vector3> testPoints;
    testPoints.push_back(dax::make_Vector3(0.2, 0.0, 0.0));
    testPoints.push_back(dax::make_Vector3(1.4, 1.5, 1.5));
    testPoints.push_back(dax::make_Vector3(1.9, 1.9, 1.9));
    testPoints.push_back(dax::make_Vector3(1.9, 2.9, 0.5));
    ArrayHandle<dax::Vector3> hTestPoints = make_ArrayHandle(testPoints);

    // output array handles
    ArrayHandle<dax::Id> hBucketIds;
    ArrayHandle<int> hBucketCellCounts;
    ArrayHandle<dax::Id> hCellIds;

    // use a worklet to test the execution object
    Scheduler<> scheduler;
    scheduler.Invoke(TestWorklet(), locator.PrepareForInput(),
                     hTestPoints, hBucketIds, hBucketCellCounts, hCellIds);

    // output
    std::vector<dax::Id> bucketIds(hBucketIds.GetNumberOfValues());
    hBucketIds.CopyInto(bucketIds.begin());
    std::vector<int> bucketCellCounts(hBucketCellCounts.GetNumberOfValues());
    hBucketCellCounts.CopyInto(bucketCellCounts.begin());
    std::vector<dax::Id> cellIds(hCellIds.GetNumberOfValues());
    hCellIds.CopyInto(cellIds.begin());

    // print
    std::cout << std::setw(20) << "Bucket Ids:";
    for (unsigned int i = 0; i < bucketIds.size(); ++i)
        std::cout << std::setw(3) << bucketIds[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(20) << "Bucket Cell Counts:";
    for (unsigned int i = 0; i < bucketCellCounts.size(); ++i)
        std::cout << std::setw(3) << bucketCellCounts[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(20) << "Cell Ids:";
    for (unsigned int i = 0; i < cellIds.size(); ++i)
        std::cout << std::setw(3) << cellIds[i] << ",";
    std::cout << std::endl;

    // asserts
    for (unsigned int i = 0; i < bucketIds.size(); ++i)
        assert(bucketIds[i] == correctBucketIds[i]);
    for (unsigned int i = 0; i < bucketCellCounts.size(); ++i)
        assert(bucketCellCounts[i] == correctCellCounts[i]);
    for (unsigned int i = 0; i < cellIds.size(); ++i)
        assert(cellIds[i] == correctCellIds[i]);
}

void testHexahedron()
{
    std::cout << "Testing Hexahedron..." << std::endl;
    dax::Id bucketIds[] = {0, 0, 7, 3};
    int cellCounts[] = {8, 8, 8, 8};
    dax::Id cellIds[] = {0, 13, 13, 7};
    testCellType<dax::CellTagHexahedron>(bucketIds, cellCounts, cellIds);
    std::cout << std::endl;
}

void testLine()
{
    std::cout << "Testing Line..." << std::endl;
    dax::Id bucketIds[] = {0, 0, 7, 3};
    int cellCounts[] = {12, 12, 12, 12};
    dax::Id cellIds[] = {0, -1, -1, -1};
    testCellType<dax::CellTagLine>(bucketIds, cellCounts, cellIds);
    std::cout << std::endl;
}

void testQuadrilateral()
{
    std::cout << "Testing Quadrilateral..." << std::endl;
    dax::Id bucketIds[] = {0, 0, 7, 3};
    int cellCounts[] = {18, 18, 18, 18};
    dax::Id cellIds[] = {0, -1, -1, -1};
    testCellType<dax::CellTagQuadrilateral>(bucketIds, cellCounts, cellIds);
    std::cout << std::endl;
}

void testTetrahedron()
{
    std::cout << "Testing Tetrahedron..." << std::endl;
    dax::Id bucketIds[] = {0, 0, 7, 3};
    int cellCounts[] = {16, 16, 16, 16};
    dax::Id cellIds[] = {0, -1, 27, 15};
    testCellType<dax::CellTagTetrahedron>(bucketIds, cellCounts, cellIds);
    std::cout << std::endl;
}

void testTriangle()
{
    std::cout << "Testing Triangle..." << std::endl;
    dax::Id bucketIds[] = {0, 0, 7, 3};
    int cellCounts[] = {36, 36, 36, 36};
    dax::Id cellIds[] = {0, -1, -1, -1};
    testCellType<dax::CellTagTriangle>(bucketIds, cellCounts, cellIds);
    std::cout << std::endl;
}

void testVertex()
{
    std::cout << "Testing Vertex..." << std::endl;
    dax::Id bucketIds[] = {0, 0, 7, 3};
    int cellCounts[] = {27, 27, 27, 27};
    dax::Id cellIds[] = {-1, -1, -1, -1};
    testCellType<dax::CellTagVertex>(bucketIds, cellCounts, cellIds);
    std::cout << std::endl;
}

void testWedge()
{
    std::cout << "Testing Wedge..." << std::endl;
    dax::Id bucketIds[] = {0, 0, 7, 3};
    int cellCounts[] = {16, 16, 16, 16};
    dax::Id cellIds[] = {0, 27, 26, 15};
    testCellType<dax::CellTagWedge>(bucketIds, cellCounts, cellIds);
    std::cout << std::endl;
}

// main
int main(void)
{
    testHexahedron();
//    testLine();
//    testQuadrilateral();
    testTetrahedron();
//    testTriangle();
    testVertex();
    testWedge();

    return 0;
}
