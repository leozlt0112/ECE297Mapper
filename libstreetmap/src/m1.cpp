/* 
 * Copyright 2020 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "m1.h"
#include "m1_more.h"
#include "StreetsDatabaseAPI.h"
#include "math.h"
#include <algorithm>    // std::sort

 
bool load_map(std::string map_path) {
    //std::cout << "LOADING\n";
    //Indicates whether the map has loaded successfully
    bool load_successful = loadStreetsDatabaseBIN(map_path); 
    
    if (!load_successful) return false;
    intersection_street_segments.resize(getNumIntersections());
    street_street_segments.resize(getNumStreets());

    for (int intersection = 0; intersection < getNumIntersections(); ++intersection){
        for (int i = 0; i < getIntersectionStreetSegmentCount(intersection); ++i){
            int ss_id = getIntersectionStreetSegment(intersection, i);
            intersection_street_segments[intersection].push_back(ss_id);
            
        }
    }

    for (int streetSegment = 0; streetSegment < getNumStreetSegments(); ++streetSegment){
        int street_id = getInfoStreetSegment(streetSegment).streetID; 
        street_street_segments[street_id].push_back(streetSegment);
    }
    return true;
}

void close_map() {
    //std::cout << "CLOSING\n";
    //Clean-up your map related data structures here
    for (int intersection = 0; intersection < getNumIntersections(); ++intersection){   
         intersection_street_segments[intersection].clear();
    }  
    closeStreetDatabase(); 
}

//
double find_distance_between_two_points(std::pair<LatLon, LatLon> points){
    std::pair<XY_,XY_> points_XY=pair_of_LatLon_to_XY(points);
    return EARTH_RADIUS_METERS * std::sqrt(std::pow(points_XY.first.x_-points_XY.second.x_,2)+std::pow(points_XY.first.y_-points_XY.second.y_,2));
}

double find_street_segment_length(int street_segment_id){
    InfoStreetSegment infoStreetSeg=getInfoStreetSegment(street_segment_id);
    IntersectionIndex from = infoStreetSeg.from;
    IntersectionIndex to = infoStreetSeg.to;
    int curvePointCount = infoStreetSeg.curvePointCount;
    
    double total_length=0;
    std::pair<LatLon, LatLon> two_points;
    
    if (curvePointCount){
        // distance between "from" and "0'th point"
        two_points.first = getIntersectionPosition(from);
        two_points.second = getStreetSegmentCurvePoint(0, street_segment_id);
        total_length += find_distance_between_two_points(two_points);
        // distance between "0th" and "curvePointCount-1'th point"
        for (int i=0; i<(curvePointCount-1); i++){
        two_points.first = getStreetSegmentCurvePoint(i, street_segment_id);
        two_points.second = getStreetSegmentCurvePoint(i+1, street_segment_id);
        total_length += find_distance_between_two_points(two_points);
        }
        //distance between "curvePointCount-1'th point" and "to"
        two_points.first = getStreetSegmentCurvePoint(curvePointCount-1, street_segment_id);
        two_points.second = getIntersectionPosition(to);
        total_length += find_distance_between_two_points(two_points);
    }
    else{
        two_points.first = getIntersectionPosition(from);
        two_points.second = getIntersectionPosition(to);
        total_length += find_distance_between_two_points(two_points);
    }
    return total_length;
}

double find_street_segment_travel_time(int street_segment_id){
    return find_street_segment_length(street_segment_id)/(getInfoStreetSegment(street_segment_id).speedLimit)*3.6;
}

int find_closest_intersection(LatLon my_position){
    double closest_distance= 2000000;
    int closest_intersection= 0;
    std::pair<LatLon, LatLon> two_points;
    two_points.first=my_position;
    for (int i=0; i<getNumIntersections(); ++i){
        two_points.second = getIntersectionPosition(i);
        double current_distance = find_distance_between_two_points(two_points);
        if (current_distance<closest_distance){
            closest_distance=current_distance;
            closest_intersection=i;
        }
    }
    return closest_intersection;
}

std::vector<int> find_street_segments_of_intersection(int intersection_id){
    return intersection_street_segments[intersection_id];
}

std::vector<std::string> find_street_names_of_intersection(int intersection_id){
    std::vector<std::string> street_names;
    std::string streetName;
    std::vector<int> street_segments_ids = intersection_street_segments[intersection_id];
    for(int i = 0; i < street_segments_ids.size(); i++) {
        streetName = getStreetName(getInfoStreetSegment(street_segments_ids[i]).streetID);
//        std::cout << street_segments_ids.size() << ", " << i << ": " << street_name << std::endl;
        street_names.push_back(streetName);
    }   
    return street_names;
}

//Returns true if you can get from intersection_ids.first to intersection_ids.second using a single 
//street segment (hint: check for 1-way streets too)
bool are_directly_connected(std::pair<int, int> intersection_ids){
    //get the street segments
    int streetSegmentCount1 = getIntersectionStreetSegmentCount(intersection_ids.first);
    int streetSegmentCount2 = getIntersectionStreetSegmentCount(intersection_ids.second); 
    std::vector<int> streetSegmentId1 = intersection_street_segments[intersection_ids.first];
    std::vector<int> streetSegmentId2 = intersection_street_segments[intersection_ids.second];
    
    //check for all street segments 
    for(int i = 0; i < streetSegmentCount1; i++){
        for(int j = 0;j < streetSegmentCount2; j++){
        //if same street segment
        if(streetSegmentId1[i] == streetSegmentId2[j])
            return true;
        }
    }
    //corner case: an intersection is considered to be connected to itself
    if(intersection_ids.first == intersection_ids.second)
        return true;
    
}

std::vector<int> find_adjacent_intersections(int intersection_id){
    std::vector<int> temp(1,0);
    return temp;
}

std::vector<int> find_street_segments_of_street(int street_id){
    return street_street_segments[street_id];
}

std::vector<int> find_intersections_of_street(int street_id){
    std::vector<int> result_d(1,0);
//    //loop thru all intersections 
//    for (int i=0; i<getNumStreetSegments(); ++i) {
//        InfoStreetSegment streetSeg = getInfoStreetSegment(i);
//        if(streetSeg.streetID==street_id) {
//            result_d.push_back(streetSeg.to);
//            result_d.push_back(streetSeg.from);
//        }
//    }
//    //remove duplicates
//    std::sort(result_d.begin(),result_d.end());
//    /*
//    std::vector<int>::iterator it;
//    it=std::unique(result_d.begin(), result_d.end());
//    result_d.resize(std::distance(result_d.begin(),it));
//     */
//    result_d.erase( std::unique(result_d.begin(),result_d.end()), result_d.end());
    return result_d;
}

std::vector<int> find_intersections_of_two_streets(std::pair<int, int> street_ids){
    std::vector<int> temp(1,0);
    return temp;
}

std::vector<int> find_street_ids_from_partial_street_name(std::string street_prefix){
    std::vector<int> temp(1,0);
    return temp;
}

double find_feature_area(int feature_id){
    return 0;
}

double find_way_length(OSMID way_id){
    return 0;
}

//send in a pair of LatLon, give out a pair of XY(int pair))
std::pair<XY_,XY_> pair_of_LatLon_to_XY(std::pair<LatLon,LatLon> points){
    double lat_first = DEGREE_TO_RADIAN * points.first.lat();
    double lon_first = DEGREE_TO_RADIAN * points.first.lon();
    double lat_second = DEGREE_TO_RADIAN * points.second.lat();
    double lon_second = DEGREE_TO_RADIAN * points.second.lon();
    double lat_avg=(lat_first+lat_second)/2;
    std::pair<XY_,XY_> points_XY;
    points_XY.first.x_=lon_first * std::cos(lat_avg);
    points_XY.first.y_=lat_first;
    points_XY.second.x_=lon_second * std::cos(lat_avg);
    points_XY.second.y_=lat_second;
    return points_XY;
}