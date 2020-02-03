/***********************************************
 * Put every extra function inside this header *
 ***********************************************/

#pragma once //protects against multiple inclusions of this header file
#include <map>
class XY_;


//it converts a pair of LatLon to a pair of XY_
std::pair<XY_,XY_> LatLon_to_XY(std::pair<LatLon,LatLon> points);
std::vector<XY_> LatLon_to_XY(std::vector<LatLon> points);

//this is a vector[IntersectionIndex], each intersection vector stores StreetSegID ints
std::vector<std::vector<int>> intersection_street_segments;

//a vector[StreetIndex], each street vector stores streetSegmentIDs
std::vector<std::vector<int>> street_street_segments;

//a vector[StreetIndex], each street vector stores IntersectionIDs
std::vector<std::vector<int>> street_intersections;

//a vector[streetSegIndex], each element stores travel time
std::vector<double> streetSeg_time;

//a map<Street_Name,StreetIndex> for all streets
std::multimap<std::string, int> streetID_streetName;

// variable used to store all possible all features
std::vector<std::vector<LatLon>> featureID_featurePts;

// a map, each stores <node ID, node index>
std::map<OSMID, int> OSMID_NodeIdx;

// a map, each stores <way ID, way length> each stores length of way
std::map<OSMID, double> wayID_length;

//This is a new class XY_, similar to LatLon but it holds x,y coordinates instead
class XY_{
public:
    double x_;
    double y_;
    XY_(){}
    XY_(double x_in,double y_in){x_=x_in; y_=y_in;}
};