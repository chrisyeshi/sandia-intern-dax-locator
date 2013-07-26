#include "TriangleMesh2DGenerator.h"

TriangleMesh2DGenerator::TriangleMesh2DGenerator()
{
    extent.resize(4);
    divisions.resize(2);
}

TriangleMesh2DGenerator::~TriangleMesh2DGenerator()
{
}

void TriangleMesh2DGenerator::setExtent(int xmin, int xmax, int ymin, int ymax)
{
    extent[0] = xmin;
    extent[1] = xmax;
    extent[2] = ymin;
    extent[3] = ymax;
}

void TriangleMesh2DGenerator::setDivisions(int x, int y)
{
    divisions[0] = x;
    divisions[1] = y;
}

void TriangleMesh2DGenerator::generate()
{
    // first generate the points
    points.resize(totalPointCount());
    for (int y = 0; y < pointCounts()[1]; ++y)
        for (int x = 0; x < pointCounts()[0]; ++x)
        {
            int ptIndex = x + y * pointCounts()[0];
            Point2D pt(spacings()[0] * float(x) + extent[0],
                       spacings()[1] * float(y) + extent[2]);
            points[ptIndex] = pt;
        }
    // then use the point ids to build the connectivities
    // for each quad, subdivide it into 2 triangles
    connections.resize(totalQuadCount() * trianglesPerQuad * verticesPerTriangle);
    for (int y = 0; y < divisions[1]; ++y)
        for (int x = 0; x < divisions[0]; ++x)
        {
            // per cell, or per lower left cornor point
            // point Ids
            int ptId00 = (x + 0) + (y + 0) * pointCounts()[0];
            int ptId10 = (x + 1) + (y + 0) * pointCounts()[0];
            int ptId01 = (x + 0) + (y + 1) * pointCounts()[0];
            int ptId11 = (x + 1) + (y + 1) * pointCounts()[0];
            // quad Id
            int quadId = x + y * quadCounts()[0];
            // starting position in the connections array
            int quadStart = quadId * trianglesPerQuad * verticesPerTriangle;
            // fill the connections array
            // first triangle
            connections[quadStart + 0] = ptId00;
            connections[quadStart + 1] = ptId10;
            connections[quadStart + 2] = ptId11;
            // second triangle
            connections[quadStart + 3] = ptId11;
            connections[quadStart + 4] = ptId01;
            connections[quadStart + 5] = ptId00;
        }
}

// number of points in each direction
std::vector<int> TriangleMesh2DGenerator::pointCounts() const
{
    std::vector<int> counts(3);
    counts[0] = divisions[0] + 1;
    counts[1] = divisions[1] + 1;
    return counts;
}

int TriangleMesh2DGenerator::totalPointCount() const
{
    std::vector<int> counts = pointCounts();
    return counts[0] * counts[1];
}

std::vector<float> TriangleMesh2DGenerator::ranges() const
{
    std::vector<float> rans(2);
    rans[0] = extent[1] - extent[0];
    rans[1] = extent[3] - extent[2];
    return rans;
}

std::vector<float> TriangleMesh2DGenerator::spacings() const
{
    std::vector<float> sps(2);
    sps[0] = ranges()[0] / divisions[0];
    sps[1] = ranges()[1] / divisions[1];
    return sps;
}

std::vector<int> TriangleMesh2DGenerator::quadCounts() const
{
    return divisions;
}

int TriangleMesh2DGenerator::totalQuadCount() const
{
    return quadCounts()[0] * quadCounts()[1];
}
