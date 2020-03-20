/***********************************************
 * Put every extra function inside this header *
 ***********************************************/

#pragma once //protects against multiple inclusions of this header file
#include "structs_and_classes.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include <map>

// it converts a pair of LatLon to a pair of XY_
std::pair<XY_,XY_> LatLon_to_XY(std::pair<LatLon,LatLon> points);
std::vector<XY_> LatLon_to_XY(std::vector<LatLon> points);

// a vector[IntersectionIndex], each intersection vector stores StreetSegID ints
std::vector<std::vector<int>> intersection_street_segments;

// a vector[StreetIndex], each street vector stores streetSegmentIDs
std::vector<std::vector<int>> street_street_segments;

// a vector[StreetIndex], each street vector stores IntersectionIDs
std::vector<std::vector<int>> street_intersections;

// a vector[streetSegIndex], each element stores distance
std::vector<double> streetSeg_length;

// a vector[streetSegIndex], each element stores travel time
std::vector<double> streetSeg_time;

// a map <Street_Name,StreetIndex> for all streets
std::multimap<std::string, int> streetID_streetName;

// a vector[streetIndex], each element stores distance
std::vector<double> streetID_streetLength;

// a variable used to store all possible all features
std::vector<std::vector<LatLon>> featureID_featurePts;

// a map, each stores <node ID, node>
std::unordered_map<OSMID, const OSMNode*> NodeID_Node;

// a map, each stores <way ID, way>
std::unordered_map<OSMID, const OSMWay*> WayID_Way;

// a map, each stores <way ID, way length>
std::unordered_map<OSMID, double> WayID_length;

int find_closest_POI(LatLon my_position);

void pathFind_load();

// a vector[Node_idx] storing Nodes
// Node_idx is the same as intersection idx
std::vector<Node> nodes;

// a vector[Edge_idx] storing Edges
// Edge_idx is not the same as segment idx
std::vector<Edge> edges;