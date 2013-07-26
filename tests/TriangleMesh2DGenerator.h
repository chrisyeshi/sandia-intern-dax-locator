#include <iostream>

#include "Point2D.h"

class TriangleMesh2DGenerator
{
public:
    TriangleMesh2DGenerator();
    virtual ~TriangleMesh2DGenerator();

    void setExtent(int xmin, int xmax, int ymin, int ymax);
    void setDivisions(int x, int y);

    void generate();

    std::vector<Point2D> getPoints() const { return points; }
    std::vector<int> getConnections() const { return connections; }

protected:
    // points are the vertices of the mesh
    // connections is the ids of the points,
    // which defines the connectivities
    //
    // It's basically subdividing each cell in uniform grid in this way:
    // +---------------+
    // |             / |
    // |           /   |
    // |         /     |
    // |       /       |
    // |     /         |
    // |   /           |
    // | /             |
    // +---------------+
    std::vector<Point2D> points;
    std::vector<int> connections;

    std::vector<int> extent;
    std::vector<int> divisions;

    // number of points in each direction
    std::vector<int> pointCounts() const;
    int totalPointCount() const;
    std::vector<float> ranges() const;
    std::vector<float> spacings() const;
    std::vector<int> quadCounts() const;
    int totalQuadCount() const;

    const static int trianglesPerQuad = 2;
    const static int verticesPerTriangle = 3;

private:
};
