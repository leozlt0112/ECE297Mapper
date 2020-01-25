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
struct XY_;


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
/******************************End of XY_ Definition********************************/
