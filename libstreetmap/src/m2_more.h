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
#include "OSMDatabaseAPI.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "math.h"
#include <map>
#include <algorithm>    // std::sort

struct intersection_data{
    double lat;
    double lon;
    std::string name;
    bool highlight = false;
    
    
};

struct street_segment_data{
    int from;   
    int to;
};

// a vector[intersection_id] storing intersection data
std::vector<intersection_data> intersections;

//a vector[streetSegment_id] storing street_segment_data
std::vector<street_segment_data> streetSegments;

// In Degrees: max_lat, min_lat, max_lon, min_lon
// In Radians: avg_lat
double max_lat, min_lat, max_lon, min_lon, avg_lat;

void draw_main_canvas (ezgl::renderer *g);

void draw_map_load ();

void draw_map_blank_canvas();

void draw_intersections (ezgl::renderer *g);

//draw all street segments 
void draw_all_street_segments(ezgl::renderer *g);

// Converting latlon in degrees to x and y
float x_from_lon(float lon);
float y_from_lat(float lat);
// Converting x and y to latlon in degrees
float lat_from_y(double y);
float lon_from_x(double x);
void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y);