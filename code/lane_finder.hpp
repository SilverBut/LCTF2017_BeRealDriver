//
// Created by bbmagnum on 10/30/17.
//

#ifndef BEREALDRIVER_LANE_FINDER_CPP_HPP
#define BEREALDRIVER_LANE_FINDER_CPP_HPP

#include <opencv2/imgproc.hpp>

void find_edge(cv::Mat &Input, cv::Mat &Output);
void find_init_lane(cv::Mat &Input, int &Output_window_left, int &Output_window_right);
void find_lane(cv::Mat &Input, const int window_init, std::vector<cv::Point> &Output);

#endif //BEREALDRIVER_LANE_FINDER_CPP_HPP
