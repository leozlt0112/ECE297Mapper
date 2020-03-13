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
#include "m2.h"
#include "m2_more.h"

/************ These are the data structures that will be used ***************/

// a vector[Node_idx] storing Nodes
// Node_idx is the same as intersection idx
std::vector<Node> nodes;

// a vector[Edge_idx] storing Edges
// Edge_idx is not the same as segment idx
std::vector<Edge> edges;

// a vector[node_idx] storing nodes
// node idx == intersection indx
extern std::vector<intersection_info> intersections;

// a vector[edge_idx] storing edges
// edge idx != segment indx
extern std::vector<segment_info> streetSegments;

// a vector[streetSegIndex], each element stores distance. from m1_more.h
extern std::vector<double> streetSeg_length;

/************ These are functions called in program ***************/

void pathFind_load();