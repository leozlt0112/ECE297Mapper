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
    //memory.initial_world_width
    max_lat = getIntersectionPosition(0).lat(); 
    min_lat = max_lat;
    max_lon = getIntersectionPosition(0).lon();
    min_lon = max_lon;
    for(int i=0; i<intersections.size(); i++ ) {
        LatLon this_position  = getIntersectionPosition(i);
        max_lat = std::max(max_lat, this_position.lat());
        min_lat = std::min(min_lat, this_position.lat());
        max_lon = std::max(max_lon, this_position.lon());
        min_lon = std::min(min_lon, this_position.lon());
    }
    avg_lat=(max_lat+min_lat)/2.0 * DEGREE_TO_RADIAN;
    // stores initailly unzoomed width of the world
    memory.initial_world_width = x_from_lon(max_lon) - x_from_lon(min_lon);
    
    //std::vector<InfoStreetSegment> streetSegments;
    for(int i=0; i<intersections.size(); i++ ) {
        LatLon this_position  = getIntersectionPosition(i);
        intersections[i].x_   = x_from_lon(this_position.lon());
        intersections[i].y_   = y_from_lat(this_position.lat());
        intersections[i].name = getIntersectionName(i);
    }
    
    //std::vector<segment_info> streetSegments;
    for(size_t i = 0; i < streetSegments.size(); ++i) {
        InfoStreetSegment this_segment_info = getInfoStreetSegment(i);
        streetSegments[i].oneWay                = this_segment_info.oneWay;
        streetSegments[i].speedLimit            = this_segment_info.speedLimit;
        //store XY_ of "from" in allPoints
        streetSegments[i].allPoints.push_back(std::make_pair(   intersections[this_segment_info.from].x_,
                                                                intersections[this_segment_info.from].y_));
        //store XY_ of "curve points" in allPoints
        for (int curvePnt = 0; curvePnt < this_segment_info.curvePointCount; ++curvePnt){
            LatLon this_curvePnt = getStreetSegmentCurvePoint(curvePnt,i);
            streetSegments[i].allPoints.push_back(std::make_pair(   x_from_lon(this_curvePnt.lon()),
                                                                    y_from_lat(this_curvePnt.lat())));
        }
        //store XY_ of "to" in allPoints
        streetSegments[i].allPoints.push_back(std::make_pair(   intersections[this_segment_info.to].x_,
                                                                intersections[this_segment_info.to].y_));
    }
}

// draws main canvas and all relevant features
// call all the draw functions
void draw_main_canvas (ezgl::renderer *g){ 
    draw_intersections(g);
    draw_all_street_segments(g);
}

//draws all intersections
void draw_intersections (ezgl::renderer *g){
    float width = 5;
    float height = width;
    for (size_t i = 0; i < intersections.size(); ++i) {
        float x = intersections[i].x_;
        float y = intersections[i].y_;
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
    g->set_line_cap(ezgl::line_cap::round); // round ends
    g->set_line_dash(ezgl::line_dash::none); // Solid line
    int visible_width = g->get_visible_world().width();
    int iniWidth = memory.initial_world_width;
    std::cout<<g->get_visible_world().width()/iniWidth<<std::endl;
    for(size_t i = 0; i < streetSegments.size(); ++i) {
         //get coordinate of two points 
        segment_info this_segment = streetSegments[i];
        float x_from = this_segment.allPoints[0]    .first;
        float y_from = this_segment.allPoints[0]    .second;
        float x_to   = this_segment.allPoints.back().first;
        float y_to   = this_segment.allPoints.back().second;
        //draw all street segments with curve!
        //for (int curvePnt=0; curvePnt<this_segment.allPoints.size(); ++curvePnt){}
        //draw the streetsegments according to zoom
        // determine how much is zoomed in using if conditions
        if (visible_width > 0.75 * iniWidth){
            if (this_segment.speedLimit > 50){
                g->set_line_width(1);
                g->set_color(0, 0, 0, 255);
                g->draw_line({x_from, y_from}, {x_to, y_to});
            }
        }
        else if (visible_width > 0.3 * iniWidth){
            if (this_segment.speedLimit > 50){
                g->set_line_width(1);
                g->set_color(0, 0, 0, 255);
                g->draw_line({x_from, y_from}, {x_to, y_to});
            }
            else if (this_segment.speedLimit > 40){
                g->set_line_width(0.5);
                g->set_color(200, 200, 200, 255);
                g->draw_line({x_from, y_from}, {x_to, y_to});
            }
        }
        else if (visible_width > 0.02 * iniWidth){
            if (this_segment.speedLimit > 50){
                g->set_line_width(1);
                g->set_color(0, 0, 0, 255);
                g->draw_line({x_from, y_from}, {x_to, y_to});
            }
            else if (this_segment.speedLimit > 40){
                g->set_line_width(0.5);
                g->set_color(100, 100, 100, 255);
                g->draw_line({x_from, y_from}, {x_to, y_to});
            }
            else if ( visible_width < 5000){
                g->set_line_width(0.1);
                g->set_color(200, 200, 200, 255);
                g->draw_line({x_from, y_from}, {x_to, y_to});
            }
        }
        else{
            if (this_segment.speedLimit > 50){
                g->set_line_width(1);
                g->set_color(0, 0, 0, 255);
                g->draw_line({x_from, y_from}, {x_to, y_to});
            }
            else if (this_segment.speedLimit > 40){
                g->set_line_width(0.5);
                g->set_color(100, 100, 100, 255);
                g->draw_line({x_from, y_from}, {x_to, y_to});
            }
            else if ( visible_width < 5000){
                g->set_line_width(0.1);
                g->set_color(100, 100, 100, 255);
                g->draw_line({x_from, y_from}, {x_to, y_to});
            }
        }
    }
}

void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y) {
    std::cout << "Mouse clicked at (" << x << "," << y << ")\n";
    LatLon pos = LatLon(lat_from_y(y), lon_from_x(x));
    int id = find_closest_intersection(pos);
    std::cout<< "Closest Intersection: "<< intersections[id].name << "\n";
    //un-highlight the last clicked intersection
    
    if (memory.last_clicked_intersection != -1) 
        intersections[memory.last_clicked_intersection].highlight = false;
    intersections[id].highlight       = true;
    memory.last_clicked_intersection  = id;
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