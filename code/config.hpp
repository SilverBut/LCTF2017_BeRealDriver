//
// Created by bbmagnum on 10/30/17.
//

#ifndef BEREALDRIVER_CONFIG_HPP
#define BEREALDRIVER_CONFIG_HPP

#include <cstdint>

#ifdef DEBUG
// Debug level:
//   0  = None
//   10 = Param info
//   20 = Matrix value
//   60 = Param info and matrix value, even in loop
#define DEBUG_LEVEL 10
#define DEBUG_VISION 1
// We now need to use file source. However later we will use more things
#define DEBUG_SOURCE_FILE 1
#else
#define DEBUG_LEVEL 0
#define DEBUG_VISION 0
#define DEBUG_SOURCE_FILE 0
#endif

/* Give some edge filter parameters here */
const auto edge_blur = 4;
const auto edge_threshold = 60;

/*
 * Assume we are using a image axis, so the value for three parts are:
 *  Part a:  350
 *  Part b:  950
 *  Part c: 1600
 */

// Let's have a initial position in the graph
// Assume current position is (950, 10)
const uint16_t current_pos_col = 950;
const uint16_t current_pos_row = 10;
// And we set the road width to 3m=600px. add a range to it
const uint16_t lane_space_width_min = 500;
const uint16_t lane_space_width_max = 700;
// And also set our car's information
const uint16_t car_len = 720;
const uint16_t car_width = 400;

// For each lane, its normal width is between 30px to 40px
// So we add -10px to our window, and make our detector window width is 25px
// it should fit for most situations
const int32_t lane_window_width = 15;
const int32_t lane_window_offset = -10;
const int32_t lane_window_height = 10;

const double path_zoom_factor = double(car_width) / double(lane_space_width_max);

const double ldw_warn_threshold = 0.9;
const double crash_warn_threshold = 0.5;

const uint8_t crash_areas_inital_value[][4] =
    {  //x, y, w, h
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1}
    };

#endif //BEREALDRIVER_CONFIG_HPP
