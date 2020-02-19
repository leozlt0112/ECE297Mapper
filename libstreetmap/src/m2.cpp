#include "m2.h"
#include "m2_more.h"
        
// draw map loads necessary variables and calls draw_map_blank_canvas() in the end.
void draw_map () {
    draw_map_load();
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
    return;
}

void draw_map_load (){
    intersections.resize(getNumIntersections());
    streetSegments.resize(getNumStreetSegments());
    
    //double max_lat, min_lat, max_lon, min_lon, avg_lat;
    //std::vector<intersection_data> intersections;
    //std::vector<street_segment_data> streetSegments;
    max_lat = getIntersectionPosition(0).lat(); 
    min_lat = max_lat;
    max_lon = getIntersectionPosition(0).lon();
    min_lon = max_lon;
    for(int i=0; i<intersections.size(); i++ ) {
        LatLon this_position  = getIntersectionPosition(i);
        intersections[i].lat  = this_position.lat();
        intersections[i].lon  = this_position.lon();
        intersections[i].name = getIntersectionName(i);
        max_lat = std::max(max_lat, intersections[i].lat);
        min_lat = std::min(min_lat, intersections[i].lat);
        max_lon = std::max(max_lon, intersections[i].lon);
        min_lon = std::min(min_lon, intersections[i].lon);
    }
    avg_lat=(max_lat+min_lat)/2.0 * DEGREE_TO_RADIAN;
    
    //std::vector<street_segment_data> streetSegments;
    for(size_t i = 0; i < streetSegments.size(); ++i) {
        InfoStreetSegment streetSeg = getInfoStreetSegment(i);
        streetSegments[i].from = streetSeg.from;
        streetSegments[i].to= streetSeg.to;
    }
}

// draws main canvas and all relevant features
void draw_main_canvas (ezgl::renderer *g){    
    g->draw_rectangle({0, 0}, {1000, 1000});
    draw_intersections(g);
    draw_all_street_segments(g);
}

//draws all intersections
void draw_intersections (ezgl::renderer *g){
    for (size_t i = 0; i < intersections.size(); ++i) {
        float x = x_from_lon(intersections[i].lon);
        float y = y_from_lat(intersections[i].lat);

        float width = 50;
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

//draw all street segments. solid lines of width 3, with butt ends, opaque
void draw_all_street_segments(ezgl::renderer *g){
    //draw all the lines 
    g->set_color(ezgl::BLACK);
    g->set_line_cap(ezgl::line_cap::butt); // Butt ends
    g->set_line_dash(ezgl::line_dash::none); // Solid line
    g->set_line_width(3);
    for(size_t i = 0; i < streetSegments.size(); ++i) {
         //get coordinate of two points 
        float x_from = x_from_lon(intersections[streetSegments[i].from].lon);
        float y_from = y_from_lat(intersections[streetSegments[i].from].lat);
        float x_to = x_from_lon(intersections[streetSegments[i].to].lon);
        float y_to = y_from_lat(intersections[streetSegments[i].to].lat);
        //draw the line from to to 
        g->draw_line({x_from, y_from}, {x_to, y_to});
    }
}

void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y) {
    std::cout << "Mouse clicked at (" << x << "," << y << ")\n";
    LatLon pos = LatLon(lat_from_y(y), lon_from_x(x));
    int id = find_closest_intersection(pos);
    std::cout<< "Closest Intersection: "<< intersections[id].name << "\n";
    intersections[id].highlight =true;
    app->refresh_drawing();
}

// uses global variable avg_lat (in radians)
// uses parameter lon (in degrees)
float lon_from_x(double x) {
    float lon = x / EARTH_RADIUS_METERS / std::cos(avg_lat) / DEGREE_TO_RADIAN; 
    return lon; 
}

// uses global variable avg_lat (in radians)
// uses parameter lon (in degrees)
float lat_from_y(double y) {
    float lat = y / DEGREE_TO_RADIAN / EARTH_RADIUS_METERS;
    return lat; 
}

// uses global variable avg_lat (in radians)
// uses parameter lon (in degrees)
float x_from_lon(float lon) {
    float x = lon * DEGREE_TO_RADIAN * std::cos(avg_lat) * EARTH_RADIUS_METERS;
    return x;
}

// uses parameter lat (in degrees)
float y_from_lat(float lat) {
    float y = lat * DEGREE_TO_RADIAN * EARTH_RADIUS_METERS;
    return y;
}