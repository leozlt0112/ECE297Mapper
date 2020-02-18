/*
 * 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m2.h"
#include "m2_more.h"

// draw map loads necessary variables and calls draw_map_blank_canvas() in the end.
void draw_map () {
    //Loading intersections[]. Finding min_latlon, max_latlon;
    max_lat = getIntersectionPosition(0).lat(); 
    min_lat = max_lat;
    max_lon = getIntersectionPosition(0).lon();
    min_lon = max_lon;
    
    intersections.resize(getNumIntersections());
    for(int i=0; i<intersections.size(); i++ ) {
        intersections[i].position=getIntersectionPosition(i);
        intersections[i].name= getIntersectionName(i);
        max_lat = std::max(max_lat, intersections[i].position.lat());
        min_lat = std::min(min_lat, intersections[i].position.lat());
        max_lon = std::max(max_lon, intersections[i].position.lon());
        min_lon = std::min(min_lon, intersections[i].position.lon());
    }
    avg_lat=(max_lat+min_lat)/2.0 * DEGREE_TO_RADIAN;
    
    draw_map_blank_canvas();
    return;
}

// draws the canvas and all intersections
void draw_main_canvas (ezgl::renderer *g){    
    g->draw_rectangle({0, 0}, {1000, 1000});

    for (size_t i = 0; i < intersections.size(); ++i) {
        float x = x_from_lon(intersections[i].position.lon());
        float y = y_from_lat(intersections[i].position.lat());

        float width = 100;
        float height = width;
        if (intersections[i].highlight) {
            g->set_color(ezgl::RED);
        } 
        else {
            g->set_color(ezgl::GREY_55);
        }
        g->fill_rectangle({x-width/2, y-height/2}, {x+width/2, y+height/2});
    }
   
}

// draw(initialize) the canvas
void draw_map_blank_canvas() {
    ezgl::application::settings settings; 
    settings.main_ui_resource = 
               "libstreetmap/resources/main.ui"; 
    settings.window_identifier = "MainWindow"; 
    settings.canvas_identifier = "MainCanvas";

    ezgl::application application(settings); 

    ezgl::rectangle initial_world({x_from_lon(min_lon), y_from_lat(min_lat)},
                                  {x_from_lon(max_lon), y_from_lat(max_lat)});
    application.add_canvas("MainCanvas", 
                           draw_main_canvas,
                           initial_world);
    application.run(nullptr, act_on_mouse_click,
                    nullptr, nullptr);
}

// uses global variable avg_lat (in radians)
// uses parameter lon (in degrees)
void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y) {
    std::cout << "Mouse clicked at (" << x << "," << y << ")\n";
    LatLon pos = LatLon(lat_from_y(y), lon_from_x(x));
    int id = find_closest_intersection(pos);
    std::cout<< "Closest Intersection: "<< intersections[id].name << "\n";
    intersections[id].highlight =true;
    app->refresh_drawing();
}

float lat_from_y(double y) {
    float lat = y / DEGREE_TO_RADIAN / EARTH_RADIUS_METERS;
    return lat; 
}
float lon_from_x(double x) {
    float lon = x / EARTH_RADIUS_METERS / std::cos(avg_lat) / DEGREE_TO_RADIAN; 
    return lon; 
}

float x_from_lon(float lon) {
    float x = lon * DEGREE_TO_RADIAN * std::cos(avg_lat) * EARTH_RADIUS_METERS;
    return x;
}

// uses parameter lat (in degrees)
float y_from_lat(float lat) {
    float y = lat * DEGREE_TO_RADIAN * EARTH_RADIUS_METERS;
    return y;
}