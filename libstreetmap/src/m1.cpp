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
#include "StreetsDatabaseAPI.h"
#include "math.h"

int earth_radius=6378100;

bool load_map(std::string /*map_path*/) {
    bool load_successful = false; //Indicates whether the map has loaded 
                                  //successfully
    
    //
    //Load your map related data structures here
    //
    
    

    load_successful = true; //Make sure this is updated to reflect whether
                            //loading the map succeeded or failed

    return load_successful;
}

void close_map() {
    //Clean-up your map related data structures here
    
}

//
double find_distance_between_two_points(std::pair<LatLon, LatLon> points){
    double lat_avg=0.5*(points.first.lat()+points.second.lat());
    double x1=points.first.lon() * cos(lat_avg);
    double y1=points.first.lat();
    double x2=points.second.lon() * cos(lat_avg);
    double y2=points.second.lat();
    double d=earth_radius * sqrt(pow(y2-y1,2)+pow(x2-x1,2));
    return d;
}

double find_street_segment_length(int street_segment_id){
    return 0;
}

double find_street_segment_travel_time(int street_segment_id){
    return 0;
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

std::vector<int> find_street_segments_of_street(int street_id){
    std::vector<int> temp(1,0);
    return temp;
}

std::vector<int> find_intersections_of_street(int street_id){
    std::vector<int> temp(1,0);
    return temp;
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