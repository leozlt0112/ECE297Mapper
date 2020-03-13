/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m3.h"
#include "m3_more.h"

void pathFind_load(){
    nodes.resize(intersections.size());
    
    // std::vector<Edge> edges;
    // std::vector<Node> nodes; => outEdges only
    for(size_t i = 0; i < streetSegments.size(); ++i) {
        // get the info
        InfoStreetSegment this_info = getInfoStreetSegment(i);
        // declare a new edge
        Edge new_edge;
        
        // store one direction edge
        new_edge.idx_seg = i;
        new_edge.from    = this_info.from;
        new_edge.to      = this_info.to;
        
        // push one direction edge
        edges.push_back(new_edge);
        
        // store into outEdges of "from" Node
        // because current edge is always the last element, edge_idx = size-1
        nodes[new_edge.from].outEdges.push_back(edges.size()-1);
        
        if (!(this_info.oneWay)){
            // store opposite direction edge
            new_edge.from = this_info.to;
            new_edge.to   = this_info.from;

            // push opposite direction edge
            edges.push_back(new_edge);
            
            // store into outEdges of "from" Node
            // because current edge is always the last element, edge_idx = size-1
            nodes[new_edge.from].outEdges.push_back(edges.size()-1);
        }
    }
    
    // std::vector<Node> nodes;
    for(size_t i = 0; i < nodes.size(); ++i) {
        nodes[i].idx_pnt = i;
    }
}
