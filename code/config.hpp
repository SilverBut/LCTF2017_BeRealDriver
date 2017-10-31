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
#define DEBUG_VISION 0
// We now need to use file source. However later we will use more things
#define DEBUG_SOURCE_FILE 1
#define REAL_FLAG 0
#else
#define DEBUG_LEVEL 0
#define DEBUG_VISION 0
#define DEBUG_SOURCE_FILE 0
#define REAL_FLAG 1
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

const double ldw_warn_threshold = 0.9;
const double crash_warn_threshold = 0.7;

const uint16_t crash_areas_initial_value[7][5] =
    {  //x, y, w, h
        {720, 50, 500, 500, 80},
        {330, 655, 500, 500, 70},
        {70, 1510, 520, 1050, 75},
        {535, 2585, 615, 660, 65},
        {1070, 3900, 455, 430, 70},
        {1350, 4710, 455, 430, 80},
        {710, 5790, 440, 210, 75}
    };

#if REAL_FLAG
const std::string flag =
    "Hey man you are a fxxking real driver! Flag is LCTF{We1c0m3_to_ldw_wor1d_And_is_th3_malf0rmed_fi13_1337_en0ugh_f0r_me}";
#else
const std::string flag = "Hey man you are a fxxking real driver! You get the flag!";
#endif

#endif //BEREALDRIVER_CONFIG_HPP
