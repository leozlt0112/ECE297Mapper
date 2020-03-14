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

// Returns the time required to travel along the path specified, in seconds.
// The path is given as a vector of street segment ids, and this function can
// assume the vector either forms a legal path or has size == 0.  The travel
// time is the sum of the length/speed-limit of each street segment, plus the
// given turn_penalty (in seconds) per turn implied by the path.  If there is
// no turn, then there is no penalty. Note that whenever the street id changes
// (e.g. going from Bloor Street West to Bloor Street East) we have a turn.
double compute_path_travel_time(const std::vector<StreetSegmentIndex>& path, 
                                const double turn_penalty){
    //double temp = 0.0;
    //return temp;
    double travel_time = 0.0;
    double seg_travel_time = 0.0;
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
     
    //adds up travel time for all the segs in the path
    for(int segCount=0; segCount< path.size(); ++segCount){ 
        // DAVE's COMMENT: don't calculate the division here, go to M1, 
        // there is already a function find_street_segment_travel_time()
        // and a data structure streetSeg_time[]
        // use either one of them to substitute the division.
        seg_travel_time += streetSeg_time[path[segCount]];
    }    
        
    //The travel time is the sum of the length/speed-limit of each street segment, plus the
    // given turn_penalty (in seconds) per turn implied by the path. 
    travel_time =  seg_travel_time + (turns*turn_penalty);
    return travel_time;
}




std::vector<StreetSegmentIndex> find_path_between_intersections(
		          const IntersectionIndex intersect_id_start, 
                  const IntersectionIndex intersect_id_end,
                  const double turn_penalty){
    std::vector<StreetSegmentIndex>  temp;
    return temp;
}



double compute_path_walking_time(const std::vector<StreetSegmentIndex>& path, 
                                 const double walking_speed, 
                                 const double turn_penalty){  
    double walking_time = 0.0;
    double seg_walking_time = 0.0;
    int turns = 0; 
   //empty vector
    if(path.size() == 0){
        return 0.0;
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

// This is an "uber pool"-like function. The idea is to minimize driving travel
// time by walking to a pick-up intersection (within walking_time_limit secs)
// from start_intersection while waiting for the car to arrive.  While walking,
// you can ignore speed limits of streets and just account for given
// walking_speed [m/sec]. However, pedestrians should also wait for traffic
// lights, so turn_penalty applies to whether you are driving or walking.
// Walking in the opposite direction of one-way streets is fine. Driving is
// NOT!  The routine returns a pair of vectors of street segment ids. The first
// vector is the walking path starting at start_intersection and ending at the
// pick-up intersection. The second vector is the driving path from pick-up
// intersection to end_interserction.  Note that a start_intersection can be a
// pick-up intersection. If this happens, the first vector should be empty
// (size = 0).  If there is no driving path found, both returned vectors should
// be empty (size = 0). 
// If the end_intersection turns out to be within the walking time limit, 
// you can choose what to return, but you should not crash. If your user 
// interface can call this function for that case, the UI should handle
// whatever you return in that case.
 std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> 
         find_path_with_walk_to_pick_up(
                          const IntersectionIndex start_intersection, 
                          const IntersectionIndex end_intersection,
                          const double turn_penalty,
                          const double walking_speed, 
                          const double walking_time_limit){
     std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> temp;
     std::vector<StreetSegmentIndex> temp1;
     std::vector<StreetSegmentIndex> temp2;
     temp = std::make_pair(temp1,temp2);
     return temp;
}