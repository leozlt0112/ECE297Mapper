<<<<<<< HEAD
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
=======

double compute_path_travel_time(const std::vector<StreetSegmentIndex>& path, 
                                const double turn_penalty){
    double temp = 0.0;
    return temp;
}




std::vector<StreetSegmentIndex> find_path_between_intersections(
		          const IntersectionIndex intersect_id_start, 
                  const IntersectionIndex intersect_id_end,
                  const double turn_penalty){
    std::vector<StreetSegmentIndex>  temp;
    return temp;
}



// Returns the time required to "walk" along the path specified, in seconds.
// The path is given as a vector of street segment ids. The vector can be of
// size = 0, and in this case, it the function should return 0. The walking
// time is the sum of the length/<walking_speed> for each street segment, plus
// the given turn penalty, in seconds, per turn implied by the path. If there
// is no turn, then there is no penalty.  As mentioned above, going from Bloor
// Street West to Bloor street East is considered a turn
double compute_path_walking_time(const std::vector<StreetSegmentIndex>& path, 
                                 const double walking_speed, 
                                 const double turn_penalty){  
    double walking_time = 0.0;
    double seg_walking_time = 0.0;
    int turns = 0; 
   //empty vector
    if(path.size() == 0){
        return 0;
    }
       
    //count turns in the path (whenever the street id changes) 
    for(int i=0; i< (path.size()-1); ++i){
        InfoStreetSegment this_Seg_info = getInfoStreetSegment (path[i]);
        InfoStreetSegment next_Seg_info = getInfoStreetSegment (path[i+1]);
        
        //calculate turns by comparing street id with its the next street id. 
        if(this_Seg_info.streetID != next_Seg_info.streetID){
            turns = turns +1;
        }
    }
       
    //adds up walking time for all the segs in the path
    for(int segCount=0; segCount< path.size(); ++segCount){       
        seg_walking_time += (streetSeg_length[path[segCount]] / walking_speed);
    }
    
        
    //The walking time is the sum of the length/<walking_speed> for each street segment, plus
    // the given turn penalty, in seconds, per turn implied by the path
    walking_time =  seg_walking_time + (turns*turn_penalty);
    return walking_time;
}

