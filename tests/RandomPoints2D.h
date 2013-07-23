#ifndef __RANDOMPOINTS2D_H__
#define __RANDOMPOINTS2D_H__

#include <vector>

#include "../Point2D.h"

class RandomPoints2D
{
public:
    RandomPoints2D();
    virtual ~RandomPoints2D();

    void setPointCount(int count);
    void setExtent(int xmin, int xmax, int ymin, int ymax);
    void generate();
    void write(const char *path) const;
    void write(const std::string& path) const;
    bool read(const char *path);
    bool read(const std::string& path);
    const std::vector<Point2D>& getPoints() const { return points; }

protected:
    std::vector<int> extent;
    std::vector<Point2D> points;

private:

};

#endif // __RANDOMPOINTS2D_H__
