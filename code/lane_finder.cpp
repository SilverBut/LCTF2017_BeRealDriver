//
// Created by bbmagnum on 10/30/17.
//

#ifdef DEBUG
#include <iostream>
#endif
#include "lane_finder.hpp"
#include "config.hpp"

void find_lane(cv::Mat &Input, const int window_init, std::vector<cv::Point> &Output) {
  for (int32_t window_x = window_init + lane_window_offset, window_y = current_pos_row;
       window_y + lane_window_height < Input.rows && window_y > 0 &&
           window_x > 0 && window_x + lane_window_width < Input.cols;
      ) {
#ifdef DEBUG
//    std::cout << "window_x = " << window_x << ", window_y = " << window_y << std::endl;
//    std::cout << "lane_window_width = " << lane_window_width << ", lane_window_height = " << lane_window_height
//              << std::endl;
#endif
    // for each window:
    // 0. Put current window into the vector
    Output.emplace_back(cv::Point(window_x, window_y));
    // 1. Get edge image in the window
    cv::Mat window_image;
    Input(
        cv::Range(window_y, window_y + lane_window_height),
        cv::Range(window_x, window_x + lane_window_width)
    ).copyTo(window_image);
    // 2. Find the center point of the image, which we think is the path point!
    cv::Moments window_moments = cv::moments(window_image);
    cv::Point window_point;
    double x = window_moments.m10 / window_moments.m00;
    double y = window_moments.m01 / window_moments.m00;
    window_point.x = static_cast<int>(window_x + x);
    window_point.y = static_cast<int>(window_y + y);
#ifdef DEBUG
//    std::cout << x << " " << y << std::endl;
//    std::cout << window_point << std::endl;
#endif
    // 3. Update window parameter
    window_x = window_point.x + lane_window_offset;
    window_x = window_x < 0 ? 0 :
               window_x + lane_window_width > Input.cols ? Input.cols - lane_window_width : window_x + 2;
    window_y = window_point.y;
    window_y = window_y < 0 ? 0 :
               window_y + lane_window_height > Input.rows ? Input.rows - lane_window_height : window_y + 2;
  }

}

