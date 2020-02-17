/*
 * 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m2.h"
#include "m2_more.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

void draw_map () {
    draw_map_blank_canvas();
    return;
}

void draw_main_canvas (ezgl::renderer *g){   
  g->draw_rectangle({0, 0},
                    {1000, 1000});
};

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