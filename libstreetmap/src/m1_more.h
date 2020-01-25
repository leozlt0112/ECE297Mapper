/* 
 * Put everything extra functions inside this header
 */
#pragma once //protects against multiple inclusions of this header file

#include "LatLon.h"
#include "OSMID.h"
#include "math.h"
#include <string>
#include <vector>
#include <utility>

std::pair<std::pair<int, int>,std::pair<int, int>> pair_of_LatLon_to_XY(std::pair<LatLon,LatLon> points);