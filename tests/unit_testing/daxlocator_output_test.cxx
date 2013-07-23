#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include <dax/Types.h>

#include "Point2D.h"
#include "tests/RandomPoints2D.h"
#include "DaxLocator.h"

// main
int main(void)
{
    // first generate a bunch of random points
    RandomPoints2D random;
    random.setExtent(0, 4, 0, 4);
    random.setPointCount(20);
    random.generate();
    std::vector<Point2D> points = random.getPoints();
    points.push_back(Point2D(0.99, 0.99));

    // translate Point2D to dax::vector2
    std::vector<dax::Vector2> daxPoints(points.size());
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        Point2D point = points[i];
        dax::Vector2 daxvec(point.x(), point.y());
        daxPoints[i] = daxvec;
    }

    // use DaxLocator class
    DaxLocator locator;
    locator.setSpacing(1.0, 1.0);
    locator.setExtent(0, 4, 0, 4);
    locator.setPoints(daxPoints);
    locator.build();

    // outputs
    std::vector<dax::Id> pointStarts = locator.getPointStarts();
    std::vector<int> pointCounts = locator.getPointCounts();

    // print
    std::stringstream ss;
    ss.precision(4);
    ss << std::setw(10) << "Pt Start: ";
    for (unsigned int i = 0; i < pointStarts.size(); ++i)
        ss << std::setw(3) << pointStarts[i] << ", ";
    ss << std::endl;
    ss << std::setw(10) << "Pt Count: ";
    for (unsigned int i = 0; i < pointCounts.size(); ++i)
        ss << std::setw(3) << pointCounts[i] << ", ";
    ss << std::endl;

    // binning a point 
    {
        // inputs for binPoint
        float x, y;
        x = y = 1.f;
        ss << std::setw(10) << "X: " << x << std::endl;
        ss << std::setw(10) << "Y: " << y << std::endl;
        dax::Vector2 point(x, y);
        // find the bucket id the point belongs to
        dax::Id id = locator.locatePoint(point);
        // find the points in the same bucket
        std::vector<dax::Vector2> points = locator.getBucketPoints(id);
        
        // print 
        ss << std::setw(10) << "Bucket Id: " << id << std::endl;
        ss << std::setw(10) << "Bin Count: " << points.size() << std::endl;
    }

    // output string that needs to be verified
    std::string output = ss.str();
    std::cout << "Output: " << std::endl << output << std::endl << std::endl;

    // read in the correct output
    std::ifstream fin("daxlocator_correct_output.txt", std::ios::binary);
    assert(fin.good());
    std::string correct_output;
    fin.seekg(0, std::ios::end);
    correct_output.resize(fin.tellg());
    fin.seekg(0, std::ios::beg);
    fin.read(&correct_output[0], correct_output.size());
    fin.close();
    std::cout << "Correct Output: " << std::endl << correct_output << std::endl;
    
    // compare the output and the correct output 
    assert(output == correct_output);

    return 0;
}
