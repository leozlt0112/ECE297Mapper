/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m2.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

void draw_map () {
  ezgl::application::settings settings; 
  settings.main_ui_resource = 
           "libstreetmap/resources/main.ui"; 
  settings.window_identifier = "MainWindow"; 
  settings.canvas_identifier = "MainCanvas";

  ezgl::application application(settings); 



  application.run(nullptr, nullptr,
                  nullptr, nullptr);
}