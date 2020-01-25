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
    //Indicates whether the map has loaded successfully
    bool load_successful = loadStreetsDatabaseBIN(map_path); 
    
    intersection_street_segments.resize(getNumIntersections());
    
    for (int intersection =0; intersection < getNumIntersections(); ++intersection){
        for (int i=0; i<getIntersectionStreetSegmentCount(intersection); ++i){
            int ss_id = getIntersectionStreetSegment(intersection,i);
            intersection_street_segments[intersection].push_back(ss_id);
        }
    }
    
    //Make sure this is updated to reflect whether loading the map succeeded or failed
    return load_successful;
}

void close_map() {
    //Clean-up your map related data structures here
    closeStreetDatabase(); 
}

//
double find_distance_between_two_points(std::pair<LatLon, LatLon> points){
    std::pair<XY_,XY_> points_XY=pair_of_LatLon_to_XY(points);
    double d=EARTH_RADIUS_METERS * std::sqrt(std::pow(points_XY.first.x_-points_XY.second.x_,2)+std::pow(points_XY.first.y_-points_XY.second.y_,2));
    return d;
}

double find_street_segment_length(int street_segment_id){
    int from = getInfoStreetSegment(street_segment_id).from;
    int to = getInfoStreetSegment(street_segment_id).to;
    int curvePointCount = getInfoStreetSegment(street_segment_id).curvePointCount;
    
    double total_length=0;
    std::pair<LatLon, LatLon> two_points;
    
    if (curvePointCount){
        // distance between "from" and "0'th point"
        two_points.first = getIntersectionPosition(from);
        two_points.second = getStreetSegmentCurvePoint(0, street_segment_id);
        total_length += find_distance_between_two_points(two_points);       
        // distance between "0th" and "curvePointCount-1'th point"
        for (int i=1; i<(curvePointCount-1); i++){
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
    return find_street_segment_length(street_segment_id)/(getInfoStreetSegment(street_segment_id).speedLimit);
}

int find_closest_intersection(LatLon my_position){
    return 0;
}

std::vector<int> find_street_segments_of_intersection(int intersection_id){
    std::vector<int> temp(1,0);
    return temp;
}

//
std::vector<std::string> find_street_names_of_intersection(int intersection_id){
    std::vector<std::string> temp(1,0);
    return temp;
}

bool are_directly_connected(std::pair<int, int> intersection_ids){
    return true;
}

std::vector<int> find_adjacent_intersections(int intersection_id){
    std::vector<int> temp(1,0);
    return temp;
}
// Leo 
std::vector<int> find_street_segments_of_street(int street_id){
    std::vector<int> temp(1,0);
    return temp;
}

std::vector<int> find_intersections_of_street(int street_id){
    std::vector<int> result_d;
    //loop thru all intersections 
    for (int i=0; i<getNumStreetSegments(); ++i) {
        InfoStreetSegment streetSeg = getInfoStreetSegment(i);
        if(streetSeg.streetID==street_id) {
            result_d.push_back(streetSeg.to);
            result_d.push_back(streetSeg.from);
        }
    }
    //remove duplicates
    std::sort(result_d.begin(),result_d.end());
    /*
    std::vector<int>::iterator it;
    it=std::unique(result_d.begin(), result_d.end());
    result_d.resize(std::distance(result_d.begin(),it));
     */
    result_d.erase( std::unique(result_d.begin(),result_d.end()), result_d.end());
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
    double lat_avg=0.5*(lat_first+lat_second);
    std::pair<XY_,XY_> points_XY;
    points_XY.first.x_=lon_first * std::cos(lat_avg);
    points_XY.first.y_=lat_first;
    points_XY.second.x_=lon_second * std::cos(lat_avg);
    points_XY.second.y_=lat_second;
    return points_XY;
}