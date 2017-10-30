//
// Created by bbmagnum on 10/30/17.
//

#ifndef BEREALDRIVER_LANE_FINDER_CPP_HPP
#define BEREALDRIVER_LANE_FINDER_CPP_HPP

#include <opencv2/imgproc.hpp>

void find_lane(cv::Mat &Input, const int window_init, std::vector<cv::Point> &Output);

#endif //BEREALDRIVER_LANE_FINDER_CPP_HPP
