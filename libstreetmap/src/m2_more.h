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
#include <vector>
#include <algorithm>    // std::sort

struct intersection_info{
    float x_;
    float y_;
    std::string name;
    bool highlight = false;
};
 
struct segment_info {
    bool oneWay;                                // if true, then can only travel in from->to direction
    int major_minor=0;                          // determine how major it is. (0=minor, 1=medium, 2=major)
    std::vector<ezgl::point2d> allPoints;       // from, curve points, to
    float speedLimit;                           // in km/h
    OSMID wayOSMID;                             // index of way this segment belongs to
};

struct feature_info {
    bool closed = false;                        // if true, it's closed (poly))
    std::string name;                           // feature name
    FeatureType type;                           // feature type
    std::vector<ezgl::point2d> allPoints;       // from, curve points, to
    float area;                                 // area
};

struct poi_info {
    float x_;                                   // position x
    float y_;                                   // position y
    std::string type;                           // poi type
    std::string name;                           // poi name
};

// a vector[intersection_idx] storing intersection data
std::vector<intersection_info> intersections;

// a vector[streetSegment_idx] storing street_segment_data
std::vector<segment_info> streetSegments;

// a vector[features_idx] storing features_data
std::vector<feature_info> features;

// a vector[POI_idx] storing poi_data
std::vector<poi_info> POIs;

// a vector[way_index] storing all the tags
std::unordered_map<OSMID, std::unordered_map<std::string,std::string>> WayID_tags;

// a vector[streetSegIndex], each element stores distance. from m1_more.h
extern std::vector<double> streetSeg_length;

// a vector[StreetIndex], each street vector stores streetSegmentIDs
extern std::vector<std::vector<int>> street_street_segments;

// a vector[streetIndex], each element stores distance
extern std::vector<double> streetID_streetLength;

// a variable used to store all possible all features
extern std::vector<std::vector<LatLon>> featureID_featurePts;

// In Degrees: max_lat, min_lat, max_lon, min_lon
// In Radians: avg_lat
double max_lat, min_lat, max_lon, min_lon, avg_lat;

// it stores the value of initial world size
ezgl::rectangle initial_world;

//prevent zooming out of bound
void out_of_bound_prevention(ezgl::renderer *g);

// load all the data
void draw_map_load ();

// draw main canvas and related features
void draw_main_canvas (ezgl::renderer *g);

// draw all intersections
void draw_intersections (ezgl::renderer *g);

// draw all street segments 
void draw_all_street_segments(ezgl::renderer *g);

// draw all the features
void draw_features(ezgl::renderer *g); 

extern int find_closest_POI(LatLon my_position);

// Converting latlon in degrees to x and y
float x_from_lon(float lon);
float y_from_lat(float lat);

// Converting x and y to latlon in degrees
float lat_from_y(double y);
float lon_from_x(double x);

// stores bunch of variables 
struct action_mem{
    // store intersections u clickded
    int last_clicked_intersection=-1;
    int last_clicked_POI=-1;
    ezgl::rectangle last_visible_world;
};

action_mem memory;

// initial setup 
void initial_setup(ezgl::application *application, bool new_window);

// events triggered by mouse clicks
void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y);