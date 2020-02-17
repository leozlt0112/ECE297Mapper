/*
 * 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m2.h"
#include "m2_more.h"

void draw_map () {
    //draw_map_blank_canvas();
    intersections.resize(getNumIntersections());
    for(int i=0; i<getNumIntersections(); i++ ) {
        intersections[i].position=getIntersectionPosition(i);
        intersections[i].name= getIntersectionName(i);
    }
    draw_map_blank_canvas();
    return;
}

void draw_main_canvas (ezgl::renderer *g){    
    //double max_lat = getIntersectionPosition(0).lat();
//double min_lat = max_lat;
    g->draw_rectangle({0, 0}, {1000, 1000});

    for (size_t i = 0; i < intersections.size(); ++i) {
        float x = intersections[i].position.lon();
        float y = intersections[i].position.lat();

        float width = 10;
        float height = width;

        g->fill_rectangle({x, y}, {x + width, y + height});
    }
}

void draw_map_blank_canvas() {
  ezgl::application::settings settings; 
  settings.main_ui_resource = 
           "libstreetmap/resources/main.ui"; 
  settings.window_identifier = "MainWindow"; 
  settings.canvas_identifier = "MainCanvas";

  ezgl::application application(settings); 

  ezgl::rectangle initial_world({0, 0}, 
                                {1000,1000});
  application.add_canvas("MainCanvas", 
                         draw_main_canvas,
                         initial_world);

  application.run(nullptr, nullptr,
                  nullptr, nullptr);
}