/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m2.h"
#include "m1.h"
#include "m1_more.h"
#include "StreetsDatabaseAPI.h"
#include "math.h"
#include <map>
#include <algorithm>    // std::sort
#include "OSMDatabaseAPI.h"

// Draws the currently loaded map.
// draw_map () assumes the map has already been loaded with load_map ()
// before it is called. The ece297exercise unit tests always call 
// load_map (string map_name) before calling draw_map. Your main () program
// should follow the same convention.
void draw_map();