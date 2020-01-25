/* 
 * Put every extra functions inside this header
 */
#pragma once //protects against multiple inclusions of this header file

#include "LatLon.h"
#include "OSMID.h"
#include "math.h"
#include <string>
#include <vector>
#include <utility>
#include <limits>
#include <iostream>
#include <array>
class XY_;


//it converts a pair of LatLon to a pair of XY_
std::pair<XY_,XY_> pair_of_LatLon_to_XY(std::pair<LatLon,LatLon> points);


/************************************************************************************
 *                                                                                  *
 * This is a new struct XY_, similar to LatLon but it holds x,y coordinates instead  *
 *                                                                                  *
 * The usage is almost exactly the same as LatLon                                          *
 *                                                                                  *
 ************************************************************************************/

struct XY_
{
    int x_;
    int y_;
};

std::ostream& operator<<(std::ostream& os,XY_);


// Convert two (lat,lon) pairs to 4 corners of a bounding box.
std::array<XY_,4> bounds_to_corners(std::pair<XY_,XY_> bounds);

/******************************End of XY_ Definition********************************/
