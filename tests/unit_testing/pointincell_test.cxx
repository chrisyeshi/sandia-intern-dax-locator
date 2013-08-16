#define BOOST_SP_DISABLE_THREADS

#include <vector>

#include <dax/Types.h>
#include <dax/exec/CellField.h>
#include <dax/CellTag.h>
#include <dax/cont/ArrayHandle.h>
#include <dax/cont/Scheduler.h>
#include <dax/exec/ParametricCoordinates.h>
#include <dax/exec/internal/WorkletBase.h>
#include <dax/exec/Assert.h>
#include <dax/exec/internal/ErrorMessageBuffer.h>
#include <dax/cont/ErrorExecution.h>

#include "PointInCell.h"

using namespace dax::cont;
using namespace dax::exec;

typedef dax::cont::internal::DeviceAdapterAlgorithm<DAX_DEFAULT_DEVICE_ADAPTER_TAG> Algorithm;

struct TestWorklet : dax::exec::internal::WorkletBase
{
    // overload operator()
    DAX_EXEC_EXPORT
    void operator()(dax::Id index) const
    {
        testHexahedron();
        testLine();
        testQuadrilateral();
        testTetrahedron();
        testTriangle();
        testVoxel();
        testWedge();
    }

    DAX_EXEC_EXPORT
    void testHexahedron() const
    {
        CellField<dax::Vector3, dax::CellTagHexahedron> vertices;
        vertices[0] = dax::make_Vector3(0.0, 0.0, 0.0);
        vertices[1] = dax::make_Vector3(1.0, 0.0, 0.0);
        vertices[2] = dax::make_Vector3(1.0, 1.0, 0.0);
        vertices[3] = dax::make_Vector3(0.0, 1.0, 0.0);
        vertices[4] = dax::make_Vector3(0.0, 0.0, 1.0);
        vertices[5] = dax::make_Vector3(1.0, 0.0, 1.0);
        vertices[6] = dax::make_Vector3(1.0, 1.0, 1.0);
        vertices[7] = dax::make_Vector3(0.0, 1.0, 1.0);
        Assert(PointInCell(dax::make_Vector3(0.9, 0.2, 0.3), vertices),
               "Hexahedron inside test failed...", *this);
        Assert(!PointInCell(dax::make_Vector3(1.1, 0.3, 0.4), vertices),
               "Hexahedron outside test failed...", *this);
    }

    DAX_EXEC_EXPORT
    void testLine() const
    {
        CellField<dax::Vector3, dax::CellTagLine> vertices;
        vertices[0] = dax::make_Vector3(0.0, 0.0, 0.0);
        vertices[1] = dax::make_Vector3(1.0, 0.0, 0.0);
        Assert(PointInCell(dax::make_Vector3(0.3, 0.0, 0.000001), vertices),
               "Line inside test failed...", *this);
        Assert(!PointInCell(dax::make_Vector3(1.1, 0.3, 0.4), vertices),
               "Line outside test failed...", *this);
    }

    DAX_EXEC_EXPORT
    void testQuadrilateral() const
    {
        CellField<dax::Vector3, dax::CellTagQuadrilateral> vertices;
        vertices[0] = dax::make_Vector3(0.0, 0.0, 0.0);
        vertices[1] = dax::make_Vector3(1.0, 0.0, 0.0);
        vertices[2] = dax::make_Vector3(1.0, 1.0, 0.0);
        vertices[3] = dax::make_Vector3(0.0, 1.0, 0.0);
        Assert(PointInCell(dax::make_Vector3(0.3, 0.9, 0.000001), vertices),
               "Quadrilateral inside test failed...", *this);
        Assert(!PointInCell(dax::make_Vector3(1.1, 0.3, 0.0), vertices),
               "Quadrilateral outside test 1 failed...", *this);
        Assert(!PointInCell(dax::make_Vector3(0.3, 0.8, 0.4), vertices),
               "Quadrilateral outside test 2 failed...", *this);
        Assert(!PointInCell(dax::make_Vector3(0.2, 0.4, -1.0), vertices),
               "Quadrilateral outside test 3 failed...", *this);
    }

    DAX_EXEC_EXPORT
    void testTetrahedron() const
    {
        CellField<dax::Vector3, dax::CellTagTetrahedron> vertices;
        vertices[0] = dax::make_Vector3(0.0, 0.0, 0.0);
        vertices[1] = dax::make_Vector3(1.0, 0.0, 0.0);
        vertices[2] = dax::make_Vector3(0.0, 1.0, 0.0);
        vertices[3] = dax::make_Vector3(0.0, 0.0, 1.0);
        Assert(PointInCell(dax::make_Vector3(0.3, 0.2, 0.1), vertices),
               "Tetrahedron inside test failed...", *this);
        Assert(!PointInCell(dax::make_Vector3(0.5, 0.5, 0.5), vertices),
               "Tetrahedron outside test failed...", *this);
    }

    DAX_EXEC_EXPORT
    void testTriangle() const
    {
        CellField<dax::Vector3, dax::CellTagTriangle> vertices;
        vertices[0] = dax::make_Vector3(0.0, 0.0, 0.0);
        vertices[1] = dax::make_Vector3(1.0, 0.0, 0.0);
        vertices[2] = dax::make_Vector3(0.0, 1.0, 0.0);
        Assert(PointInCell(dax::make_Vector3(0.3, 0.2, 0.000001), vertices),
               "Triangle inside test failed...", *this);
        Assert(!PointInCell(dax::make_Vector3(0.51, 0.51, 0.0), vertices),
               "Triangle outside test 1 failed...", *this);
        Assert(!PointInCell(dax::make_Vector3(0.3, 0.2, 0.5), vertices),
               "Triangle outside test 2 failed...", *this);
        Assert(!PointInCell(dax::make_Vector3(0.3, 0.2, -0.5), vertices),
               "Triangle outside test 3 failed...", *this);
    }

    DAX_EXEC_EXPORT
    void testVoxel() const
    {
        CellField<dax::Vector3, dax::CellTagVoxel> vertices;
        vertices[0] = dax::make_Vector3(0.0, 0.0, 0.0);
        vertices[1] = dax::make_Vector3(1.0, 0.0, 0.0);
        vertices[2] = dax::make_Vector3(1.0, 1.0, 0.0);
        vertices[3] = dax::make_Vector3(0.0, 1.0, 0.0);
        vertices[4] = dax::make_Vector3(0.0, 0.0, 1.0);
        vertices[5] = dax::make_Vector3(1.0, 0.0, 1.0);
        vertices[6] = dax::make_Vector3(1.0, 1.0, 1.0);
        vertices[7] = dax::make_Vector3(0.0, 1.0, 1.0);
        Assert(PointInCell(dax::make_Vector3(0.9, 0.2, 0.3), vertices),
               "Voxel inside test failed...", *this);
        Assert(!PointInCell(dax::make_Vector3(1.1, 0.3, 0.4), vertices),
               "Voxel outside test failed...", *this);
    }

    DAX_EXEC_EXPORT
    void testWedge() const
    {
        CellField<dax::Vector3, dax::CellTagWedge> wedge;
        wedge[0] = dax::make_Vector3(0.0, 0.0, 0.0);
        wedge[1] = dax::make_Vector3(0.0, 1.0, 0.0);
        wedge[2] = dax::make_Vector3(1.0, 0.0, 0.0);
        wedge[3] = dax::make_Vector3(0.0, 0.0, 1.0);
        wedge[4] = dax::make_Vector3(0.0, 1.0, 1.0);
        wedge[5] = dax::make_Vector3(1.0, 0.0, 1.0);
        Assert(PointInCell(dax::make_Vector3(0.4, 0.4, 0.5), wedge),
               "Wedge inside test failed...", *this);
        Assert(!PointInCell(dax::make_Vector3(0.51, 0.51, 0.5), wedge),
               "Wedge outside test failed...", *this);
    }
};

// main
int main(void)
{
    // use a worklet because it PointInCell only works in device
    TestWorklet testWorklet;
    try
    {
        Algorithm::Schedule(testWorklet, 1);
    } catch (ErrorExecution e)
    {
        std::cout << "Error: " << e.GetMessage() << std::endl;
        return 1;
    }

    return 0;
}
