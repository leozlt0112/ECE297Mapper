#include "m2.h"
#include "m2_more.h"
#include "rectangle.hpp"
#include "graphics.hpp"
        
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
                           initial_world,
                           ezgl::BLACK);
    application.run(initial_setup, act_on_mouse_click,
                    nullptr, nullptr);
    return;
}

void draw_map_load (){
    intersections.resize(getNumIntersections());
    streetSegments.resize(getNumStreetSegments());
    features.resize(getNumFeatures());
    POIs.resize(getNumPointsOfInterest());
    
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
        LatLon this_position      = getIntersectionPosition(i);
        intersections[i].x_       = x_from_lon(this_position.lon());
        intersections[i].y_       = y_from_lat(this_position.lat());
        intersections[i].name     = getIntersectionName(i);
    }
    
    // std::unordered_map<OSMID, std::unordered_map<std::string,std::string>> WayID_tags;
    for (int way=0; way<getNumberOfWays(); ++way){
        const OSMWay* this_way = getWayByIndex(way);
        std::unordered_map<std::string,std::string> these_tags;
        for(int i=0;i<getTagCount(this_way); ++i)
        {
            these_tags.insert(getTagPair(this_way,i));
        }
        WayID_tags.insert(std::make_pair(this_way->id(), these_tags));
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
        std::unordered_map<std::string,std::string> these_tags = WayID_tags.find(this_segment_info.wayOSMID)->second;
        std::string highway_tag = these_tags.find("highway")->second;
        if      (highway_tag == "motorway"      || highway_tag == "trunk"           || 
                 highway_tag == "primary"       || highway_tag == "secondary")                                              { streetSegments[i].major_minor = 2; }
        else if (highway_tag == "motorway_link" || highway_tag == "trunk_link"      || 
                 highway_tag == "primary_link"  || highway_tag == "secondary_link"  ||
                 highway_tag == "tertiary"      || highway_tag == "tertiary_link")                                          { streetSegments[i].major_minor = 1; }
        else                                                                                                                { streetSegments[i].major_minor = 0; }
    }
    
    // std::vector<poi_info> POIs;
    for(int i=0; i< POIs.size(); i++ ) {
        LatLon this_position      = getPointOfInterestPosition(i);
        POIs[i].x_   = x_from_lon(this_position.lon());
        POIs[i].y_   = y_from_lat(this_position.lat());
        POIs[i].name = getPointOfInterestName(i);
        POIs[i].type = getPointOfInterestType(i);
    }
    
    // load features
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
         //
         features[featureid].area = find_feature_area(featureid);
    }
}

// draws main canvas and all relevant features
// call all the draw functions
void draw_main_canvas (ezgl::renderer *g){
    out_of_bound_prevention(g); 
    draw_features(g); 
    draw_all_street_segments(g); 
    draw_intersections(g); 
    draw_points_of_interests(g); 
    draw_street_names(g);
}

void out_of_bound_prevention(ezgl::renderer *g) {
    ezgl::rectangle current_visible_world = g->get_visible_world();
    if (current_visible_world.width() > initial_world.width() && current_visible_world.height() > initial_world.height()){
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
    if (visible_width > 2000) return;
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
            ezgl::point2d point1 = this_segment.allPoints[pnt  ];
            ezgl::point2d point2 = this_segment.allPoints[pnt+1];
            // draw the street segments according to zoom
            // determine how much is zoomed in using if conditions
            // 1, 0.6, 0.36
            if (visible_width > 0.3 * initial_width){
                if (this_segment.major_minor==2){
                    g->set_line_width(1);
                    g->set_color(255, 255, 255, 255);
                    g->draw_line(point1, point2);
                }
            }
            // 0.22
            else if (visible_width > 0.2 * initial_width){
                if (this_segment.major_minor==2){
                    g->set_line_width(1);
                    g->set_color(255, 255, 255, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(0.5);
                    g->set_color(50, 50, 50, 255);
                    g->draw_line(point1, point2);
                }
            }
            // 0.12
            else if (visible_width > 0.1 * initial_width){
                if (this_segment.major_minor==2){
                    g->set_line_width(1);
                    g->set_color(255, 255, 255, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(0.5);
                    g->set_color(100, 100, 100, 255);
                    g->draw_line(point1, point2);
                }
            }
            // 0.07
            else if (visible_width > 0.05 * initial_width){
                if (this_segment.major_minor==2){
                    g->set_line_width(1);
                    g->set_color(255, 255, 255, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(0.5);
                    g->set_color(155, 155, 155, 255);
                    g->draw_line(point1, point2);
                }
            }
            // 0.046
            else if (visible_width > 0.04 * initial_width){
                if (this_segment.major_minor==2){
                    g->set_line_width(2);
                    g->set_color(255, 255, 255, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(1);
                    g->set_color(150, 150, 150, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==0){
                    g->set_line_width(0.5);
                    g->set_color(50, 50, 50, 255);
                    g->draw_line(point1, point2);
                }
            }
            // 0.027
            else if (visible_width > 0.02 * initial_width){
                if (this_segment.major_minor==2){
                    g->set_line_width(5);
                    g->set_color(255, 255, 255, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(2);
                    g->set_color(200, 200, 200, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==0){
                    g->set_line_width(2);
                    g->set_color(100, 100, 100, 255);
                    g->draw_line(point1, point2);
                }
            }
            // 0.016, 0
            else{
                if (this_segment.major_minor==2){
                    g->set_line_width(10);
                    g->set_color(255, 255, 255, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(5);
                    g->set_color(200, 200, 200, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==0){
                    g->set_line_width(5);
                    g->set_color(150, 150, 150, 255);
                    g->draw_line(point1, point2);
                }
            }
        }
    }
}

// Leo draw features
void draw_features(ezgl::renderer *g) {
    g->set_line_cap(ezgl::line_cap::round); // round ends
    g->set_line_dash(ezgl::line_dash::none); 
    float visible_width = g->get_visible_world().width();
    float initial_width = initial_world.width();
    float visible_area = g->get_visible_world().area();
    for (int feature_id=0; feature_id<features.size(); ++feature_id) {
        feature_info this_feature = features[feature_id];
        // 1, 0.6
        if (visible_width > 0.5 * initial_width){
            if (this_feature.closed && this_feature.area > visible_area/50000){
                // light blue
                if (this_feature.type==Lake) {
                    g->set_color(100, 150, 200, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark green
                if (this_feature.type==Park) {
                    g->set_color(100, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // black
                if (this_feature.type==Island) {
                    g->set_color(ezgl::BLACK);
                    g->fill_poly(this_feature.allPoints);
                }
            }
        }
        // 0.6^2
        else if (visible_width > 0.3 * initial_width) {
            if (this_feature.closed && this_feature.area > visible_area/100000) {
                // dark blue
                if (this_feature.type==Lake) {
                    g->set_color(100, 150, 200, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark green
                if (this_feature.type==Park) {
                    g->set_color(100, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // black
                if (this_feature.type==Island) {
                    g->set_color(ezgl::BLACK);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark bluish green
                if (this_feature.type==Greenspace) {
                    g->set_color(75, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
            }
        }
        // 0.6^3, 0.6^4
        else if (visible_width > 0.1 * initial_width){
            if (this_feature.closed){
                // dark blue
                if (this_feature.type==Lake) {
                    g->set_color(100, 150, 200, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // reddish green
                if (this_feature.type==Golfcourse) {
                    g->set_color(120, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark green
                if (this_feature.type==Park) {
                    g->set_color(100, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // black
                if (this_feature.type==Island) {
                    g->set_color(ezgl::BLACK);
                    g->fill_poly(this_feature.allPoints);
                }
                // bluish green
                if (this_feature.type==Greenspace) {
                    g->set_color(75, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark blue
                if (this_feature.type==River) {
                    g->set_color(100, 150, 200, 255);
                    g->fill_poly(this_feature.allPoints);
                }
            }
        }
        // 0.6^5
        else if (visible_width > 0.05 * initial_width){
             if (this_feature.closed){
                // dark blue
                if (this_feature.type==Lake) {
                    g->set_color(100, 150, 200, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // reddish green
                if (this_feature.type==Golfcourse) {
                    g->set_color(120, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark green
                if (this_feature.type==Park) {
                    g->set_color(100, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // black
                if (this_feature.type==Island) {
                    g->set_color(ezgl::BLACK);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark bluish green
                if (this_feature.type==Greenspace) {
                    g->set_color(75, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark blue
                if (this_feature.type==River) {
                    g->set_color(100, 150, 200, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark gray
                if (this_feature.type==Building) {
                    g->set_color(50, 50, 50, 255);
                    g->fill_poly(this_feature.allPoints);
                }
            }
        }
        // 0.6^6, 0.6^7
        else if (visible_width > 0.02 * initial_width){
             if (this_feature.closed){
                // dark blue
                if (this_feature.type==Lake) {
                    g->set_color(100, 150, 200, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // reddish green
                if (this_feature.type==Golfcourse) {
                    g->set_color(120, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark green
                if (this_feature.type==Park) {
                    g->set_color(100, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // black
                if (this_feature.type==Island) {
                    g->set_color(ezgl::BLACK);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark bluish green
                if (this_feature.type==Greenspace) {
                    g->set_color(75, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark blue
                if (this_feature.type==River) {
                    g->set_color(100, 150, 200, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // lighter gray
                if (this_feature.type==Building) {
                    g->set_color(75, 75, 75, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // brown
                if (this_feature.type==Beach) {
                    g->set_color(125, 100, 75, 255);
                    g->fill_poly(this_feature.allPoints);
                }
            }
        }
        // 0.6^8, 0
        else{
            // closed
            if (this_feature.closed){
                // dark blue
                if (this_feature.type==Lake) {
                    g->set_color(100, 150, 200, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // reddish green
                if (this_feature.type==Golfcourse) {
                    g->set_color(120, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark green
                if (this_feature.type==Park) {
                    g->set_color(100, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // black
                if (this_feature.type==Island) {
                    g->set_color(ezgl::BLACK);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark bluish green
                if (this_feature.type==Greenspace) {
                    g->set_color(75, 130, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // dark blue
                if (this_feature.type==River) {
                    g->set_color(100, 150, 200, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // lighter gray
                if (this_feature.type==Building) {
                    g->set_color(100, 100, 100, 255);
                    g->fill_poly(this_feature.allPoints);
                }
                // brown
                if (this_feature.type==Beach) {
                    g->set_color(125, 100, 75, 255);
                    g->fill_poly(this_feature.allPoints);
                } 
            }
            // non-closed
            else{
                for(int pts=0; pts<this_feature.allPoints.size()-1; pts++) {
                    ezgl::point2d position1 = this_feature.allPoints[pts  ];
                    ezgl::point2d position2 = this_feature.allPoints[pts+1];
                    g->set_line_width(10);
                    g->set_color(100, 150, 200, 255);
                    g->draw_line(position1, position2);
                }
            }
        }
    }
}

void draw_points_of_interests(ezgl::renderer *g) {
    float visible_width = g->get_visible_world().width();
    float initial_width = initial_world.width();
    if (visible_width < 2000 * initial_width){
        for(int poi=0; poi<POIs.size(); poi++) {
            float x = POIs[poi].x_;
            float y = POIs[poi].y_;
            if (POIs[poi].highlight) {
                g->set_color(ezgl::MAGENTA);
                g->fill_arc({x, y}, 5, 0, 360);
            }
            else {
                 g->set_color(ezgl::RED);
                g->fill_arc({x, y}, 5, 0, 360);
            }
        }
    }
}

void draw_street_names(ezgl::renderer *g) {
    g->set_font_size(20);
    g->set_color(50, 100, 50, 255);
    float visible_width = g->get_visible_world().width();
    float initial_width = initial_world.width();
    if (visible_width < 0.02 * initial_width){
        for (int street=0; street < getNumStreets(); ++street){
            // find street name and its length
            std::string street_name = getStreetName(street);
            int name_size = street_name.size();
            // find the position and angle to draw the name
            std::vector<int> these_segments = street_street_segments[street];
            int this_segment_idx = these_segments[these_segments.size()/2];
            std::vector<ezgl::point2d> these_points = streetSegments[this_segment_idx].allPoints;
            ezgl::point2d point1 = these_points[these_points.size()/2 -1];
            ezgl::point2d point2 = these_points[these_points.size()/2   ];
            ezgl::point2d middle((point1.x + point2.x)*0.5, (point1.y + point2.y)*0.5);
            double angle = atan((point2.y-point1.y)/(point2.x-point1.x)) / DEGREE_TO_RADIAN;
            // draw text
            g->set_text_rotation(angle);
            double segment_length = streetSeg_length[this_segment_idx];
            g->draw_text(middle,street_name,std::min((double)(10*name_size),segment_length),10);
        }
    }
}

void initial_setup(ezgl::application *application, bool new_window){
    // Update the status bar message
    application->update_message("EZGL Application"); 
  
    // Create a Find button and link it with find_button callback.
    application->create_button("Find", 8, find_button);
}

void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y) {
    std::cout << "Mouse clicked at (" << x << "," << y << ")\n";
    LatLon pos = LatLon(lat_from_y(y), lon_from_x(x));
    // find closedPOIid, -1 means too far away to any poi
    int idForPOI=find_closest_POI(pos);
    // un-highlight the last intersection
    if(memory.last_clicked_POI !=-1) 
        POIs[memory.last_clicked_POI].highlight = false; 
    // set the value of last_clicked
    memory.last_clicked_POI = idForPOI;
    // highlight current clicked 
    if (idForPOI != -1) {
        POIs[idForPOI].highlight = true; 
    }
    
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
        std::string status_message = "Closest Intersection: " + intersections[id].name;
       app->update_message(status_message);
    }

    app->refresh_drawing();
}

//A callback function for Find button 
void find_button(GtkWidget */*widget*/, ezgl::application *application){
    
    // Update the status bar message
    application->update_message("Find Button Pressed");
    std::cout << "please enter both street names:" << std::endl;
    
    // get street1 entry
    GtkEntry* text_entry1 = (GtkEntry *) application->get_object("TextEntry1");
    std::string firstStreet(gtk_entry_get_text(text_entry1));
    std::cout<<"entry1:"<<firstStreet<<"\n";
    
    //get street ids from street names    
    std::vector<int> first_ids  = find_street_ids_from_partial_street_name(firstStreet);
    for (int i=0; i <first_ids.size(); ++i)
        std::cout << first_ids[i] << "  " << getStreetName(first_ids[i]) << std::endl;
    
    // get street2 entry
    GtkEntry* text_entry2 = (GtkEntry *) application->get_object("TextEntry2");
    std::string secondStreet(gtk_entry_get_text(text_entry2));
    std::cout<<"entry2:"<<secondStreet<<"\n";
    
    //get street ids from street names
    std::vector<int> second_ids = find_street_ids_from_partial_street_name(secondStreet);
    for (int i=0; i <second_ids.size(); ++i)
        std::cout << second_ids[i] << "  " << getStreetName(second_ids[i]) << std::endl;

    //the two streets are not the same 
    if(first_ids[0] != second_ids[0]){
    
        std::pair<int, int> street_ids;
        street_ids = std::make_pair(first_ids[0], second_ids[0]); 
    
        //find intersection ids for two intersecting streets
        std::vector<int> intersections_id;
        intersections_id = find_intersections_of_two_streets(street_ids);
        
        if (intersections_id.size()==0)
            std::cout<<"no intersections"<<std::endl;
        //highlight intersections 
        for(int i=0; i<intersections_id.size(); ++i){
            intersections[intersections_id[i]].highlight = true;
            //print intersection info
            std::cout << intersections_id[i] << "  " << intersections[intersections_id[i]].name <<std::endl;
        }
        
        application->refresh_drawing();
    }
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