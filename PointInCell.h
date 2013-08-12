#ifndef __POINTINCELL_H__
#define __POINTINCELL_H__

#include <dax/CellTag.h>
#include <dax/Types.h>
#include <dax/exec/ParametricCoordinates.h>
#include <dax/math/VectorAnalysis.h>

using namespace dax::exec;

template <class CellTag>
DAX_EXEC_EXPORT
bool PointInCell(const dax::Vector3& point,
                 CellField<dax::Vector3, CellTag> vertices)
{
    return false;
}

template <>
DAX_EXEC_EXPORT
bool PointInCell(const dax::Vector3& wCoord,
                 CellField<dax::Vector3, dax::CellTagHexahedron> vertices)
{
    // well, it's hexahedron, so parametric coordinate rules!!!
    dax::Vector3 pCoord = WorldCoordinatesToParametricCoordinates(vertices, wCoord,
            dax::CellTagHexahedron());
    return pCoord[0] >= 0.0 && pCoord[0] <= 1.0 &&
           pCoord[1] >= 0.0 && pCoord[1] <= 1.0 &&
           pCoord[2] >= 0.0 && pCoord[2] <= 1.0;
}

template <>
DAX_EXEC_EXPORT
bool PointInCell(const dax::Vector3& wCoord,
                 CellField<dax::Vector3, dax::CellTagLine> vertices)
{
    // distance from point to line
    // x0: the point, x1 and x2 are points define the line
    // equation: d^2 = |(x2 - x1) X (x1 - x0)|^2 / |x2 - x1|^2
    dax::Vector3 x0, x1, x2;
    x0 = wCoord;
    x1 = vertices[0];
    x2 = vertices[1];
    dax::Scalar distance2
      = dax::math::MagnitudeSquared(dax::math::Cross(x2 - x1, x1 - x0))
      / dax::math::MagnitudeSquared(x2 - x1);
    // the point has to be reasonably close to the line,
    if (distance2 > 0.0001)
        return false;
    // and within the two bounding points of the line segment.
    dax::Vector3 pCoord = WorldCoordinatesToParametricCoordinates(vertices, wCoord,
            dax::CellTagLine());
    return pCoord[0] >= 0.0 && pCoord[0] <= 1.0;
}

// seems like this is exactly the same as triangle, for now I copied it over
template <>
DAX_EXEC_EXPORT
bool PointInCell(const dax::Vector3& wCoord,
                 CellField<dax::Vector3, dax::CellTagQuadrilateral> vertices)
{
    // distance from point to plane
    // derive plane equation
    dax::Vector3 normal = dax::math::TriangleNormal(vertices[0],
                                                    vertices[1],
                                                    vertices[2]);
    // plane equation ax + by + cz + d = 0
    dax::Scalar a = normal[0];
    dax::Scalar b = normal[1];
    dax::Scalar c = normal[2];
    dax::Scalar d = -a * vertices[0][0] - b * vertices[0][1] - c * vertices[0][2];
    // point to plane distance
    dax::Scalar distance
      = (a * wCoord[0] + b * wCoord[1] + c * wCoord[2] + d)
      / dax::math::Sqrt(a * a + b * b + c * c);
    // the point has to be reasonably close to the plane
    if (distance > 0.0001)
        return false;
    // then use parametric coordinates to see if the point is within the bounds
    dax::Vector3 pCoord
      = WorldCoordinatesToParametricCoordinates(vertices, wCoord,
                                                dax::CellTagQuadrilateral());
    return pCoord[0] >= 0.0 && pCoord[0] <= 1.0 &&
           pCoord[1] >= 0.0 && pCoord[1] <= 1.0;
}

template <>
DAX_EXEC_EXPORT
bool PointInCell(const dax::Vector3& wCoord,
                 CellField<dax::Vector3, dax::CellTagTetrahedron> vertices)
{
    dax::Vector3 pCoord
      = WorldCoordinatesToParametricCoordinates(vertices, wCoord,
                                                dax::CellTagTetrahedron());
    // we cannot just use the three parametric coordinates to verfiy
    // because tetrahedron is not a cube type shape.
    // There is a plane connected by (1,0,0), (0,1,0), (0,0,1),
    // and when a point is closer to origin than this plane,
    // then the point is in the tetrahedron.
    dax::Scalar p4 = pCoord[0] + pCoord[1] + pCoord[2];
    return pCoord[0] >= 0.0 && pCoord[0] <= 1.0 &&
           pCoord[1] >= 0.0 && pCoord[1] <= 1.0 &&
           pCoord[2] >= 0.0 && pCoord[2] <= 1.0 &&
           p4 >= 0.0 && p4 <= 1.0;
}

// seems like this is exactly the same as quadrilateral, for now I copied it over
template <>
DAX_EXEC_EXPORT
bool PointInCell(const dax::Vector3& wCoord,
                 CellField<dax::Vector3, dax::CellTagTriangle> vertices)
{
    // distance from point to plane
    // derive plane equation
    dax::Vector3 normal = dax::math::TriangleNormal(vertices[0],
                                                    vertices[1],
                                                    vertices[2]);
    // plane equation ax + by + cz + d = 0
    dax::Scalar a = normal[0];
    dax::Scalar b = normal[1];
    dax::Scalar c = normal[2];
    dax::Scalar d = -a * vertices[0][0] - b * vertices[0][1] - c * vertices[0][2];
    // point to plane distance
    dax::Scalar distance
      = (a * wCoord[0] + b * wCoord[1] + c * wCoord[2] + d)
      / dax::math::Sqrt(a * a + b * b + c * c);
    // the point has to be reasonably close to the plane
    if (distance > 0.0001)
        return false;
    // then use parametric coordinates to see if the point is within the bounds
    dax::Vector3 pCoord
      = WorldCoordinatesToParametricCoordinates(vertices, wCoord,
                                                dax::CellTagTriangle());
    return pCoord[0] >= 0.0 && pCoord[0] <= 1.0 &&
           pCoord[1] >= 0.0 && pCoord[1] <= 1.0;
}

template <>
DAX_EXEC_EXPORT
bool PointInCell(const dax::Vector3& wCoord,
                 CellField<dax::Vector3, dax::CellTagVoxel> vertices)
{
    // Voxel... do we need to comment on this one?
    dax::Vector3 pCoord
      = WorldCoordinatesToParametricCoordinates(vertices, wCoord,
                                                dax::CellTagVoxel());
    return pCoord[0] >= 0.0 && pCoord[0] <= 1.0 &&
           pCoord[1] >= 0.0 && pCoord[1] <= 1.0 &&
           pCoord[2] >= 0.0 && pCoord[2] <= 1.0;
}

template <>
DAX_EXEC_EXPORT
bool PointInCell(const dax::Vector3& wCoord,
                 CellField<dax::Vector3, dax::CellTagWedge> vertices)
{
    dax::Vector3 pCoord
      = WorldCoordinatesToParametricCoordinates(vertices, wCoord,
                                                dax::CellTagWedge());
    dax::Scalar p4 = pCoord[0] + pCoord[1];
    return pCoord[0] >= 0.0 && pCoord[0] <= 1.0 &&
           pCoord[1] >= 0.0 && pCoord[1] <= 1.0 &&
           pCoord[2] >= 0.0 && pCoord[2] <= 1.0 &&
           p4 >= 0.0 && p4 <= 1.0;
}

#endif //__POINTINCELL_H__
