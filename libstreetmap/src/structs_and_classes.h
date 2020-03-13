#pragma once
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "math.h"
#include "rectangle.hpp"
#include "graphics.hpp"
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>    // std::sort
#include <iostream>
#include <fstream>

/************ M 1 ***************/

//This is a new class XY_, similar to LatLon but it holds x,y coordinates instead
class XY_{
public:
    double x_;
    double y_;
    XY_(){}
    XY_(double x_in,double y_in){x_=x_in; y_=y_in;}
};

/************ M 2 ***************/
struct intersection_info{
    float x_;
    float y_;
    std::string name;
    bool highlight = false;
};
 
struct segment_info {
    bool oneWay;                                // if true, then can only travel in from->to direction
    std::vector<ezgl::point2d> allPoints;       // from, curve points, to
    float speedLimit;                           // in km/h
    OSMID wayOSMID;                             // index of way this segment belongs to
};

struct feature_info {
    bool closed = false;                        // if true, it's closed (poly))
    std::string name;                           // feature name
    FeatureType type;                           // feature type
    std::vector<ezgl::point2d> allPoints;       // from, curve points, to
    float area;                                 // area
};

struct poi_info {
    float x_;                                   // position x
    float y_;                                   // position y
    std::string type;
    std::string name;                           // poi name
    bool highlight = false; 
};

struct highway_info {
    std::unordered_map<std::string,std::string> tags;   // store all the tags in a unordered_map
    bool oneWay = false;                                        // whether one way or not
    std::vector<ezgl::point2d> allPoints;               // from, curve points, to
    OSMID wayOSMID;                                     // ID of this way
    bool highlight = false;
};

struct railway_info {
    std::unordered_map<std::string,std::string> tags;   // store all the tags in a unordered_map
    std::vector<ezgl::point2d> allPoints;               // from, curve points, to
    OSMID wayOSMID;                                     // ID of this way
};

/************ M 3 ***************/

// this is a variable type for each Node.
// it's similar to intersections but focus on path finding
struct Node{
    // intersection index
    int idx_pnt;
    // idx's of Edges that can be traveled from node
    std::vector<int> outEdges;
    // idx of Edge to reach this Node during path finding
    // -1 means no edge
    int reachingEdge=-1;
    // Shortest time to travel from starting point to this Node
    // remember to set it to large number at the beginning of each path finding function
    double  bestTime;
};

// this is a variable type for each Edge.
// it's similar to streetSegments but focus on path finding
struct Edge{
    // segment index
    int idx_seg;
    // idx of "from" Node
    int from;
    // idx of "to" Node
    int to;
};

// this is a variable type for each element in variable WaveFront
// this is a variable type during path finding only, does not store final result
struct WaveElem{
    Node* node;
    int edgeID;
    WaveElem(Node* n, int id) {node = n; edgeID = id;}
};



