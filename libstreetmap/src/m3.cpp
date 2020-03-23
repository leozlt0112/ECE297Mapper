#include "m3.h"
#include "m3_more.h"
#include <queue> //std::priority_queue
#define NO_EDGE -1

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



// Returns a path (route) between the start intersection and the end
// intersection, if one exists. This routine should return the shortest path
// between the given intersections, where the time penalty to turn right or
// left is given by turn_penalty (in seconds).  If no path exists, this routine
// returns an empty (size == 0) vector.  If more than one path exists, the path
// with the shortest travel time is returned. The path is returned as a vector
// of street segment ids; traversing these street segments, in the returned
// order, would take one from the start to the end intersection.
std::vector<StreetSegmentIndex> find_path_between_intersections(
		          const IntersectionIndex intersect_id_start, 
                  const IntersectionIndex intersect_id_end,
                  const double turn_penalty){
   std::vector<StreetSegmentIndex> empty_path;
   
    //reset all elements 
    for(int i=0; i < nodes.size(); ++i){
        //reset reachingEdge to NO_EDGE
        nodes[i].reachingEdge= NO_EDGE;
        //reset bestTime of all nodes to 100000000.00
        nodes[i].bestTime = 100000000.00;
    }
    //define waveFront with root node with least travel time. 
    std::priority_queue<WaveElem, std::vector<WaveElem>, compareEstimateTravelTime> waveFront; //Nodes to explore next
   
    /************ A***************/
    //find the max (float speedLimit)speed limit (in km/h) of the city 
    //initialize max to the speed limit of streetseg[0]
    InfoStreetSegment this_info = getInfoStreetSegment(0);
    float max_speed_limit = this_info.speedLimit;
    for(int i = 1; i < getNumStreetSegments(); ++i) { 
       InfoStreetSegment seg_info = getInfoStreetSegment(i);
       float this_seg_speed_limit = seg_info.speedLimit;
       if(max_speed_limit < this_seg_speed_limit){
           max_speed_limit = this_seg_speed_limit;
       }
    }
     
 if(max_speed_limit != 0){  
    //distance from source to destination 
    LatLon  start_position = getIntersectionPosition(intersect_id_start);
    LatLon  end_position = getIntersectionPosition(intersect_id_end);
    std::pair<LatLon, LatLon> points = std::make_pair(start_position, end_position);
    //Returns the distance between two coordinates in meters
    double distance_start_to_end_m = find_distance_between_two_points(points);
    double distance_start_to_end_km = distance_start_to_end_m/1000;
    
    //estimateTravelTime for source ( travel time + distance/ max speed limit of the city)
    double source_estimateTravelTime = 0.0;
    source_estimateTravelTime = 0.0 + (distance_start_to_end_km / max_speed_limit)*3600;
    /************ A***************/
    
    
    waveFront.push(WaveElem(&(nodes[intersect_id_start]), NO_EDGE, 0.0, source_estimateTravelTime)); //source node
    
    while(!waveFront.empty()){
        WaveElem current_node = waveFront.top();
        //remove node from waveFront
        waveFront.pop();
        
        //update the edge with shorter travel time
        if(current_node.travelTime < current_node.node->bestTime){
            current_node.node->bestTime = current_node.travelTime;
            current_node.node->reachingEdge = current_node.edgeID;
            
            //reach destination
            if(current_node.node->idx_pnt == intersect_id_end){           
                return path_search_result(intersect_id_end);     
            }

            //go through all the outEdges of the current_node
            for(int i=0; i< (current_node.node->outEdges.size()); ++i){
                //get to_node of one outEdge
                Node* to_node = &(nodes[ edges[((current_node.node)->outEdges)[i]].to ]);                                       

                //update the travel time & estimateTravelTime
                double totalTravelTime = 0.0;
                
                //outEdge[i] edge travel time 
                double this_edgeTravelTime = edges[((current_node.node)->outEdges)[i]].edgeTravelTime;
                
                //check if turn
                //compare street id of this_seg with reaching_edge of current_node
                int turn = 0;
                if (current_node.edgeID != NO_EDGE){
                    int this_seg_id = edges[current_node.edgeID].idx_seg;
                    int next_seg_id = edges[current_node.node->outEdges[i]].idx_seg;
                    //std::cout << "\n\n\n -------------------\n"<<this_seg_id<<"\n"<<current_node.edgeID<<"\n -------------------\n\n\n";
                    InfoStreetSegment this_Seg_info = getInfoStreetSegment (this_seg_id);
                    InfoStreetSegment next_Seg_info = getInfoStreetSegment (next_seg_id);
                    //calculate turns by comparing street id with its the next street id. 
                    if(this_Seg_info.streetID != next_Seg_info.streetID){
                            turn = 1;
                    }
                }
                totalTravelTime = (current_node.node->bestTime) + this_edgeTravelTime + (turn*turn_penalty);

                
                /************ A***************/
                //calculate estimateTravelTime for outEdge[i]
                int to_node_id = to_node->idx_pnt;
                
                //distance from to_node to end_node
                LatLon  to_node_position = getIntersectionPosition(to_node_id);
                std::pair<LatLon, LatLon> points_new = std::make_pair(to_node_position, end_position);
                //the distance between two coordinates in meters
                double distance_toNode_to_end_m = find_distance_between_two_points(points_new);
                double distance_toNode_to_end_km = distance_toNode_to_end_m/1000;
                
                //estimateTravelTime for source ( travel time + distance/ max speed limit of the city)
                double toNode_estimateTravelTime = (distance_toNode_to_end_km / max_speed_limit)*3600;
                double total_estimateTravelTime = 0.0;
                total_estimateTravelTime = totalTravelTime + toNode_estimateTravelTime;
                /************ A***************/
                
                //update waveFront 
                waveFront.push(WaveElem(to_node, current_node.node->outEdges[i], totalTravelTime, total_estimateTravelTime)); 
            }   
        }                       
    }
    return empty_path;
 }
}
    

std::vector<StreetSegmentIndex> path_search_result(const IntersectionIndex intersect_id_end){
    std::vector<StreetSegmentIndex> path;
    Node* currentNode = &(nodes[intersect_id_end]);
    int pre_edge = currentNode->reachingEdge;
    int pre_seg;
    //transverse the path
    while(pre_edge != NO_EDGE){
        //update pre_seg
        pre_seg = edges[pre_edge].idx_seg;
        
        path.insert(path.begin(),pre_seg);
        
        // update current_node
        currentNode =  &(nodes[edges[pre_edge].from]);
        // update pre_edge
        pre_edge = currentNode->reachingEdge; 
        
    }
    
    return path;       
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
     
     //std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> temp;
     //std::vector<StreetSegmentIndex> temp1;
     //std::vector<StreetSegmentIndex> temp2;
     //temp = std::make_pair(temp1,temp2);
     //return temp;     

     //find k walkable intersection within time limit      
     //find drive path for all k intersections 
     
     //easy version
     std::vector <StreetSegmentIndex> walk_path;
     std::vector <StreetSegmentIndex> drive_path;
     std::vector <StreetSegmentIndex> best_drive_path;
     std::vector <StreetSegmentIndex> best_walk_path; 
     double driving_time = 99999999;
     for (int i=0; i<nodes.size(); i++) {
        // Node intersect_node = nodes [i];
         walk_path = find_walking_path(start_intersection, i, turn_penalty, walking_speed);
         double walking_time=compute_path_walking_time(walk_path, walking_speed, turn_penalty);
         if(walking_time < walking_time_limit) {
             drive_path = find_path_between_intersections(i,end_intersection,turn_penalty);
             double temp_time= compute_path_travel_time(drive_path,turn_penalty);
             if (driving_time > temp_time) {
                 driving_time = temp_time;
                 best_drive_path = drive_path;
                 best_walk_path = walk_path;
             }
         }
     }
      std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> path_with_walk;
      std::make_pair(best_drive_path, best_walk_path);
      return path_with_walk;
      
     /*Node walk_node; 
     int walk_inter_id;
     double walking_time = 0.0;
     double drive_time_before = 0.0;
     double drive_time_next = 0.0;     
     std::vector<StreetSegmentIndex> drive_path;
     std::vector<StreetSegmentIndex> walk_path;
     //go through all the intersections 
     for(int i = 0; i < nodes.size(); ++i) {
        walk_node = nodes[i]; 
        walk_inter_id = walk_node.idx_pnt;
        
        walk_path = find_walking_path(start_intersection, walk_inter_id, turn_penalty, walking_speed);
        //calculate walking time of the path
        walking_time = compute_path_walking_time(walk_path, walking_speed,turn_penalty);
        
        if(walking_time <= walking_time_limit){
            drive_path = find_path_between_intersections(walk_inter_id, end_intersection, turn_penalty);
            //check if the fastest path            
        }
     }*/
}
 
 
 std::vector<StreetSegmentIndex> find_walking_path(const IntersectionIndex start_intersection,
                                                   const IntersectionIndex walk_intersection,
                                                   const double turn_penalty,
                                                   const double walking_speed){
   // std::vector<StreetSegmentIndex> temp;
   // return temp;     
    // same variable is used here now for travel_time and walk_time
     
   std::vector<StreetSegmentIndex> empty_path;
   for(int i=0; i < nodes.size(); ++i){
        //reset reachingEdge to NO_EDGE
        nodes[i].reachingEdge= NO_EDGE;
        //reset bestTime of all nodes to 100000000.00
        nodes[i].bestTime = 100000000.00;
    }
    //define waveFront with root node with least travel time. 
    std::priority_queue<WaveElem, std::vector<WaveElem>, compareTravelTime> waveFront;
    waveFront.push(WaveElem(&(nodes[start_intersection]), NO_EDGE, 0.0)); //source node
    
    while(!waveFront.empty()){
        WaveElem current_node = waveFront.top();
        //remove node from waveFront
        waveFront.pop();
        
        //update the edge with shorter travel time
        if(current_node.travelTime < current_node.node->bestTime){             
            current_node.node->bestTime = current_node.travelTime;
            current_node.node->reachingEdge = current_node.edgeID;
            
            //reach destination
            if(current_node.node->idx_pnt == walk_intersection){           
                return path_search_result(walk_intersection);     
            }
            
           //go through all the outEdges of the current_node
            for(int i=0; i< (current_node.node->outEdges.size()); ++i){
                //get to_node of one outEdge
                // to_node of the current out_edge
                Node* to_node = &(nodes[ edges[((current_node.node)->outEdges)[i]].to ]);                                       

                //update the travel time 
                double totalTravelTime = 0.0;
                
                //outEdge[i] edge travel time 
                double this_edgeTravelTime = streetSeg_length[(edges[((current_node.node)->outEdges)[i]].idx_seg)] / walking_speed ;
                
                //check if turn
                //compare street id of this_seg with reaching_edge of current_node
                int turn = 0;
                if (current_node.edgeID != NO_EDGE){
                    int this_seg_id = edges[current_node.edgeID].idx_seg;
                    int next_seg_id = edges[current_node.node->outEdges[i]].idx_seg;
                    InfoStreetSegment this_Seg_info = getInfoStreetSegment (this_seg_id);
                    InfoStreetSegment next_Seg_info = getInfoStreetSegment (next_seg_id);
                    //calculate turns by comparing street id with its the next street id. 
                    if(this_Seg_info.streetID != next_Seg_info.streetID){
                            turn = 1;
                    }
                }
                totalTravelTime = (current_node.node->bestTime) + this_edgeTravelTime + (turn*turn_penalty);
                
                //update waveFront 
                waveFront.push(WaveElem(to_node, current_node.node->outEdges[i], totalTravelTime)); 
            }   
        }                       
    }
    return empty_path;
 }