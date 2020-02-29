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
                           initial_world,
                           ezgl::BLACK);
    application.run(initial_setup, act_on_mouse_click,
                    nullptr, act_on_key_press);
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
    
    // std::unordered_map<OSMID, OSMNode*> NodeID_node;
    for (int node=0; node<getNumberOfNodes(); ++node){
        const OSMNode* this_node = getNodeByIndex(node);
        NodeID_node.insert(std::make_pair(this_node->id(), this_node));
    }
    
    
    // std::vector<highway_info> highways_major/medium/minor;
    for (int way=0; way<getNumberOfWays(); ++way){
        // find all the tags of this way
        const OSMWay* this_way   = getWayByIndex(way);
        OSMID         this_wayID = this_way->id();
        std::unordered_map<std::string,std::string> these_tags = WayID_tags.find(this_wayID)->second;
        // determine if is street
        if (these_tags.find("highway") != these_tags.end()){
            // find all the points coordinates
            std::vector<OSMID>          all_nodes  = getWayMembers(this_way);
            std::vector<ezgl::point2d>  all_coords_xy;
            for (int i = 0; i < all_nodes.size(); ++i){
                const OSMNode* this_node = NodeID_node.find(all_nodes[i])->second;
                LatLon        coords_latlon = getNodeCoords(this_node);
                ezgl::point2d coords_xy(x_from_lon(coords_latlon.lon()),
                                        y_from_lat(coords_latlon.lat()));
                all_coords_xy.push_back(coords_xy);
            }
            // determine major_minor
            std::string highway_tag = these_tags.find("highway")->second;
            int     major_minor = 0;
            if      (highway_tag == "motorway"      || highway_tag == "trunk"           || 
                     highway_tag == "primary"       || highway_tag == "secondary")              { major_minor = 2; }
            else if (highway_tag == "motorway_link" || highway_tag == "trunk_link"      || 
                     highway_tag == "primary_link"  || highway_tag == "secondary_link"  ||
                     highway_tag == "tertiary"      || highway_tag == "tertiary_link")          { major_minor = 1; }
            else                                                                                { major_minor = 0; }
            // store all the data in one highway_info
            highway_info this_info;
            this_info.allPoints = all_coords_xy;
            this_info.tags      = these_tags;
            // insert the struct into the variable
            if      (major_minor==2)                                                            { highways_major.push_back(this_info); }
            else if (major_minor==1)                                                            { highways_medium.push_back(this_info); }
            else                                                                                { highways_minor.push_back(this_info); }
        }
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
    draw_all_highways(g);
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

// draw all the streets
void draw_all_highways(ezgl::renderer *g){
    //draw all the lines 
    g->set_line_cap(ezgl::line_cap::round); // round ends
    g->set_line_dash(ezgl::line_dash::none); // Solid line
    float visible_w = g->get_visible_world().width();
    float visible_h = g->get_visible_world().height();
    float initial_w = initial_world.width();
    float initial_h = initial_world.height();
    float zoom_factor = std::min(visible_w/initial_w, visible_h/initial_h);
    std::cout<<zoom_factor<<std::endl;
    // 1, 0.6, 0.36
    if ( zoom_factor > 0.3 ){
        // major highways
        g->set_line_width(1);
        g->set_color(255, 255, 255, 255);
        for(size_t i = 0; i < highways_major.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_major[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points 
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
    }
    // 0.22
    else if ( zoom_factor > 0.2 ){
        // medium highways    
        g->set_line_width(0.5);
        g->set_color(50, 50, 50, 255);
        for(size_t i = 0; i < highways_medium.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_medium[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
        // major highways
        g->set_line_width(1);
        g->set_color(255, 255, 255, 255);
        for(size_t i = 0; i < highways_major.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_major[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
    }
    // 0.12
    else if ( zoom_factor > 0.1 ){
        // medium highways 
        g->set_line_width(0.5);
        g->set_color(100, 100, 100, 255);
        for(size_t i = 0; i < highways_medium.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_medium[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
        // major highways
        g->set_line_width(1);
        g->set_color(255, 255, 255, 255);
        for(size_t i = 0; i < highways_major.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_major[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
    }
    // 0.07
    else if ( zoom_factor > 0.05 ){
        // medium highways 
        g->set_line_width(0.5);
        g->set_color(155, 155, 155, 255);
        for(size_t i = 0; i < highways_medium.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_medium[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
        // major highways
        g->set_line_width(1);
        g->set_color(255, 255, 255, 255);
        for(size_t i = 0; i < highways_major.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_major[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
    }
    // 0.046
    else if ( zoom_factor > 0.04 ){
        // minor highways
        g->set_line_width(1);
        g->set_color(50, 50, 50, 255);
        for(size_t i = 0; i < highways_minor.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_minor[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
        // medium highways
        g->set_line_width(1);
        g->set_color(150, 150, 150, 255);
        for(size_t i = 0; i < highways_medium.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_medium[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
        // major highways
        g->set_line_width(2);
        g->set_color(255, 255, 255, 255);
        for(size_t i = 0; i < highways_major.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_major[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
    }
    // 0.027
    else if ( zoom_factor > 0.02 ){
        // minor highways
        g->set_line_width(2);
        g->set_color(100, 100, 100, 255);
        for(size_t i = 0; i < highways_minor.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_minor[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
        // medium highways
        g->set_line_width(2);
        g->set_color(200, 200, 200, 255);
        for(size_t i = 0; i < highways_medium.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_medium[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
        // major highways
        g->set_line_width(5);
        g->set_color(255, 255, 255, 255);
        for(size_t i = 0; i < highways_major.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_major[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }        
    }
    // 0.6^8 (0.016)
    else if ( zoom_factor > 0.016 ){
        // minor highways
        g->set_line_width(3);
        g->set_color(150, 150, 150, 255);
        for(size_t i = 0; i < highways_minor.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_minor[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
        // medium highways
        g->set_line_width(3);
        g->set_color(200, 200, 200, 255);
        for(size_t i = 0; i < highways_medium.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_medium[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
        // major highways
        g->set_line_width(10);
        g->set_color(255, 255, 255, 255);
        for(size_t i = 0; i < highways_major.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_major[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
    }
    else {
        // minor highways
        g->set_line_width(10);
        g->set_color(150, 150, 150, 255);
        for(size_t i = 0; i < highways_minor.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_minor[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
        // medium highways
        g->set_line_width(10);
        g->set_color(200, 200, 200, 255);
        for(size_t i = 0; i < highways_medium.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_medium[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
            }
        }
        // major highways
        g->set_line_width(10);
        g->set_color(255, 255, 255, 255);
        for(size_t i = 0; i < highways_major.size(); ++i){
            std::vector<ezgl::point2d> these_points = highways_major[i].allPoints;
            for (int pnt = 0; pnt < (these_points.size()-1); ++pnt){
                //get coordinate of two points and draw
                ezgl::point2d point1 = these_points[pnt  ];
                ezgl::point2d point2 = these_points[pnt+1];
                g->draw_line(point1, point2);
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
            // non-closed
            else{
                for(int pts=0; pts<this_feature.allPoints.size()-1; pts++) {
                    ezgl::point2d position1 = this_feature.allPoints[pts  ];
                    ezgl::point2d position2 = this_feature.allPoints[pts+1];
                    g->set_line_width(2);
                    g->set_color(100, 150, 200, 255);
                    g->draw_line(position1, position2);
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
            // non-closed
            else{
                for(int pts=0; pts<this_feature.allPoints.size()-1; pts++) {
                    ezgl::point2d position1 = this_feature.allPoints[pts  ];
                    ezgl::point2d position2 = this_feature.allPoints[pts+1];
                    g->set_line_width(5);
                    g->set_color(100, 150, 200, 255);
                    g->draw_line(position1, position2);
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
    if (visible_width < 2000){
        for(int poi=0; poi<POIs.size(); poi++) {
            float x = POIs[poi].x_;
            float y = POIs[poi].y_;
            if (POIs[poi].highlight) {
                g->set_color(ezgl::MAGENTA);
                g->fill_arc({x, y}, 5, 0, 360);
                ezgl::surface *png_surface = ezgl::renderer::load_png("small_image.png");
                g->draw_surface(png_surface, {x, y});
                ezgl::renderer::free_surface(png_surface);
            }
            else {
                 g->set_color(ezgl::RED);
                g->fill_arc({x, y}, 2, 0, 360);
            }
        }
    }
}

void draw_street_names(ezgl::renderer *g) {
    g->set_font_size(10);
    g->set_color(0, 0, 0, 255);
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
            //g->draw_text(middle,street_name,std::min((double)(10*name_size),segment_length),10);
            g->draw_text(middle,street_name,(double)(10*name_size),10);
        }
    }
}


/*************************Callback Functions*************************/

void initial_setup(ezgl::application *application, bool new_window){
    // Update the status bar message
    application->update_message("EZGL Application"); 
    
    // Update the title bar
    GtkWindow* main_window = (GtkWindow *)application->get_object("MainWindow");
    gtk_window_set_title(main_window, "Amazing Map");
    
    // link the test button with callback function
    // type casting the object you get with the corresponding object
    GtkButton* find_button = (GtkButton *) application->get_object("FindButton");
    g_signal_connect(find_button, "clicked", G_CALLBACK(FindButton_callback), application);
    
    // Initialize the intersections search window.
    // Link the deletion of popup window with callback function
    GtkWindow* pop_window = (GtkWindow *) application->get_object("IntersectionsSearch");
    gtk_window_set_title(pop_window, "Streets Search");
    // the second parameter is the signal that triggers the callback function, the fourth parameter is used as user data (the one i want to pass into the callback function)
    g_signal_connect(pop_window, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);
    
    // Link pop entry key press "Return" with callback function
    GtkEntry* pop_entry = (GtkEntry *) application->get_object("Popup_Entry");
    g_signal_connect(pop_entry, "key-release-event", G_CALLBACK(StreetsEntryReturn_callback), application);
    
    // editing-done doesn't work on GtkEntry! It's a lie!
    //g_signal_connect((GtkCellEditable*)pop_entry, "editing-done", G_CALLBACK(Test_callback), application);
    
    // Link pop entry text change with callback function
    g_signal_connect(pop_entry, "changed", G_CALLBACK(StreetsEntryChange_callback), application);
    
    // force the completion to show
    GtkEntryCompletion* completion = gtk_entry_get_completion(pop_entry);
    gtk_entry_completion_set_match_func (completion, forced_auto_completion, NULL, NULL);
    gtk_entry_completion_set_text_column(completion, 1);
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
        std::string status_message = "Closest POI: " + POIs[idForPOI].name;
        app->update_message(status_message);
    }
    
    //find closest intersection_id, -1 means too far away to any intersections
    int id = find_closest_intersection(pos);
    //un-highlight the last highlighted intersection
    std::vector<int>* last_intersections = &(memory.last_highlighted_intersections);
    for (int i = 0; i < last_intersections->size(); ++i){
        intersections[(*last_intersections)[i]].highlight = false;
    }
    last_intersections->clear();
    // set new value of last highlighted
    last_intersections->push_back(id);
    // highlight current clicked
    if (id != -1) {
        intersections[id].highlight = true;
        std::string status_message = "Closest Intersection: " + intersections[id].name;
        app->update_message(status_message);
    }
    
    app->refresh_drawing();
}

// use key_release_event for signals instead of key_press
void act_on_key_press(ezgl::application *application, GdkEventKey */*event*/, char *key_name)
{
    std::string key_pressed(key_name);
    /*
    application->update_message("Key Pressed");

    std::cout << key_name << " key is pressed" << std::endl;
    */
    
    return;
}

// initialize the pop up window and present it
void FindButton_callback(GtkButton* /*widget*/, ezgl::application *application){
        // initialize entry (empty)
        GtkEntry* entry = (GtkEntry*)application->get_object("Popup_Entry");
        gtk_entry_set_text (entry, "");
        // initialize label ("first street name")
        GtkLabel* label = (GtkLabel*)application->get_object("Popup_Label");
        gtk_label_set_text (label, "Please enter the first street name");
        // initialize the StatusBar
        GtkWidget* status = (GtkWidget*)application->get_object("Popup_StatusBar");
        gtk_widget_show(status);
        // present the window
        GtkWindow* popup = (GtkWindow*)application->get_object("IntersectionsSearch");
        gtk_window_present (popup);
}

void StreetsEntryReturn_callback(GtkEntry* widget, GdkEventKey* event, ezgl::application *application){
    // get key pressed
    std::string key_released(gdk_keyval_name(event->keyval));
    if ( key_released == "Return"){
        // get the StatusBar
        GtkStatusbar* status = (GtkStatusbar*)application->get_object("Popup_StatusBar");
        // get the label
        GtkLabel* label = (GtkLabel*)application->get_object("Popup_Label");
        // get the entry text
        std::string entryText(gtk_entry_get_text(widget));
        // get the global variable list store
        std::unordered_map<std::string, int>* last_list_mem = &(memory.last_autocompletion_list);
        auto foundMatch = last_list_mem->find(entryText);
        // if entry is valid proceed
        if (foundMatch != last_list_mem->end()){
            // street id found
            int street_id = foundMatch->second;
            // re-initialize entry and status bar
            gtk_entry_set_text (widget,"");
            gtk_statusbar_push (status,0,"");
            // insert the entry into memory
            std::vector<int>* entry_mem = &(memory.last_entry);
            entry_mem->push_back(street_id);
            // if push_back has a size of 0, asking for first street name
            if      (entry_mem->size() == 0){
                gtk_label_set_text (label, "Please enter the first street name");
            }
            // if push_back has a size of 1, asking for second street name
            else if (entry_mem->size() == 1){
                gtk_label_set_text (label, "Please enter the second street name");
            }
            // if push_back has a size of 2, double check both entries
            else if (entry_mem->size() == 2){
                // same ids 
                if ((*entry_mem)[0] == (*entry_mem)[1]){
                    // go back to asking for first street name
                    gtk_statusbar_push (status,0,"Invalid entry: Please enter different street names");
                    gtk_label_set_text (label, "Please enter the first street name");
                    // initialize entry_mem
                    entry_mem->clear();
                }
                // different ids
                else{
                    std::cout<<"Search done\n";  
                    // call the function to find intersections of two streets
                    std::vector<int> intersections_found = find_intersections_of_two_streets(std::make_pair((*entry_mem)[0],(*entry_mem)[1]));
                    // initialize entry_mem
                    entry_mem->clear();
                    // if empty, no intersections
                    if (intersections_found.empty()){
                        gtk_label_set_text (label,"Search done:\nNo intersections between two streets");
                        gtk_widget_hide((GtkWidget*)status);
                        gtk_widget_hide((GtkWidget*)widget);
                    }
                    // if not empty, hide window, highlight intersections
                    else{
                        // get and hide the window
                        GtkWidget* window = (GtkWidget*)application->get_object("IntersectionsSearch");
                        gtk_widget_hide(window);
                        // call the function present the intersections found
                        IntersectionsSearchResult(intersections_found, application);
                    }
                }
            }
        }
        // if entry not valid, update status bar
        else{
            gtk_statusbar_push (status,0,"Invalid entry: Please follow the format in the dropdown menu");
        }
    }
}

void StreetsEntryChange_callback(GtkEntry* widget, ezgl::application *application){
    // intialize status bar
    GtkStatusbar* status = (GtkStatusbar*)application->get_object("Popup_StatusBar");
    gtk_statusbar_push (status,0,"");
    // get the auto completion and the auto completion list
    GtkEntryCompletion* completion = gtk_entry_get_completion(widget);
    std::unordered_map<std::string, int>* last_list_mem = &(memory.last_autocompletion_list);
    GtkListStore*       list       = (GtkListStore*) gtk_entry_completion_get_model(completion);
    // get the entry
    std::string entryText(gtk_entry_get_text(widget));
    std::cout<<entryText<<std::endl;
    // if a bracket exist, stop updating the auto completion
    if (entryText.find('(') != std::string::npos){
        return;
    }
    // get all the auto completions (if no bracket)
    std::vector<int> streets_idxs = find_street_ids_from_partial_street_name(entryText);
    // clear and the widget list store and global variable list store
    gtk_list_store_clear (list);
    last_list_mem->clear();
    // fill the widget list store and global variable list store
    GtkTreeIter iter;
    int i;
    for(i=0; i<streets_idxs.size(); i++) {
        gtk_list_store_append(list, &iter);
        int street_idx = streets_idxs[i];
        std::string street_name = getStreetName(street_idx);
        street_name += " (";
        street_name += std::to_string(street_idx);
        street_name += ')';
        const char* street_name_idx = street_name.c_str();
        gtk_list_store_set(list, &iter, 0, street_idx, 1, street_name_idx, -1);
        last_list_mem->insert(std::make_pair(street_name_idx, street_idx));
    }
}

gboolean forced_auto_completion(GtkEntryCompletion */*completion*/, const gchar */*key*/, GtkTreeIter */*iter*/, gpointer /*user_data*/){
    return true;
}

void IntersectionsSearchResult(std::vector<int> intersections_found, ezgl::application *application){
    //un-highlight the last highlighted intersection
    for (int i = 0; i < memory.last_highlighted_intersections.size(); ++i){
        intersections[memory.last_highlighted_intersections[i]].highlight = false;
    }
    memory.last_highlighted_intersections.clear();
    // set new value of last highlighted
    for (int i = 0; i < intersections_found.size(); ++i){
        memory.last_highlighted_intersections.push_back(intersections_found[i]);
    }
    // highlight currently found intersections
    for (int i = 0; i < intersections_found.size(); ++i){
        intersections[intersections_found[i]].highlight = true;
        //print intersection info
        std::cout << intersections_found[i] << "  " << intersections[intersections_found[i]].name <<std::endl;
    }
    // change the visible to the point highlighted
    ezgl::rectangle visible_world = application->get_renderer()->get_visible_world();
    float visible_width  = visible_world.width();
    float visible_height = visible_world.height();
    float desired_width  = 100;
    float desired_height = 100/visible_width*visible_height;
    ezgl::point2d starting_point(intersections[intersections_found[0]].x_ - desired_width/2,
                                 intersections[intersections_found[0]].y_ - desired_height/2);
    ezgl::rectangle new_world_view(starting_point, desired_width, desired_height);
    application->get_renderer()->set_visible_world(new_world_view);
    application->refresh_drawing();
    return;
}

void Test_callback(GtkEntry* widget, ezgl::application *application){
    std::cout<<"it's alive!!!\n";
}

/*************************Helper Functions*************************/

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