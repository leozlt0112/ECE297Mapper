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
std::vector<CourierSubpath> traveling_courier_b(
		            const std::vector<DeliveryInfo>& deliveries,
                            const std::vector<int>& depots, 
		            const float turn_penalty, 
		            const float truck_capacity);
double calculate_time_for_paths(std::vector<CourierSubpath> final_path, const float turn_penalty);