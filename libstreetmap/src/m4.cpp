/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "m4_more.h"
#include "m4.h"

// This routine takes in a vector of N deliveries (pickUp, dropOff
// intersection pairs), another vector of M intersections that
// are legal start and end points for the path (depots), a turn 
// penalty in seconds (see m3.h for details on turn penalties), 
// and the truck_capacity in pounds.
//
// The first vector 'deliveries' gives the delivery information.  Each delivery
// in this vector has pickUp and dropOff intersection ids and the weight (also
// in pounds) of the delivery item. A delivery can only be dropped-off after
// the associated item has been picked-up. 
// 
// The second vector 'depots' gives the intersection ids of courier company
// depots containing trucks; you start at any one of these depots and end at
// any one of the depots.
//
// This routine returns a vector of CourierSubpath objects that form a delivery route.
// The CourierSubpath is as defined above. The first street segment id in the
// first subpath is connected to a depot intersection, and the last street
// segment id of the last subpath also connects to a depot intersection.  The
// route must traverse all the delivery intersections in an order that allows
// all deliveries to be made with the given truck capacity. Addionally, a package
// should not be dropped off if you haven't picked it up yet.
//
// The start_intersection of each subpath in the returned vector should be 
// at least one of the following (a pick-up and/or drop-off can only happen at 
// the start_intersection of a CourierSubpath object):
//      1- A start depot.
//      2- A pick-up location (and you must specify the indices of the picked 
//                              up orders in pickup_indices)
//      3- A drop-off location. 
//
// You can assume that N is always at least one, M is always at least one
// (i.e. both input vectors are non-empty), and truck_capacity is always greater
// or equal to zero.
//
// It is legal for the same intersection to appear multiple times in the pickUp
// or dropOff list (e.g. you might have two deliveries with a pickUp
// intersection id of #50). The same intersection can also appear as both a
// pickUp location and a dropOff location.
//        
// If you have two pickUps to make at an intersection, traversing the
// intersection once is sufficient to pick up both packages, as long as the
// truck_capcity fits both of them and you properly set your pickup_indices in
// your courierSubpath.  One traversal of an intersection is sufficient to
// drop off all the (already picked up) packages that need to be dropped off at
// that intersection.
//
// Depots will never appear as pickUp or dropOff locations for deliveries.
//  
// If no valid route to make *all* the deliveries exists, this routine must
// return an empty (size == 0) vector.

/*
std::vector<CourierSubpath> traveling_courier_a(
		            const std::vector<DeliveryInfo>& deliveries,
	       	        const std::vector<int>& depots, 
		            const float turn_penalty, 
		            const float truck_capacity){
    std::cout<<"function called\n";
    // deliveries_flags that records whether picked up or dropped off
    // Updates after a pickup or a drop-off
    std::vector<std::pair<bool,bool>> pickUp_dropOff_flags(deliveries.size(),std::make_pair(false,false));
    
    // truck_deliv_idxs records all the <deliveries indices, 0> on the truck
    // Push back an element after a pickup. remove an element after a drop-off
    std::unordered_set<int> truck_deliv_idxs;
    // truck_deliv_weight records the total weight on the truck
    // Increases after a pickup, decreases after a drop-off
    float                       truck_deliv_weight = 0;
    // truck_pos_idx records the current position of the truck
    // Updates every time the truck moves from one point to another
    int                         truck_inter_idx;
    // truck_pos_latlon records the current position of the truck
    // Updates every time the truck moves from one point to another
    LatLon                      truck_inter_latlon;
    
    // check_pos_idx temporarily records the position we are checking
    // Updates every time we are checking another point
    int             check_inter_idx;
    // check_pos_latlon temporarily records the position we are checking
    // Updates every time we are checking another point
    LatLon          check_inter_latlon;
    // check_length records the distance between truck and check
    // Updates every time we are checking another point
    int             check_length;
    // check_deliv_idx records the deliveries index we are checking
    // Updates every time we are checking another point
    // this is the same thing as i inside for loop
    
    // result_inter_idx records the closest position we found
    // Updates every time we found another point with closer position
    int             result_inter_idx = -1;
    // result_inter_latlon records the closest position we found
    // Updates every time we found another point with closer position
    LatLon          result_inter_latlon;
    // result_length records the shortest distance we found
    // Updates every time we found another point with closer position
    int             result_length = 999999999;
    // result_deliv_idx records the deliveries index with shortest distance
    // Updates every time we found another point with closer position
    int             result_deliv_idx;
    // result_deliv_idx records whether we are going to pick up or drop off
    // Updates every time we found another point with closer position
    // true = pickup      false = drop-off
    bool            result_deliv_stat = true;
    
    // start_intersection_pickups records the pickups of start_intersections
    std::vector<unsigned> start_intersection_pickups;
    // final_path records the final path to return
    // Updates every time we found another path
    std::vector<CourierSubpath> final_path;
    
    // find a random depot to begin with
    truck_inter_idx    = depots[depots.size()/2];
    truck_inter_latlon = getIntersectionPosition(truck_inter_idx);
    
    // keep picking up and dropping off until truck is empty
    do{
        result_length = 999999999;
        result_inter_idx = -1;
        
        // find the closest delivery point
        for (int i = 0; i < deliveries.size(); ++i){
            // check the pickup of deliveries[i]
            check_inter_idx = deliveries[i].pickUp;
            // check legality: not picked up yet && do not exceed weight limit
            if (!pickUp_dropOff_flags[i].first && (truck_deliv_weight + deliveries[i].itemWeight) < truck_capacity){
                // check distance: shorter than the result_length
                check_inter_latlon = getIntersectionPosition(check_inter_idx);
                check_length = find_distance_between_two_points(std::make_pair(truck_inter_latlon,check_inter_latlon));
                if (check_length < result_length){
                    result_inter_idx    = check_inter_idx;
                    result_inter_latlon = check_inter_latlon;
                    result_length       = check_length;
                    result_deliv_idx    = i;
                    result_deliv_stat   = true;
                }
            }
            
            // check the drop-off of deliveries[i]
            check_inter_idx = deliveries[i].dropOff;
            // check legality: picked up but not dropped off yet
            if (pickUp_dropOff_flags[i].first && !pickUp_dropOff_flags[i].second){
                // check distance: shorter than the result_length
                check_inter_latlon = getIntersectionPosition(check_inter_idx);
                check_length = find_distance_between_two_points(std::make_pair(truck_inter_latlon,check_inter_latlon));
                if (check_length < result_length){
                    result_inter_idx    = check_inter_idx;
                    result_inter_latlon = check_inter_latlon;
                    result_length       = check_length;
                    result_deliv_idx    = i;
                    result_deliv_stat   = false;
                }
            }         
        }
        
        // if it is -1, it means there are no more deliveries, time to stop
        if (result_inter_idx != -1){
            // update the final path when it's actually moving
            if (truck_inter_idx != result_inter_idx){
                CourierSubpath subpath;
                subpath.start_intersection = truck_inter_idx;
                subpath.end_intersection   = result_inter_idx;
                subpath.subpath = find_path_between_intersections(truck_inter_idx,result_inter_idx,turn_penalty);
                subpath.pickUp_indices = start_intersection_pickups;
                start_intersection_pickups.clear();
                final_path.push_back(subpath);
            }

            // update start_intersection_pickups
            if (result_deliv_stat){
                start_intersection_pickups.push_back(result_deliv_idx);
            }

            // update the truck load and flags
            //  picked up
            if (result_deliv_stat){
                // update the truck load
                truck_deliv_idxs.insert(result_deliv_idx);
                truck_deliv_weight += deliveries[result_deliv_idx].itemWeight;
                // update the flags
                pickUp_dropOff_flags[result_deliv_idx].first = true;
            }
            //  dropped off
            else{
                // update the truck load
                truck_deliv_idxs.erase(result_deliv_idx);
                truck_deliv_weight -= deliveries[result_deliv_idx].itemWeight;
                // update the flags
                pickUp_dropOff_flags[result_deliv_idx].second = true;
            }
            // update the truck position
            truck_inter_idx    = result_inter_idx;
            truck_inter_latlon = result_inter_latlon;
        }
    }while (result_inter_idx != -1);
    
    // find a closest depot to end
    result_length = 999999999;
    for (int i = 0; i < depots.size(); ++i){
        check_inter_idx = depots[i];
        check_inter_latlon = getIntersectionPosition(check_inter_idx);
        check_length = find_distance_between_two_points(std::make_pair(truck_inter_latlon,check_inter_latlon));
        if (check_length < result_length){
            result_inter_idx    = check_inter_idx;
            result_length       = check_length;
        }
    }
    
    // update the final path
    CourierSubpath subpath;
    subpath.start_intersection = truck_inter_idx;
    subpath.end_intersection   = result_inter_idx;
    subpath.subpath = find_path_between_intersections(truck_inter_idx,result_inter_idx,turn_penalty);
    final_path.push_back(subpath);
    
    // return the path
    return final_path;
}*/
/*
std::vector<CourierSubpath> traveling_courier_b(
		            const std::vector<DeliveryInfo>& deliveries,
                            const std::vector<int>& depots, 
		            const float turn_penalty, 
		            const float truck_capacity){
    std::cout<<"function called\n";
    
    // all_inter_pickups stores all the intersection idx of both pick up and 
    // drop-off of deliveries. 
    // unordered_multimap<inter_idx, tuple<deliv/depot_idx, pick_upORdelivery, delivORdepot>>
    // creating a list of all t
    // the type of all pickup, dropff and depots[i] are all intersection idxes
    std::unordered_multimap<int, std::tuple<int,bool,bool>> all_inter_pickups;
    for (int i = 0; i < deliveries.size(); ++i){
        all_inter_pickups.insert(std::make_pair(deliveries[i].pickUp,
                                                    std::make_tuple(i,true,true)));
    }
    // all_inter_deliv_and_depot stores all the intersection idx of pickups, 
    // drop-offs of deliveries and the depots.
    // unordered_multimap<inter_idx, tuple<deliv/depot_idx, pORd, delivORdepot>>
    std::unordered_multimap<int, std::tuple<int,bool,bool>> all_inter_deliv_and_depot
                                                              = all_inter_pickups;
    for (int i = 0; i < deliveries.size(); ++i){
        all_inter_deliv_and_depot.insert(std::make_pair(deliveries[i].dropOff,
                                                    std::make_tuple(i,false,true)));
    }
    for (int i = 0; i < depots.size(); ++i){
        all_inter_deliv_and_depot.insert(std::make_pair(depots[i],
                                                    std::make_tuple(i,false,false)));
    }
    
    // all_paths_depot_deliv stores all the paths from any depot to any delivery
    // all paths need to be sorted, ex shortest path on top and longest path is on bottom
    // no, shortest path on one end, longest path on the other end
    // multimap<travel_time, tuple<path, end_inter_idx, end_deliv_idx, pORd, start_inter_idx, delivORdepot>>
    std::multimap<float, std::tuple<std::vector<StreetSegmentIndex>,int,int,bool,int,bool>>
                                                        all_paths_depot_deliv;
    // finds all the paths from depos to all pickup intersections
    all_paths_depot_deliv = find_path_between_intersections_multi_starts_ends(
                                                        depots, 
                                                        all_inter_pickups,
                                                        turn_penalty);
    // // storing all the paths from delivery(pick up/dropoff) to any other delivery(pick up/ drop off) or depot(final dest)
    // all_paths_deliv_deliv stores all the paths from any delivery to any delivery/depot
    // unordered_map<start_inter_idx, multimap<time, tuple<path, end_inter_idx, end_deliv_idx, pORd, delivORdepot>>>
    std::unordered_map<int,std::multimap<float, std::tuple<std::vector<StreetSegmentIndex>,int,int,bool,bool>>>
                                                        all_paths_deliv_deliv;
    for (const auto& i: all_inter_deliv_and_depot){
        all_paths_deliv_deliv.insert(
        std::make_pair( 
        i.first,
        find_path_between_intersections_multi_ends( i.first,
                                                    all_inter_deliv_and_depot,
                                                    turn_penalty)));
    }
    
    // deliveries_flags that records whether picked up or dropped off
    // Updates after a pickup or a drop-off
    std::vector<std::pair<bool,bool>> pickUp_dropOff_flags(deliveries.size(),std::make_pair(false,false));
    
    // truck_deliv_idxs records all the <deliveries indices> on the truck
    // Push back an element after a pickup. remove an element after a drop-off
    std::unordered_set<int>     truck_deliv_idxs;
    // truck_deliv_weight records the total weight on the truck
    // Increases after a pickup, decreases after a drop-off
    float                       truck_deliv_weight = 0;
    // truck_pos_idx records the current position of the truck
    // Updates every time the truck moves from one point to another
    int                         truck_inter_idx;
    // truck_pos_latlon records the current position of the truck
    // Updates every time the truck moves from one point to another
    LatLon                      truck_inter_latlon;
    
    // check_pos_idx temporarily records the position we are checking
    // Updates every time we are checking another point
    int             check_inter_idx;
    // check_pos_latlon temporarily records the position we are checking
    // Updates every time we are checking another point
    LatLon          check_inter_latlon;
    // check_length records the distance between truck and check
    // Updates every time we are checking another point
    int             check_length;
    // check_deliv_idx records the deliveries index we are checking
    // Updates every time we are checking another point
    /* this is the same thing as i inside for loop 
    
    // result_inter_idx records the closest position we found
    // Updates every time we found another point with closer position
    int              result_inter_idx = -1;
    // result_inter_latlon records the closest position we found
    // Updates every time we found another point with closer position
    LatLon           result_inter_latlon;
    // result_length records the shortest distance we found
    // Updates every time we found another point with closer position
    int              result_length = 999999999;
    // result_deliv_idx records the deliveries index with shortest distance
    // Updates every time we found another point with closer position
    int              result_deliv_idx;
    // result_deliv_idx records whether we are going to pick up or drop off
    // Updates every time we found another point with closer position
    // true = pickup      false = drop-off
    bool             result_deliv_stat = true;
    // result_path records the resulting path found
    std::vector<int> result_path;
    // a dummy variable for checking if legal result is found
    bool found = false;
    
    // start_intersection_pickups records the pickups of start_intersections
    std::vector<unsigned> start_intersection_pickups;
    // final_path records the final path to return
    // Updates every time we found another path
    std::vector<CourierSubpath> final_path;
    
    // find a random depot to begin with
    int starting_depot_idx = depots.size()/2;
    truck_inter_idx    = depots[starting_depot_idx];
    truck_inter_latlon = getIntersectionPosition(truck_inter_idx);
    
    // find closest delivery from depot
    auto it = all_paths_depot_deliv.begin();
    result_path       = std::get<0>(it -> second);
    result_inter_idx  = std::get<1>(it -> second);
    result_deliv_idx  = std::get<2>(it -> second);
    result_deliv_stat = std::get<3>(it -> second);
    truck_inter_idx   = std::get<4>(it -> second);
    result_inter_latlon = getIntersectionPosition(result_inter_idx);
    // update the final path
    CourierSubpath path_start;
    path_start.start_intersection = truck_inter_idx;
    path_start.end_intersection   = result_inter_idx;
    path_start.subpath = result_path;
    path_start.pickUp_indices = start_intersection_pickups;
    start_intersection_pickups.clear();
    final_path.push_back(path_start);
    // update start_intersection_pickups
    start_intersection_pickups.push_back(result_deliv_idx);
    // update the truck load
    truck_deliv_idxs.insert(result_deliv_idx);
    truck_deliv_weight += deliveries[result_deliv_idx].itemWeight;
    // update the flags
    pickUp_dropOff_flags[result_deliv_idx].first = true;
    // update the truck position
    truck_inter_idx = result_inter_idx;
    truck_inter_latlon = result_inter_latlon;
    
    // keep picking up and dropping off until no more deliveries
    do{
        result_inter_idx = -1;
        // std::cout<<"hey2\n";
        // find the closest delivery from delivery
        found = false;
        auto check_all_paths = all_paths_deliv_deliv.find(truck_inter_idx)->second;
        for (auto itr = check_all_paths.begin(); itr != check_all_paths.end() && !found; ++itr){
            // check legality:
            int i = std::get<2>(itr -> second);
            bool is_delivery_spot = std::get<4>(itr -> second);
            bool check_deliv_stat = std::get<3>(itr -> second);
            // is a delivery spot &&(
            //     (is a pickUp spot  && 
            //      not picked up yet && 
            //      do not exceed weight limit) ||
            //     (is a dropOff spot &&
            //      picked up         &&
            //      not dropped off yet)            )
            if (  is_delivery_spot &&
                ((check_deliv_stat && !pickUp_dropOff_flags[i].first && (truck_deliv_weight + deliveries[i].itemWeight) < truck_capacity) ||
                (!check_deliv_stat &&  pickUp_dropOff_flags[i].first && !pickUp_dropOff_flags[i].second))){
                result_path       = std::get<0>(itr -> second);
                result_inter_idx  = std::get<1>(itr -> second);
                result_deliv_idx  = std::get<2>(itr -> second);
                result_deliv_stat = std::get<3>(itr -> second);
                result_inter_latlon = getIntersectionPosition(result_inter_idx);
                // update the final path when it's actually moving
                if (truck_inter_idx != result_inter_idx){
                    CourierSubpath subpath;
                    subpath.start_intersection = truck_inter_idx;
                    subpath.end_intersection   = result_inter_idx;
                    subpath.subpath = result_path;
                    subpath.pickUp_indices = start_intersection_pickups;
                    start_intersection_pickups.clear();
                    final_path.push_back(subpath);
                }

                // update start_intersection_pickups
                if (result_deliv_stat){
                    start_intersection_pickups.push_back(result_deliv_idx);
                }

                // update the truck load and flags
                //  picked up
                if (result_deliv_stat){
                    // update the truck load
                    truck_deliv_idxs.insert(result_deliv_idx);
                    truck_deliv_weight += deliveries[result_deliv_idx].itemWeight;
                    // update the flags
                    pickUp_dropOff_flags[result_deliv_idx].first = true;
                }
                //  dropped off
                else{
                    // update the truck load
                    truck_deliv_idxs.erase(result_deliv_idx);
                    truck_deliv_weight -= deliveries[result_deliv_idx].itemWeight;
                    // update the flags
                    pickUp_dropOff_flags[result_deliv_idx].second = true;
                }
                // update the truck position
                truck_inter_idx    = result_inter_idx;
                truck_inter_latlon = result_inter_latlon;
                found = true;
            }

        }
    }while (result_inter_idx != -1);
    
    // find a closest depot from delivery
    found = false;
    auto check_all_paths = all_paths_deliv_deliv.find(truck_inter_idx)->second;
    for (auto itr = check_all_paths.begin(); itr != check_all_paths.end() && !found; ++itr){
        // check legality: is not a delivery spot (== is a depot spot)
        bool is_delivery_spot = std::get<4>(itr -> second);
        if (!is_delivery_spot){
            result_path       = std::get<0>(itr -> second);
            result_inter_idx  = std::get<1>(itr -> second);
            // update the final path
            CourierSubpath subpath;
            subpath.start_intersection = truck_inter_idx;
            subpath.end_intersection   = result_inter_idx;
            subpath.subpath            = result_path;
            final_path.push_back(subpath);
            // update bool
            found = true;
        }
    }
    
    // return the path
    return final_path;
}
*/
std::vector<CourierSubpath> traveling_courier(
		            const std::vector<DeliveryInfo>& deliveries,
                            const std::vector<int>& depots, 
		            const float turn_penalty, 
		            const float truck_capacity){
    std::cout<<"function called\n";
    
    // all_inter_pickups stores all the intersection idx of both pick up and 
    // drop-off of deliveries. 
    // unordered_multimap<inter_idx, tuple<deliv/depot_idx, pick_upORdrop_off, delivORdepot>>
    // creating a list of all t
    // the type of all pickup, dropff and depots[i] are all intersection idxes
    std::unordered_multimap<int, std::tuple<int,bool,bool>> all_inter_pickups;
    for (int i = 0; i < deliveries.size(); ++i){
        all_inter_pickups.insert(std::make_pair(deliveries[i].pickUp,
                                                    std::make_tuple(i,true,true)));
    }
    // all_inter_deliv_and_depot stores all the intersection idx of pickups, 
    // drop-offs of deliveries and the depots.
    // unordered_multimap<inter_idx, tuple<deliv/depot_idx, pORd, delivORdepot>>
    std::unordered_multimap<int, std::tuple<int,bool,bool>> all_inter_deliv_and_depot
                                                              = all_inter_pickups;
    for (int i = 0; i < deliveries.size(); ++i){
        all_inter_deliv_and_depot.insert(std::make_pair(deliveries[i].dropOff,
                                                    std::make_tuple(i,false,true)));
    }
    for (int i = 0; i < depots.size(); ++i){
        all_inter_deliv_and_depot.insert(std::make_pair(depots[i],
                                                    std::make_tuple(i,false,false)));
    }
    
    // all_paths_depot_pickup stores all the paths from any depot to any pickup
    // all paths need to be sorted, ex shortest path on top and longest path is on bottom
    // no, shortest path on one end, longest path on the other end
    // multimap<travel_time, tuple<path, end_inter_idx, end_deliv_idx, pORd, start_inter_idx, delivORdepot>>
    std::multimap<float, std::tuple<std::vector<StreetSegmentIndex>,int,int,bool,int,bool>>
                                                        all_paths_depot_pickup;
    // finds all the paths from depos to all pickup intersections
    all_paths_depot_pickup = find_path_between_intersections_multi_starts_ends(
                                                        depots, 
                                                        all_inter_pickups,
                                                        turn_penalty);
    // // storing all the paths from delivery(pick up/dropoff) to any other delivery(pick up/ drop off) or depot(final dest)
    // all_paths_deliv_deliv stores all the paths from any delivery to any delivery/depot
    // unordered_map<start_inter_idx, multimap<time, tuple<path, end_inter_idx, end_deliv_idx, pORd, delivORdepot>>>
    std::unordered_map<int,std::multimap<float, std::tuple<std::vector<StreetSegmentIndex>,int,int,bool,bool>>>
                                                        all_paths_deliv_deliv;
    for (const auto& i: all_inter_deliv_and_depot){
        all_paths_deliv_deliv.insert(
        std::make_pair( 
        i.first,
        find_path_between_intersections_multi_ends( i.first,
                                                    all_inter_deliv_and_depot,
                                                    turn_penalty)));
    }
    
    // deliveries_flags that records whether picked up or dropped off
    // Updates after a pickup or a drop-off
    std::vector<std::pair<bool,bool>> pickUp_dropOff_flags(deliveries.size(),std::make_pair(false,false));
    
    // truck_deliv_idxs records all the <deliveries indices> on the truck
    // Push back an element after a pickup. remove an element after a drop-off
    std::unordered_set<int>     truck_deliv_idxs;
    // truck_deliv_weight records the total weight on the truck
    // Increases after a pickup, decreases after a drop-off
    float                       truck_deliv_weight = 0;
    // truck_pos_idx records the current position of the truck
    // Updates every time the truck moves from one point to another
    int                         truck_inter_idx;
    // truck_pos_latlon records the current position of the truck
    // Updates every time the truck moves from one point to another
    LatLon                      truck_inter_latlon;
    
    // check_pos_idx temporarily records the position we are checking
    // Updates every time we are checking another point
    int             check_inter_idx;
    // check_pos_latlon temporarily records the position we are checking
    // Updates every time we are checking another point
    LatLon          check_inter_latlon;
    // check_length records the distance between truck and check
    // Updates every time we are checking another point
    int             check_length;
    // check_deliv_idx records the deliveries index we are checking
    // Updates every time we are checking another point
    /* this is the same thing as i inside for loop */
    
    // result_inter_idx records the closest position we found
    // Updates every time we found another point with closer position
    int              result_inter_idx = -1;
    // result_inter_latlon records the closest position we found
    // Updates every time we found another point with closer position
    LatLon           result_inter_latlon;
    // result_length records the shortest distance we found
    // Updates every time we found another point with closer position
    int              result_length = 999999999;
    // result_deliv_idx records the deliveries index with shortest distance
    // Updates every time we found another point with closer position
    int              result_deliv_idx;
    // result_deliv_idx records whether we are going to pick up or drop off
    // Updates every time we found another point with closer position
    // true = pickup      false = drop-off
    bool             result_deliv_stat = true;
    // result_path records the resulting path found
    std::vector<int> result_path;
    // a dummy variable for checking if legal result is found
    bool found = false;
    
    // start_intersection_pickups records the pickups of start_intersections
    std::vector<unsigned> start_intersection_pickups;
    // final_path records the final path to return
    // Updates every time we found another path
    std::vector<CourierSubpath> final_path;
     std::vector<CourierSubpath> best_path;
//    std::multimap<float, std::vector<CourierSubpath>> all_final_paths;
    // find a random depot to begin with
    int starting_depot_idx = depots.size()/2;
    truck_inter_idx    = depots[starting_depot_idx];
    truck_inter_latlon = getIntersectionPosition(truck_inter_idx);
    double best_time= 99999999999; 
    // find closest delivery from depot
     //std::vector<CourierSubpath> best_path;
    for (int first=0; first<2; first++) {
        auto it = all_paths_depot_pickup.begin();
        std::advance(it, first);
        result_path       = std::get<0>(it -> second);
        result_inter_idx  = std::get<1>(it -> second);
        result_deliv_idx  = std::get<2>(it -> second);
        result_deliv_stat = std::get<3>(it -> second);
        truck_inter_idx   = std::get<4>(it -> second);
        result_inter_latlon = getIntersectionPosition(result_inter_idx);
        // update the final path
        // series of path along the path but ends earlier in the path
        CourierSubpath path_start;
        path_start.start_intersection = truck_inter_idx;
        path_start.end_intersection   = result_inter_idx;
        path_start.subpath = result_path;
        path_start.pickUp_indices = start_intersection_pickups;
        start_intersection_pickups.clear();
        final_path.clear();
        final_path.push_back(path_start);
        // update start_intersection_pickups
        start_intersection_pickups.push_back(result_deliv_idx);
        // update the truck load
        truck_deliv_idxs.insert(result_deliv_idx);
        truck_deliv_weight += deliveries[result_deliv_idx].itemWeight;
        // update the flags
        pickUp_dropOff_flags[result_deliv_idx].first = true;
        // update the truck position
        truck_inter_idx = result_inter_idx;
        truck_inter_latlon = result_inter_latlon;
    
        // keep picking up and dropping off until no more deliveries
        do{
            result_inter_idx = -1;
            // std::cout<<"hey2\n";
            // find the closest delivery from delivery
            found = false;
            auto check_all_paths = all_paths_deliv_deliv.find(truck_inter_idx)->second;
            for (auto itr = check_all_paths.begin(); itr != check_all_paths.end() && !found; ++itr){
                // check legality:
                int i = std::get<2>(itr -> second);
                bool is_delivery_spot = std::get<4>(itr -> second);
                bool check_deliv_stat = std::get<3>(itr -> second);
                // is a delivery spot &&(
                //     (is a pickUp spot  && 
                //      not picked up yet && 
                //      do not exceed weight limit) ||
                //     (is a dropOff spot &&
                //      picked up         &&
                //      not dropped off yet)            )
                if (  is_delivery_spot &&
                    ((check_deliv_stat && !pickUp_dropOff_flags[i].first && (truck_deliv_weight + deliveries[i].itemWeight) < truck_capacity) ||
                    (!check_deliv_stat &&  pickUp_dropOff_flags[i].first && !pickUp_dropOff_flags[i].second))){
                    result_path       = std::get<0>(itr -> second);
                    result_inter_idx  = std::get<1>(itr -> second);
                    result_deliv_idx  = std::get<2>(itr -> second);
                    result_deliv_stat = std::get<3>(itr -> second);
                    result_inter_latlon = getIntersectionPosition(result_inter_idx);
                    // update the final path when it's actually moving
                    if (truck_inter_idx != result_inter_idx){
                        CourierSubpath subpath;
                        subpath.start_intersection = truck_inter_idx;
                        subpath.end_intersection   = result_inter_idx;
                        subpath.subpath = result_path;
                        subpath.pickUp_indices = start_intersection_pickups;
                        start_intersection_pickups.clear();
                        final_path.push_back(subpath);
                    }

                    // update start_intersection_pickups
                    if (result_deliv_stat){
                        start_intersection_pickups.push_back(result_deliv_idx);
                    }

                    // update the truck load and flags
                    //  picked up
                    if (result_deliv_stat){
                        // update the truck load
                        truck_deliv_idxs.insert(result_deliv_idx);
                        truck_deliv_weight += deliveries[result_deliv_idx].itemWeight;
                        // update the flags
                        pickUp_dropOff_flags[result_deliv_idx].first = true;
                    }
                    //  dropped off
                    else{
                        // update the truck load
                        truck_deliv_idxs.erase(result_deliv_idx);
                        truck_deliv_weight -= deliveries[result_deliv_idx].itemWeight;
                        // update the flags
                        pickUp_dropOff_flags[result_deliv_idx].second = true;
                    }
                    // update the truck position
                    truck_inter_idx    = result_inter_idx;
                    truck_inter_latlon = result_inter_latlon;
                    found = true;
                }

            }
        }while (result_inter_idx != -1);

        // find a closest depot from delivery
        found = false;
        auto check_all_paths = all_paths_deliv_deliv.find(truck_inter_idx)->second;
        for (auto itr = check_all_paths.begin(); itr != check_all_paths.end() && !found; ++itr){
            // check legality: is not a delivery spot (== is a depot spot)
            bool is_delivery_spot = std::get<4>(itr -> second);
            if (!is_delivery_spot){
                result_path       = std::get<0>(itr -> second);
                result_inter_idx  = std::get<1>(itr -> second);
                // update the final path
                CourierSubpath subpath;
                subpath.start_intersection = truck_inter_idx;
                subpath.end_intersection   = result_inter_idx;
                subpath.subpath            = result_path;
                final_path.push_back(subpath);
                // update bool
                found = true;
            }
        }   
    
        // return the path
        double temp_time =calculate_time_for_paths(final_path, turn_penalty);
        if (best_time > temp_time) {
            best_time =temp_time;
            best_path= final_path;
        }
    }
    return final_path;
}
    
double calculate_time_for_paths(std::vector<CourierSubpath> final_path, const float turn_penalty) {
        double totaltime = 0.0;
        for (auto it = final_path.begin(); it != final_path.end(); ++it) {
            totaltime+= compute_path_travel_time((it->subpath), turn_penalty);
        }
        return totaltime;
    }