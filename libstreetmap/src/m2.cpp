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
                    nullptr, act_on_key_press);
    return;
}

void draw_map_load (){
    intersections.resize(getNumIntersections());
    streetSegments.resize(getNumStreetSegments());
    features.resize(getNumFeatures());
//    POIs.resize(getNumPointsOfInterest());
    
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
    
    // std::vector<poiCategory> POIs;
    for(int i=0; i< getNumPointsOfInterest(); i++ ) {
        poi_info new_poi;
        LatLon this_position      = getPointOfInterestPosition(i);
        new_poi.x_   = x_from_lon(this_position.lon());
        new_poi.y_   = y_from_lat(this_position.lat());
        new_poi.name = getPointOfInterestName(i);
        std::string poiCategory=getPointOfInterestType(i);
        if( poiCategory=="ferry_terminal" || poiCategory== "theatre" 
           || poiCategory == "community_centre" || poiCategory == "nightclub" 
           || poiCategory == "pub" || poiCategory ==  "stripclub" 
           || poiCategory == "cinema" || poiCategory == "bar" 
           || poiCategory == "cafe" || poiCategory=="old_cafe" 
                || poiCategory== "social_facility" || poiCategory == "lotto" || 
                poiCategory=="betting" ) {
            new_poi.type = "entertainment";
            entertainment_POIS.push_back(new_poi);
        }
        
        else if (poiCategory=="fast_food" || poiCategory == "restaurant" 
                || poiCategory == "ice_cream" || poiCategory == "food_court"
                || poiCategory == "vending_machine") {
            new_poi.type = "food";
            food_POIS.push_back(new_poi);
        }
        else if (poiCategory=="school" || poiCategory == "community_centre" 
                    ||poiCategory=="parking" || poiCategory=="library" 
                    || poiCategory=="post_office") {
            new_poi.type = "public_gathering";
            public_gathering_POIS.push_back(new_poi);
        }
        else {
            new_poi.type = "other";
            other_POIS.push_back(new_poi);
        }    
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
            // 0.6^8
            else if (visible_width > 0.016 * initial_width){
                if (this_segment.major_minor==2){
                    g->set_line_width(10);
                    g->set_color(255, 255, 255, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(7);
                    g->set_color(200, 200, 200, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==0){
                    g->set_line_width(7);
                    g->set_color(150, 150, 150, 255);
                    g->draw_line(point1, point2);
                }
            }
            else {
                if (this_segment.major_minor==2){
                    g->set_line_width(15);
                    g->set_color(255, 255, 255, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==1){
                    g->set_line_width(15);
                    g->set_color(200, 200, 200, 255);
                    g->draw_line(point1, point2);
                }
                else if (this_segment.major_minor==0){
                    g->set_line_width(15);
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
        for(int poi=0; poi<entertainment_POIS.size(); poi++) {
            float x = entertainment_POIS[poi].x_;
            float y = entertainment_POIS[poi].y_;
            //if (entertainment_POIS[poi].highlight) {
              //  g->set_color(ezgl::MAGENTA);
              //  g->fill_arc({x, y}, 5, 0, 360);
                //ezgl::surface *png_surface = ezgl::renderer::load_png("small_image.png");
                //g->draw_surface(png_surface, {x, y});
                //ezgl::renderer::free_surface(png_surface);
           // }
            //else {
                g->set_color(ezgl::RED);
                g->fill_arc({x, y}, 2, 0, 360);
            //}
        }
        for(int poi=0; poi<food_POIS.size(); poi++) {
            float x = food_POIS[poi].x_;
            float y = food_POIS[poi].y_;
//            if (food_POIS[poi].highlight) {
//                g->set_color(ezgl::MAGENTA);
//                g->fill_arc({x, y}, 5, 0, 360);
//                //ezgl::surface *png_surface = ezgl::renderer::load_png("small_image.png");
//                //g->draw_surface(png_surface, {x, y});
//                //ezgl::renderer::free_surface(png_surface);
//            }
//            else {
                g->set_color(ezgl::ORANGE);
                g->fill_arc({x, y}, 2, 0, 360);
//            }
        }
        for(int poi=0; poi<public_gathering_POIS.size(); poi++) {
             float x = public_gathering_POIS[poi].x_;
            float y = public_gathering_POIS[poi].y_;
//            if (public_gathering_POIS[poi].highlight) {
//                g->set_color(ezgl::MAGENTA);
//                g->fill_arc({x, y}, 5, 0, 360);
//                //ezgl::surface *png_surface = ezgl::renderer::load_png("small_image.png");
//                //g->draw_surface(png_surface, {x, y});
//                //ezgl::renderer::free_surface(png_surface);
//            }
//            else {
                g->set_color(ezgl::YELLOW);
                g->fill_arc({x, y}, 2, 0, 360);
//            }
        }
        for(int poi=0; poi<other_POIS.size(); poi++) {
             float x = other_POIS[poi].x_;
            float y = other_POIS[poi].y_;
//            if (other_POIS[poi].highlight) {
//                g->set_color(ezgl::MAGENTA);
//                g->fill_arc({x, y}, 5, 0, 360);
//                //ezgl::surface *png_surface = ezgl::renderer::load_png("small_image.png");
//                //g->draw_surface(png_surface, {x, y});
//                //ezgl::renderer::free_surface(png_surface);
//            }
//            else {
                g->set_color(ezgl::GREEN);
                g->fill_arc({x, y}, 2, 0, 360);
//            }
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
    
    /*
    // link the find button with callback function
    GtkToggleButton* find_button = (GtkToggleButton *) application->get_object("FindButton");
    g_signal_connect(find_button, "toggled", G_CALLBACK(FindButton_callback), application);
    */
    
    // link the test button with callback function
    // type casting the object you get with the corresponding object
    GtkButton* find_button = (GtkButton *) application->get_object("FindButton");
    g_signal_connect(find_button, "clicked", G_CALLBACK(FindButton_callback), application);
    
    // Initialize the intersections search window.
    // Link the deletion of popup window with callback function
    GtkWindow* pop_window = (GtkWindow *) application->get_object("IntersectionsSearch");
    gtk_window_set_title(pop_window, "Streets Intersections Search");
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
//    if(memory.last_clicked_POI !=-1) 
//        POIs[memory.last_clicked_POI].highlight = false; 
//    // set the value of last_clicked
//    memory.last_clicked_POI = idForPOI;
//    // highlight current clicked 
//    if (idForPOI != -1) {
//        POIs[idForPOI].highlight = true;
//        std::string status_message = "Closest POI: " + POIs[idForPOI].name;
//        app->update_message(status_message);
//    }
    
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

// use key_release_event for signals instead of key_press
void act_on_key_press(ezgl::application *application, GdkEventKey */*event*/, char *key_name)
{
    std::string key_pressed(key_name);
    /*
    application->update_message("Key Pressed");

    std::cout << key_name << " key is pressed" << std::endl;
    */
    
    /*
    // read entry when it is visible, then perform actions accordingly
    auto popCombo    = application->get_object("Results_Matched");
    auto popCombo    = application->get_object("Results_Matched");
    if (button_state && key_pressed=="Return"){
        std::string Street_1,Street_2;
        GtkWindow* popWindow    = (GtkWindow*) application->get_object("IntersectionsSearch");
        std::cout<<"yooo\n";
        // get entry text
        auto popEntry  = application->get_object("Popup_Entry");
        std::string entryText(gtk_entry_get_text((GtkEntry*)popEntry));
        // get and check the label
        GtkLabel* popLabel = (GtkLabel*) application->get_object("Popup_Label");
        std::string labelText(gtk_label_get_text(popLabel));
        // check if prompting street name 1 or 2 entering
        if (labelText == "Please enter the first street name"){
            // get street1 entry
            Street_1 = entryText;
            std::cout<<"entry1:"<<Street_1<<"\n";
            // update the label
            gtk_label_set_text(popLabel, "Please select the first street name");
            // clear and hide the entry
            gtk_widget_set_visible((GtkWidget*)popEntry,false);
            // populate the combo box
            
            // set the combo box visible
            
        }
        else if (labelText == "Please enter the first street name"){
            // get street2 entry
            Street_2 = entryText;
            std::cout<<"entry2:"<<Street_2<<"\n";
        }
        
    }
     */
    
        /*
        {
        // get street1 entry
        GtkEntry* text_entry1 = (GtkEntry *) application->get_object("TextEntry1");
        std::string firstStreet(gtk_entry_get_text(text_entry1));
        std::cout<<"entry1:"<<firstStreet<<"\n";

        //get street ids from street name 1
        std::vector<int> first_ids  = find_street_ids_from_partial_street_name(firstStreet);
        for (int i=0; i <first_ids.size(); ++i)
            std::cout << first_ids[i] << "  " << getStreetName(first_ids[i]) << std::endl;

        // get street2 entry
        GtkEntry* text_entry2 = (GtkEntry *) application->get_object("TextEntry2");
        std::string secondStreet(gtk_entry_get_text(text_entry2));
        std::cout<<"entry2:"<<secondStreet<<"\n";

        // get street ids from street name 2
        std::vector<int> second_ids = find_street_ids_from_partial_street_name(secondStreet);
        for (int i=0; i <second_ids.size(); ++i)
            std::cout << second_ids[i] << "  " << getStreetName(second_ids[i]) << std::endl;
        
        // check if no streets found or both entries are the same street
        if (first_ids.size()==0 || second_ids.size()==0 || first_ids[0] == second_ids[0]){
            std::cout<<"invalid entry"<<std::endl;
            return;
        }
        
        // find intersection ids for two intersecting streets
        std::pair<int, int> street_ids;
        street_ids = std::make_pair(first_ids[0], second_ids[0]); 
        std::vector<int> intersections_id;
        intersections_id = find_intersections_of_two_streets(street_ids);
        
        // intersections not found
        if (intersections_id.size()==0){
            std::cout<<"no intersections"<<std::endl;
            return;
        }
        
        // intersections found and highlighted
        for(int i=0; i<intersections_id.size(); ++i){
            intersections[intersections_id[i]].highlight = true;
            //print intersection info
            std::cout << intersections_id[i] << "  " << intersections[intersections_id[i]].name <<std::endl;
        }
        
        // change the visible to the point highlighted
        ezgl::rectangle visible_world = application->get_renderer()->get_visible_world();
        float visible_width  = visible_world.width();
        float visible_height = visible_world.height();
        float desired_width  = 100;
        float desired_height = 100/visible_width*visible_height;
        ezgl::point2d starting_point(intersections[intersections_id[0]].x_ - desired_width/2,
                                     intersections[intersections_id[0]].y_ - desired_height/2);
        ezgl::rectangle new_world_view(starting_point, desired_width, desired_height);
        application->get_renderer()->set_visible_world(new_world_view);
        application->refresh_drawing();
        
        return;
    }
    */
    

}

/*
// A callback function for Find button 
// it turns on/off the search bar
void FindButton_callback(GtkToggleButton* widget, ezgl::application *application){
    gboolean button_state = gtk_toggle_button_get_active(widget);
    GtkWidget* entry1 = (GtkWidget *) application->get_object("TextEntry1");
    GtkWidget* entry2 = (GtkWidget *) application->get_object("TextEntry2");
    if (button_state){
        gtk_widget_set_visible(entry1, true);
        gtk_widget_set_visible(entry2, true);
    }
    else{
        gtk_widget_set_visible(entry1, false);
        gtk_widget_set_visible(entry2, false);
    }
}
*/

// initialize the pop up window and present it
void FindButton_callback(GtkButton* /*widget*/, ezgl::application *application){
        // initialize entry (empty)
        GtkEntry* entry = (GtkEntry*)application->get_object("Popup_Entry");
        gtk_entry_set_text (entry, "");
        // initialize status bar
        GtkStatusbar* status = (GtkStatusbar*)application->get_object("PopupStatusBar");
        // initialize label ("first street name")
        GtkLabel* label = (GtkLabel*)application->get_object("Popup_Label");
        gtk_label_set_text (label, "Please enter the first street name");
        // present the window
        GtkWindow* popup = (GtkWindow*)application->get_object("IntersectionsSearch");
        gtk_window_present (popup);
}

void StreetsEntryReturn_callback(GtkEntry* widget, GdkEventKey* event, ezgl::application *application){
    // get key pressed
    std::string key_released(gdk_keyval_name(event->keyval));
    if ( key_released == "Return"){
        // get the entry text
        std::string entryText(gtk_entry_get_text(widget));
        // store it in global variable
        memory.last_entry = entryText;
        std::cout<<entryText<<std::endl;
    }
}

void StreetsEntryChange_callback(GtkEntry* widget, ezgl::application *application){
    // get the auto completion and the auto completion list
    GtkEntryCompletion* completion = gtk_entry_get_completion(widget);
    GtkListStore*       list       = (GtkListStore*) gtk_entry_completion_get_model(completion);
    // get all the auto completions
    std::string entryText(gtk_entry_get_text(widget));
    std::cout<<entryText<<std::endl;
    std::vector<int> streets_idxs = find_street_ids_from_partial_street_name(entryText);
    // clear and fill the list store
    gtk_list_store_clear (list);
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
    }
}

gboolean forced_auto_completion(GtkEntryCompletion *completion, const gchar *key, GtkTreeIter *iter, gpointer user_data){
    return true;
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