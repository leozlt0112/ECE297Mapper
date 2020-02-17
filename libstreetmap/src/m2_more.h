/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m2_more.h
 * Author: wangbox2
 *
 * Created on February 17, 2020, 2:22 PM
 */

#pragma once
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "math.h"
#include <map>
#include <algorithm>    // std::sort
#include "OSMDatabaseAPI.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

struct intersection_data{
    LatLon position; 
    std::string name; 
};

std::vector<intersection_data> intersections;

double max_lat, min_lat, max_lon, min_lon;

void draw_main_canvas (ezgl::renderer *g);

void draw_map_blank_canvas();

