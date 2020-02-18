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
    bool highlight = false;
    
};

// a vector[intersection_id] storing intersection data
std::vector<intersection_data> intersections;

// In Degrees: max_lat, min_lat, max_lon, min_lon
// In Radians: avg_lat
double max_lat, min_lat, max_lon, min_lon, avg_lat;

void draw_main_canvas (ezgl::renderer *g);

void draw_map_blank_canvas();

// Converting latlon in degrees to x and y
float x_from_lon(float lon);
float y_from_lat(float lat);
// Converting x and y to latlon in degrees
float lat_from_y(double y);
float lon_from_x(double x);
void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y);