//
// Created by bbmagnum on 10/30/17.
//

#ifdef DEBUG
#include <iostream>
#endif
#include <vector>
#include "lane_finder.hpp"
#include "array_tools.hpp"
#include "config.hpp"

void find_init_lane(const cv::Mat &Input, int &Output_window_left, int &Output_window_right) {

  // Threshold the image, and cut first 2m for a initial boundary hist test
  cv::Mat hist_input;
  cv::threshold(
      Input(cv::Range(0, 800), cv::Range(0, Input.size().width)), //input
      hist_input, //output
      1.0,
      1,
      CV_THRESH_BINARY
  );
  cv::Mat hist_result(1, hist_input.cols, CV_32FC1, cv::Scalar(0));
  for (int i = 0; i < hist_input.rows; i++) {
    cv::Mat _tmp_conv;
    hist_input(cv::Range(i, i + 1), cv::Range(0, hist_result.cols)).convertTo(_tmp_conv, CV_32FC1);
    hist_result += _tmp_conv;
  }

#if DEBUG_LEVEL >= 20
  // Now hist_result contains initial vector for driving lanes. Let's try it out
  std::cout << "hist_result: " << std::endl;
  std::cout << hist_result << std::endl;
  int _i = 50;
  std::cout << "Top " << _i << ":" << std::endl;
  std::vector<uint16_t> _v = hist_result;
  for (auto i: array_sort_descending(_v)) {
    std::cout << i << ", " << _v[i] << "; ";
    if (!_i--)
      break;
  }
  std::cout << std::endl;
#endif

  // Do cv::threshold for hist_result again to discard polluted results
  // For this time, we make threshold value is (max()-min())/2
  double hist_max, hist_min;
  cv::minMaxIdx(hist_result, &hist_min, &hist_max);
  double hist_threshold = (hist_max - hist_min) / 2;
#if DEBUG_LEVEL >= 10
  std::cout << "hist_threshold = " << hist_threshold << std::endl;
#endif
  cv::Mat hist_clear;
  cv::threshold(
      hist_result,
      hist_clear,
      hist_threshold,
      1.0,
      CV_THRESH_BINARY
  );
#if DEBUG_LEVEL >= 20
  std::cout << "hist_clear: " << std::endl;
  std::cout << hist_clear << std::endl;
#endif

  /*
   * From now, for most parameters, we can basically use uint16_t since they will not larget than 65536
   * And now it is time to find the initial lane.
   */

  int left_search_point = current_pos_col - lane_space_width_max / 2;
  left_search_point = left_search_point < 0 ? 0 : left_search_point;
  int right_search_point = current_pos_col + lane_space_width_max / 2;
  right_search_point = right_search_point > hist_result.cols ? hist_result.cols : right_search_point;
  int mid_search_point = current_pos_col;

  Output_window_left = -1;
  Output_window_right = -1;
  for (int i = mid_search_point; i > left_search_point; i--) {
    if (hist_clear.at<float>(0, i)) {
      Output_window_left = i;
      break;
    }
  }

  for (int i = mid_search_point; i < right_search_point; i++) {
    if (hist_clear.at<float>(0, i)) {
      Output_window_right = i;
      break;
    }
  }
  assert(Output_window_left != -1 && Output_window_right != -1);
#if DEBUG_LEVEL >= 10
  std::cout << "window_left = " << Output_window_left << ", window_right = " << Output_window_right << std::endl;
#endif
}

void find_edge(const cv::Mat &Input, cv::Mat &Output) {
  // Pre-process it to gray-scale image
  cv::Mat gray;
  cv::cvtColor(Input, gray, cv::COLOR_BGR2GRAY);
  // blur and use Canny to find edge
  cv::Mat blurImage;
  cv::blur(gray, blurImage, cv::Size(edge_blur, edge_blur));
  cv::Canny(blurImage, Output, edge_threshold, edge_threshold * 3);
}

void find_lane(const cv::Mat &Input, const int window_init, std::vector<cv::Point> &Output) {
  int32_t prev_window_x = -1, prev_window_y = -1;
  for (int32_t window_x = window_init + lane_window_offset, window_y = current_pos_row;
       window_y + lane_window_height < Input.rows && window_y > 0 &&
           window_x > 0 && window_x + lane_window_width < Input.cols;
      ) {
#if DEBUG_LEVEL >= 60
    std::cout << "window_x = " << window_x << ", window_y = " << window_y << std::endl;
    std::cout << "lane_window_width = " << lane_window_width << ", lane_window_height = " << lane_window_height
              << std::endl;
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
    // Fixed a bug for having difficulty when processing some special curves
    // Detected if (x,y) is still in the detector window
    if (x > lane_window_width || x < 0 || std::isnan(x)) {
#if DEBUG_LEVEL >= 60
      std::cout << "Warn: bad x at current window " << window_x << ", " << window_y << std::endl;
#endif
      x = lane_window_width - 2;
    }
    if (y > lane_window_height || y < 0 || std::isnan(y)) {
#if DEBUG_LEVEL >= 60
      std::cout << "Warn: bad y at current window " << window_x << ", " << window_y << std::endl;
#endif
      y = lane_window_height - 2;
    }
#if DEBUG_LEVEL >= 60
    std::cout << x << " " << y << std::endl;
    std::cout << window_point << std::endl;
#endif
    window_point.x = static_cast<int>(window_x + x);
    window_point.y = static_cast<int>(window_y + y);
    // 3. Update window parameter
    window_x = window_point.x + lane_window_offset;
    window_x = window_x < 0 ? 0 :
               window_x + lane_window_width > Input.cols ? Input.cols - lane_window_width : window_x;
    window_y = window_point.y;
    window_y = window_y < 0 ? 0 :
               window_y + lane_window_height > Input.rows ? Input.rows - lane_window_height : window_y;
    // Fixed a bug for having difficulty when processing some special curves
    if (prev_window_x == window_x) {
      window_x += 2;
    }
    if (prev_window_y == window_y) {
      window_y += 2;
    }
    prev_window_x = window_x;
    prev_window_y = window_y;
  }

}

void find_road(const cv::Mat &Input, cv::Mat &Output) {
  /*
   * Here we will call a sample function to implement primary functions of a LDW, including:
   *    1. Read a top-view image
   *    2. Find edges and driving lanes in the image
   *    3. Find correct/acceptable path for the car according to car's arguments and the previous image
   * Notice, the input is a top-view image, which means we will not do undistort and perspective wrap at this point.
   */
  cv::Mat edge;
  find_edge(Input, edge);
#if DEBUG_LEVEL >= 10
  std::cout << "Info of edge: " << edge.size() << ", " << edge.type() << std::endl;
#endif
#if 0 && DEBUG_VISION
  // For debug, show the edge
  cv::Mat cedge;  //covered the original image
  cedge.create(Input.size(), Input.type());
  cedge = cv::Scalar::all(0);
  Input.copyTo(cedge, edge);
  cv::namedWindow("Image Boundary", cv::WINDOW_NORMAL);
  cv::resizeWindow("Image Boundary", 1000, 1000);
  cv::imshow("Image Boundary", edge); //show the boundary
#endif

  /*
   * Set initial window. We will slide the window to find all possible areas to drive on
   */
  int window_left, window_right;
  find_init_lane(edge, window_left, window_right);

  std::vector<cv::Point> left_lane, right_lane;
  // Find left boundary first
  find_lane(edge, window_left, left_lane);
  // Then right boundary
  find_lane(edge, window_right, right_lane);

#if 0 && DEBUG_VISION
  cv::Mat cimage;
  Input.copyTo(cimage);
  for (cv::Point i : left_lane) {
    cv::circle(cimage, i, 3, cv::Scalar(0, 255, 0), 2);
  }
  for (cv::Point i : right_lane) {
    cv::circle(cimage, i, 3, cv::Scalar(0, 0, 255), 2);
  }
  cv::namedWindow("Road Boundary", cv::WINDOW_NORMAL);
  cv::resizeWindow("Road Boundary", 1000, 1000);
  cv::imshow("Road Boundary", cimage);
#endif

  // Now we have got all necessary parts. Let's try to make a polygon representing the whole image and the driving road
  std::reverse(right_lane.begin(), right_lane.end());
  std::vector<cv::Point> road = left_lane;
  road.insert(road.end(), right_lane.begin(), right_lane.end());
  const cv::Point *croad[] = {road.data()};
  int nroad[] = {road.size()};

  // Fill into the poly for futher changes
  cv::fillPoly(Output, croad, nroad, 1, cv::Scalar(255), cv::LINE_8);

}

