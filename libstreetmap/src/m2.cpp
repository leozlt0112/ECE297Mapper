#include "m2.h"
#include "m2_more.h"
        
// draw map loads necessary variables and calls draw_map_blank_canvas() in the end.
void draw_map () {
    draw_map_load();
    
    ezgl::application::settings settings; 
    settings.main_ui_resource =  "libstreetmap/resources/main.ui"; 
    settings.window_identifier = "MainWindow"; 
    settings.canvas_identifier = "MainCanvas";

    ezgl::application application(settings); 

    application.add_canvas("MainCanvas", 
                           draw_main_canvas,
                           initial_world);
    application.run(initial_setup, act_on_mouse_click,
                    nullptr, nullptr);
    return;
}

void draw_map_load (){
    intersections.resize(getNumIntersections());
    streetSegments.resize(getNumStreetSegments());
    features.resize(getNumFeatures());
    
    // double max_lat, min_lat, max_lon, min_lon, avg_lat;
    // memory.initial_world_width
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
    
    // ezgl::rectangle initial_world;
    // memory.last_visible_world
    ezgl::rectangle rhs({x_from_lon(min_lon), y_from_lat(min_lat)},
                        {x_from_lon(max_lon), y_from_lat(max_lat)});
    initial_world = rhs;
    memory.last_visible_world = rhs;
    
    // std::vector<intersection_info> intersections;
    for(int i=0; i<intersections.size(); i++ ) {
        LatLon this_position  = getIntersectionPosition(i);
        intersections[i].x_   = x_from_lon(this_position.lon());
        intersections[i].y_   = y_from_lat(this_position.lat());
        intersections[i].name = getIntersectionName(i);
    }
    
    // std::vector<segment_info> streetSegments;
    for(size_t i = 0; i < streetSegments.size(); ++i) {
        InfoStreetSegment this_segment_info = getInfoStreetSegment(i);
        // store oneWay
        streetSegments[i].oneWay                = this_segment_info.oneWay;
        // store speedLimit
        streetSegments[i].speedLimit            = this_segment_info.speedLimit;
        // store XY_ of "from" in allPoints
        streetSegments[i].allPoints.push_back({ intersections[this_segment_info.from].x_,
                                                intersections[this_segment_info.from].y_});
        // store XY_ of "curve points" in allPoints
        for (int curvePnt = 0; curvePnt < this_segment_info.curvePointCount; ++curvePnt){
            LatLon this_curvePnt = getStreetSegmentCurvePoint(curvePnt,i);
            streetSegments[i].allPoints.push_back({ x_from_lon(this_curvePnt.lon()),
                                                    y_from_lat(this_curvePnt.lat())});
        }
        // store XY_ of "to" in allPoints
        streetSegments[i].allPoints.push_back({ intersections[this_segment_info.to].x_,
                                                intersections[this_segment_info.to].y_});
        // store major_minor
        float   initial_width = initial_world.width();
        if      (streetID_streetLength[this_segment_info.streetID]>initial_width/50 && this_segment_info.speedLimit > 50)   { streetSegments[i].major_minor = 2; }
        else if (streetID_streetLength[this_segment_info.streetID]>initial_width/20)                                        { streetSegments[i].major_minor = 1; }
        else                                                                                                                { streetSegments[i].major_minor = 0; }
    }
    
    // load features accordingly based on if they are closed or open
    for (int featureid=0; featureid<featureID_featurePts.size(); featureid++) {
         int totalPtsCount = getFeaturePointCount(featureid); 
         // set all points
         for(int i=0; i<totalPtsCount; i++) {
            float xcoords =  x_from_lon((float)(featureID_featurePts[featureid][i].lon()));
            float ycoords =  y_from_lat((float)(featureID_featurePts[featureid][i].lat()));
            features[featureid].allPoints.push_back({xcoords,ycoords});
         }
         // set the closed bool
         LatLon latLonPoint0    = featureID_featurePts[featureid][0];
         LatLon latLonPointLast = featureID_featurePts[featureid][totalPtsCount-1];
         if((latLonPoint0.lat()==latLonPointLast.lat()) && 
            (latLonPoint0.lon()==latLonPointLast.lon()) && 
            totalPtsCount>1)
            features[featureid].closed = true;
         // set type
         features[featureid].type = getFeatureType(featureid);
         // set name
         features[featureid].name = getFeatureName(featureid);
    }
}

// draws main canvas and all relevant features
// call all the draw functions
void draw_main_canvas (ezgl::renderer *g){
    out_of_bound_prevention(g); 
    draw_features(g); 
    draw_intersections(g); 
    draw_all_street_segments(g); 
}

void out_of_bound_prevention(ezgl::renderer *g) {
    
    ezgl::rectangle current_visible_world = g->get_visible_world();
    if (current_visible_world.width() > initial_world.width()){
        g -> set_visible_world(initial_world);
        memory.last_visible_world = initial_world;
    }/*
    else if (current_visible_world.right()  > initial_world.right() ||
             current_visible_world.left()   < initial_world.left()  ||
             current_visible_world.top()    > initial_world.top()   ||
             current_visible_world.bottom() < initial_world.bottom()){
        g -> set_visible_world(memory.last_visible_world);
    }
    else{
        memory.last_visible_world = current_visible_world;
    }*/
    return;
}

//draws all intersections
void draw_intersections (ezgl::renderer *g){
    int visible_width = g->get_visible_world().width();
    if (visible_width >2000) return;
    float radius = 1;
    for (size_t i = 0; i < intersections.size(); ++i) {
        float x = intersections[i].x_;
        float y = intersections[i].y_;
        if (intersections[i].highlight) {
            g->set_color(ezgl::RED);
        } 
        else {
            g->set_color(ezgl::GREY_55);
        }
        g->fill_arc({x, y}, radius, 0, 360);
    }
}

//draw all street segments. solid lines of width 3, with round ends, opaque
void draw_all_street_segments(ezgl::renderer *g){
    //draw all the lines 
    g->set_line_cap(ezgl::line_cap::round); // round ends
    g->set_line_dash(ezgl::line_dash::none); // Solid line
    float visible_width = g->get_visible_world().width();
    float initial_width = initial_world.width();
    std::cout<<visible_width/initial_width<<std::endl;
    for(size_t i = 0; i < streetSegments.size(); ++i) {
        segment_info   this_segment = streetSegments[i];
        for (int pnt = 0; pnt < (this_segment.allPoints.size()-1); ++pnt){
            //get coordinate of two points 
            ezgl::point2d from = this_segment.allPoints[pnt  ];
            ezgl::point2d to   = this_segment.allPoints[pnt+1];
            // draw the street segments according to zoom
            // determine how much is zoomed in using if conditions
            // 1, 0.6, 0.36
            if (visible_width > 0.3 * initial_width){
                if (this_segment.major_minor==2){
                    g->set_line_width(1);
                    g->set_color(0, 0, 0, 255);
                    g->draw_line(from, to);
                }
            }
            // 0.22, 0.12
            else if (visible_width > 0.1 * initial_width){
                if (this_segment.major_minor==2){
                    g->set_line_width(1);
                    g->set_color(0, 0, 0, 255);
                    g->draw_line(from, to);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(0.5);
                    g->set_color(200, 200, 200, 255);
                    g->draw_line(from, to);
                }
            }
            // 0.07
            else if (visible_width > 0.05 * initial_width){
                if (this_segment.major_minor==2){
                    g->set_line_width(1);
                    g->set_color(0, 0, 0, 255);
                    g->draw_line(from, to);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(0.5);
                    g->set_color(100, 100, 100, 255);
                    g->draw_line(from, to);
                }
            }
            // 0.046, 0.027
            else if (visible_width > 0.02 * initial_width){
                if (this_segment.major_minor==2){
                    g->set_line_width(2);
                    g->set_color(0, 0, 0, 255);
                    g->draw_line(from, to);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(1);
                    g->set_color(100, 100, 100, 255);
                    g->draw_line(from, to);
                }
                else if (this_segment.major_minor==0){
                    g->set_line_width(0.5);
                    g->set_color(200, 200, 200, 255);
                    g->draw_line(from, to);
                }
            }
            // 0.016, 0.010
            else{
                if (this_segment.major_minor==2){
                    g->set_line_width(2);
                    g->set_color(0, 0, 0, 255);
                    g->draw_line(from, to);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(1);
                    g->set_color(100, 100, 100, 255);
                    g->draw_line(from, to);
                }
                else if (this_segment.major_minor==0){
                    g->set_line_width(0.5);
                    g->set_color(100, 100, 100, 255);
                    g->draw_line(from, to);
                }
            }
        }
    }
}

void initial_setup(ezgl::application *application, bool new_window){
    return;
}

// Leo draw features
void draw_features(ezgl::renderer *g) {
    float visible_width = g->get_visible_world().width();
    float initial_width = initial_world.width();
    for (int feature_id=0; feature_id<features.size(); ++feature_id) {
        feature_info this_feature = features[feature_id];
        // 0.5 - 
        if (visible_width > 0.5 * initial_width){
            if (this_feature.closed){
                if (this_feature.type==Lake) {
                    g->set_color(174, 234, 250, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                if (this_feature.type==Park) {
                    g->set_color(174, 225, 195, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                    
            }
        }
        else if (visible_width > 0.4 * initial_width) {
            if (this_feature.closed) {
                if (this_feature.type==Beach) {
                    g->set_color(168, 160, 50, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                if (this_feature.type==Lake) {
                    g->set_color(174, 234, 250, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                if (this_feature.type==Park) {
                    g->set_color(174, 225, 195, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                
            }
        }
        // 0.1 - 0.5
        else if (visible_width > 0.1 * initial_width){
            if (this_feature.closed){
               if (this_feature.type==Beach) {
                    g->set_color(168, 160, 50, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                if (this_feature.type==Lake) {
                    g->set_color(174, 234, 250, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                if (this_feature.type==Park) {
                    g->set_color(174, 225, 195, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==River) {
                    g->set_color(51, 200, 245, 255);
                    g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Golfcourse) {
                    g->set_color(11, 214, 89, 255);
                    g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Golfcourse) {
                   g->set_color(11, 214, 89, 255);
                   g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Island) {
                    g->set_color(148, 124, 77, 255);
                    g->fill_poly(this_feature.allPoints);
                }
            }
        }
        // 0.05 - 0.1
        else if (visible_width > 0.05 * initial_width){
             if (this_feature.closed){
               if (this_feature.type==Beach) {
                    g->set_color(168, 160, 50, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                if (this_feature.type==Lake) {
                    g->set_color(174, 234, 250, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                if (this_feature.type==Park) {
                    g->set_color(174, 225, 195, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==River) {
                    g->set_color(51, 200, 245, 255);
                    g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Golfcourse) {
                    g->set_color(11, 214, 89, 255);
                    g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Golfcourse) {
                   g->set_color(11, 214, 89, 255);
                   g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Island) {
                    g->set_color(148, 124, 77, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==Greenspace) {
                    g->set_color(7, 1247, 55, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==Stream) {
                    g->set_color(21, 167, 247, 255);
                    g->fill_poly(this_feature.allPoints);
                }
            }
        }
        else if (visible_width > 0.02 * initial_width){
             if (this_feature.closed){
               if (this_feature.type==Beach) {
                    g->set_color(168, 160, 50, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                if (this_feature.type==Lake) {
                    g->set_color(174, 234, 250, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                if (this_feature.type==Park) {
                    g->set_color(174, 225, 195, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==River) {
                    g->set_color(51, 200, 245, 255);
                    g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Golfcourse) {
                    g->set_color(11, 214, 89, 255);
                    g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Golfcourse) {
                   g->set_color(11, 214, 89, 255);
                   g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Island) {
                    g->set_color(148, 124, 77, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==Greenspace) {
                    g->set_color(7, 1247, 55, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==Stream) {
                    g->set_color(21, 167, 247, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==Building) {
                    g->set_color(181, 180, 177, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               
            }
        }
        else{
           if (this_feature.closed){
               if (this_feature.type==Beach) {
                    g->set_color(168, 160, 50, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                if (this_feature.type==Lake) {
                    g->set_color(174, 234, 250, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                if (this_feature.type==Park) {
                    g->set_color(174, 225, 195, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==River) {
                    g->set_color(51, 200, 245, 255);
                    g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Golfcourse) {
                    g->set_color(11, 214, 89, 255);
                    g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Golfcourse) {
                   g->set_color(11, 214, 89, 255);
                   g->fill_poly(this_feature.allPoints);
               }
               if (this_feature.type==Island) {
                    g->set_color(148, 124, 77, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==Greenspace) {
                    g->set_color(7, 1247, 55, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==Stream) {
                    g->set_color(21, 167, 247, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               if (this_feature.type==Building) {
                    g->set_color(181, 180, 177, 255);
                    g->fill_poly(this_feature.allPoints);
                }
               
            }
        }
    }
}

void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y) {
    std::cout << "Mouse clicked at (" << x << "," << y << ")\n";
    LatLon pos = LatLon(lat_from_y(y), lon_from_x(x));
    //find closest intersection_id, -1 means too far away to any intersections
    int id = find_closest_intersection(pos);
    //un-highlight the last clicked intersection
    if (memory.last_clicked_intersection != -1) 
        intersections[memory.last_clicked_intersection].highlight = false;
    // set new value of last_clicked
    memory.last_clicked_intersection  = id;
    // highlight current clicked
    if (id != -1) {
        intersections[id].highlight = true;
        std::cout<< "Closest Intersection: "<< intersections[id].name << "\n";
    }
    app->refresh_drawing();
}

// uses global variable avg_lat (in radians)
// uses parameter lon (in degrees)
float lon_from_x(double x) {
    float lon = x / EARTH_RADIUS_METERS / std::cos(avg_lat) / DEGREE_TO_RADIAN; 
    return lon; 
}

// uses parameter lat (in degrees)
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