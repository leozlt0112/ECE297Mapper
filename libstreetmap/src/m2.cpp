/*
 * 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m2.h"
#include "m2_more.h"

void draw_map () {
    //Loading intersection. Finding min_latlon, max_latlon;
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
    draw_map_blank_canvas();
    return;
}

void draw_main_canvas (ezgl::renderer *g){    
    g->draw_rectangle({0, 0}, {1000, 1000});

    for (size_t i = 0; i < intersections.size(); ++i) {
        float x = intersections[i].position.lon();
        float y = intersections[i].position.lat();

        float width = 0.001;
        float height = width;

        g->fill_rectangle({x-width/2, y-height/2}, {x+width/2, y+height/2});
    }
}

void draw_map_blank_canvas() {
  ezgl::application::settings settings; 
  settings.main_ui_resource = 
           "libstreetmap/resources/main.ui"; 
  settings.window_identifier = "MainWindow"; 
  settings.canvas_identifier = "MainCanvas";

  ezgl::application application(settings); 

  ezgl::rectangle initial_world({min_lon, min_lat},
                                {max_lon, max_lat});
  application.add_canvas("MainCanvas", 
                         draw_main_canvas,
                         initial_world);

  application.run(nullptr, nullptr,
                  nullptr, nullptr);
}