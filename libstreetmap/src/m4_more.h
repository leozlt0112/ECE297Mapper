/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m4_more.h
 * Author: wangbox2
 *
 * Created on March 27, 2020, 11:40 PM
 */

#pragma once
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "structs_and_classes.h"
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m4.h"
#include "m3_more.h"

std::vector<CourierSubpath> traveling_courier_a(
		            const std::vector<DeliveryInfo>& deliveries,
	       	        const std::vector<int>& depots, 
		            const float turn_penalty, 
		            const float truck_capacity);
/***
            // update the final path
            CourierSubpath subpath;
            subpath.start_intersection = truck_inter_idx;
            subpath.end_intersection   = result_inter_idx;
            subpath.subpath = result_path;
            subpath.pickUp_indices = start_intersection_pickups;
            start_intersection_pickups.clear();
            final_path.push_back(subpath);
            // update start_intersection_pickups
            start_intersection_pickups.push_back(std::get<2>(itr -> second));
            // update the truck load
            truck_deliv_idxs.insert(result_deliv_idx);
            truck_deliv_weight += deliveries[result_deliv_idx].itemWeight;
            // update the flags
            pickUp_dropOff_flags[result_deliv_idx].first = true;
            // update the truck position
            truck_inter_idx = result_inter_idx;
            truck_inter_latlon = getIntersectionPosition(truck_inter_idx);
 
 
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
 
 ***/