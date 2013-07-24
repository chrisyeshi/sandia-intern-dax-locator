#include "Help.h"

#include <fstream>

#include "DaxLocator.h"

namespace help
{

void printStartCount(const std::vector<dax::Id>& pointStarts,
                     const std::vector<int>& pointCounts,
                     std::iostream& stream)
{
    stream << std::setw(10) << "Pt Start: ";
    for (unsigned int i = 0; i < pointStarts.size(); ++i)
        stream << std::setw(3) << pointStarts[i] << ", ";
    stream << std::endl;
    stream << std::setw(10) << "Pt Count: ";
    for (unsigned int i = 0; i < pointCounts.size(); ++i)
        stream << std::setw(3) << pointCounts[i] << ", ";
    stream << std::endl;
}

void printBinPoint(float x, float y, float z,
                   const DaxLocator& locator,
                   std::iostream& stream)
{
    stream << std::setw(10) << "X: " << x << std::endl;
    stream << std::setw(10) << "Y: " << y << std::endl;
    stream << std::setw(10) << "Z: " << z << std::endl;
    dax::Vector3 point(x, y, z);
    // find the bucket id the point belongs to
    dax::Id id = locator.locatePoint(point);
    // find the points in the same bucket
    std::vector<dax::Vector3> points = locator.getBucketPoints(id);
    
    // print 
    stream << std::setw(10) << "Bucket Id: " << id << std::endl;
    stream << std::setw(10) << "Bin Count: " << points.size() << std::endl;
}

void printCompare(const std::string& output, const std::string& filename)
{
    // output string that needs to be verified
    std::cout << "Output: " << std::endl << output << std::endl << std::endl;

    // read in the correct output
    std::ifstream fin(filename.c_str(), std::ios::binary);
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
}

void printCoinPoints(const std::vector<dax::Vector3>& testPoints,
                     const std::vector<dax::Id>& testBucketIds,
                     const std::vector<int>& testCounts,
                     const std::vector<dax::Vector3>& testCoinPoints,
                     std::iostream& stream)
{
    // print
    stream << std::setw(10) << "Test X: ";
    for (unsigned int i = 0; i < testPoints.size(); ++i)
        stream << std::setw(6) << testPoints[i][0] << ", ";
    stream << std::endl;
    stream << std::setw(10) << "Test Y: ";
    for (unsigned int i = 0; i < testPoints.size(); ++i)
        stream << std::setw(6) << testPoints[i][1] << ", ";
    stream << std::endl;
    stream << std::setw(10) << "Test Z: ";
    for (unsigned int i = 0; i < testPoints.size(); ++i)
        stream << std::setw(6) << testPoints[i][2] << ", ";
    stream << std::endl;
    stream << std::setw(10) << "Bucket: ";
    for (unsigned int i = 0; i < testBucketIds.size(); ++i)
        stream << std::setw(6) << testBucketIds[i] << ", ";
    stream << std::endl;
    stream << std::setw(10) << "Count: ";
    for (unsigned int i = 0; i < testCounts.size(); ++i)
        stream << std::setw(6) << testCounts[i] << ", ";
    stream << std::endl;
    stream << std::setw(10) << "Coin X: ";
    for (unsigned int i = 0; i < testCoinPoints.size(); ++i)
        stream << std::setw(6) << testCoinPoints[i][0] << ", ";
    stream << std::endl;
    stream << std::setw(10) << "Coin Y: ";
    for (unsigned int i = 0; i < testCoinPoints.size(); ++i)
        stream << std::setw(6) << testCoinPoints[i][1] << ", ";
    stream << std::endl;
    stream << std::setw(10) << "Coin Z: ";
    for (unsigned int i = 0; i < testCoinPoints.size(); ++i)
        stream << std::setw(6) << testCoinPoints[i][2] << ", ";
    stream << std::endl;
}

}
