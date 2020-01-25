/***********************************************
 * Put every extra function inside this header *
 ***********************************************/

#pragma once //protects against multiple inclusions of this header file
class XY_;


//it converts a pair of LatLon to a pair of XY_
std::pair<XY_,XY_> pair_of_LatLon_to_XY(std::pair<LatLon,LatLon> points);

//this is a vector[IntersectionIndex], each intersection vector stores StreetSegID ints
std::vector<std::vector<int>> intersection_street_segments;

//This is a new class XY_, similar to LatLon but it holds x,y coordinates instead
class XY_{
public:
    double x_;
    double y_;
    XY_(){}
    XY_(double x_in,double y_in){x_=x_in; y_=y_in;}
};