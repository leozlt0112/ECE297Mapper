/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m3_more.h
 * Author: wangbox2
 *
 * Created on March 12, 2020, 10:32 AM
 */

#pragma once //protects against multiple inclusions of this header file

#include "m1.h"
#include "m2.h"
#include "m3.h"

/************ These are the information types that will be used ***************/

// this is a variable type for each element in variable WaveFront
// this is a variable type during path finding only, does not store final result
struct WaveElem{
    
};


// a vector[intersection_idx] storing intersection data
extern std::vector<intersection_info> intersections;