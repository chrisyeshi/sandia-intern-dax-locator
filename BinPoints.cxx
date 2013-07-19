#include "BinPoints.h"

dax::Id binPoint(const dax::Vector2& point,
                 const dax::Vector2& origin,
                 const dax::Vector2& spacing,
                 const dax::Extent3& extent)
{
    int resolution[2] = {extent.Max[0] - extent.Min[0],
                         extent.Max[1] - extent.Min[0]};
    // compute the point coordinate within the grid
    dax::Vector2 coord(point[0] - origin[0], point[1] - origin[1]);
    // which cell the point belongs
    dax::Id xid, yid;
    xid = dax::math::Floor(coord[0] / spacing[0]);
    yid = dax::math::Floor(coord[1] / spacing[1]);
    return xid + yid * resolution[0];
}
