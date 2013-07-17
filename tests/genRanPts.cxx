#include "./RandomPoints2D.h"

int main(void)
{
    RandomPoints2D random;
    random.setExtent(0, 100, 0, 100);
    random.setPointCount(10);
    random.generate();
    random.write("randompoints.dat");

    return 0;
}
