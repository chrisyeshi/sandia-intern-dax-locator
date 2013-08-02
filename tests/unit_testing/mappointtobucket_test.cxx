#include "MapPointToBucket.h"

#include <dax/Types.h>
#include <dax/cont/UniformGrid.h>

int main(void)
{
    UniformGrid<> grid;
    grid.SetExtent(dax::make_Id3(0, 0, 0), dax::make_Id3(100, 100, 100));

    dax::Id id = MapPointToBucket(grid).MapToFlatIndex(
            dax::make_Vector3(50.0, 50.0, 50.0));

    assert(id == 50 + 50 * 100 + 50 * 100 * 100);

    return 0;
}
