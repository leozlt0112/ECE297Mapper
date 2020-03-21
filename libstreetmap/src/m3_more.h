/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m3_more.h
 * Author: wangbox2
 *
 * Created on March 12, 2020, 10:32 AM
 */

#pragma once //protects against multiple inclusions of this header file
#include "structs_and_classes.h"
#include "m1.h"
#include "m2.h"
#include "m2_more.h"
#include <queue> //std::priority_queue
/************ These are the data structures that will be used ***************/

// a vector[Node_idx] storing Nodes
// Node_idx is the same as intersection idx
extern std::vector<Node> nodes;

// a vector[Edge_idx] storing Edges
// Edge_idx is not the same as segment idx
extern std::vector<Edge> edges;

// a vector[node_idx] storing nodes
// node idx == intersection indx
extern std::vector<intersection_info> intersections;

// a vector[edge_idx] storing edges
// edge idx != segment indx
extern std::vector<segment_info> streetSegments;

// a vector[streetSegIndex], each element stores distance. from m1_more.h
extern std::vector<double> streetSeg_length;

// a vector[streetSegment_idx] storing street_segment_data
extern std::vector<segment_info> streetSegments;

/************ These are functions called in program ***************/
extern std::vector<double> streetSeg_time;

std::vector<StreetSegmentIndex> path_search_result(const IntersectionIndex intersect_id_end);

std::vector<StreetSegmentIndex> find_walking_path(const IntersectionIndex start_intersection, const IntersectionIndex walk_intersection, const double turn_penalty,
                                                   const double walking_speed);
