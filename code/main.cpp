#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <cassert>

//Include things for opencv
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <numeric>

#include "array_tools.hpp"

int main(int argc, char *argv[]) {

  std::cout << "Welcome to LCTF 2017 Old Driver Platform!" << std::endl;

#ifdef DEBUG
  std::cout << "!!! UNDER DEBUG MODE !!!" << std::endl;
#endif

  if (argc != 6 + 1) {
    std::cout << "Seems incorrect parameters are used?" << std::endl;
#ifndef DEBUG
    return EINVAL;
#endif
  }

  std::string Token(argv[1]);
  std::string Date(argv[2]);
  std::string MapOffset(argv[3]);
  std::string OpOffset(argv[4]);

  // TODO Use methods in this link to put those file into the executable file
  // TODO  in order to add some complexity:
  // TODO  https://stackoverflow.com/questions/4864866/c-c-with-gcc-statically-add-resource-files-to-executable-library
#ifdef DEBUG
  std::cout << "!!! Debug Hint: Will not use external arguments. Will use hard-code files." << std::endl;

  // Add address for example file
  std::string MapSourceFile("../examples/test-9.jpg");
  std::string OpSourceFile("../");

#else
  std::string MapSourceFile(argv[5]);
  std::string OpSourceFile(argv[6]);
#endif

  std::cout << "Input parameters: " << std::endl;
  std::cout << " - Token:         " << Token << std::endl;
  std::cout << " - Date:          " << Date << std::endl;
  std::cout << " - MapOffsetFile: " << MapOffset << std::endl;
  std::cout << " - OpOffsetFIle:  " << OpOffset << std::endl;
  std::cout << " - MapSourceFile: " << MapSourceFile << std::endl;
  std::cout << " - OpSourceFile:  " << OpSourceFile << std::endl;

#ifdef DEBUG
  std::cout << "Debug Environment Info:" << std::endl;
  const unsigned int bufferSize = 512;
  std::vector<char> _buffer(bufferSize + 1);
  ssize_t count = readlink("/proc/self/exe", &_buffer[0], bufferSize);
  assert(count != -1);
  _buffer[count - 1] = 0;
  std::string buffer = &_buffer[0];
  std::cout << " - Current File: " << buffer << std::endl;
#endif

  /*
   * Here we will call a sample function to implement primary functions of a LDW, including:
   *    1. Read a top-view image
   *    2. Find edges and driving lanes in the image
   *    3. Find correct/acceptable path for the car according to car's arguments and the previous image
   * Notice, the input is a top-view image, which means we will not do undistort and perspective wrap at this point.
   */

  cv::Mat image;

  // Read file
  image = cv::imread(MapSourceFile);
  assert(!image.empty());

  // Pre-process it to gray-scale image
  cv::Mat gray;
  cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

  // blur and use Canny to find edge
  cv::Mat blurImage;
  cv::Mat edge;
  cv::blur(gray, blurImage, cv::Size(4, 4));
  cv::Canny(blurImage, edge, 60, 180, 3);

#ifdef DEBUG
//  // For debug, show the edge
//  cv::Mat cedge;  //covered the original image
//  cedge.create(image.size(), image.type());
//  std::cout << "Info of edge: " << edge.size() << ", " << edge.type() << std::endl;
//  cedge = cv::Scalar::all(0);
//  image.copyTo(cedge, edge);
//  cv::namedWindow("High", cv::WINDOW_NORMAL);
//  cv::resizeWindow("High", 1000, 1000);
//  cv::imshow("High", edge); //show the boundary
#endif

  // Threshold the image, and cut first 2m for a initial boundary hist test
  cv::Mat hist_input;
  cv::threshold(
      edge(cv::Range(0, 800), cv::Range(0, edge.size().width)), //input
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

#ifdef DEBUG
  // Now hist_result contains initial vector for driving lanes. Let's try it out
//  std::cout << "hist_result: " << std::endl;
//  std::cout << hist_result << std::endl;
//  int _i = 50;
//  std::vector<uint16_t> _v = hist_result;
//  std::cout << "Top " << _i << ":" << std::endl;
//  for (auto i: array_sort_descending(_v)) {
//    std::cout << i << ", " << _v[i] << "; ";
//    if (!_i--)
//      break;
//  }
//  std::cout << std::endl;
#endif
  // Do cv::threshold for hist_result again to discard polluted results
  // For this time, we make threshold value is (max()-min())/2
  double hist_max, hist_min;
  cv::minMaxIdx(hist_result, &hist_min, &hist_max);
  double hist_threshold = (hist_max - hist_min) / 2;
#ifdef DEBUG
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
#ifdef DEBUG
//  std::cout << "hist_clear: " << std::endl;
//  std::cout << hist_clear << std::endl;
#endif

  /*
   * From now, for most parameters, we can basically use uint16_t since they will not larget than 65536
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
  const uint16_t car_width = 720;
  const uint16_t car_len = 400;

  /*
   * With all those parameters, we can find the initial lane now
   */

  int left_search_point = current_pos_col - lane_space_width_max / 2;
  left_search_point = left_search_point < 0 ? 0 : left_search_point;
  int right_search_point = current_pos_col + lane_space_width_max / 2;
  right_search_point = right_search_point > hist_result.cols ? hist_result.cols : right_search_point;
  int mid_search_point = current_pos_col;

  int window_left = -1;
  for (int i = mid_search_point; i > left_search_point; i--) {
    if (hist_clear.at<float>(0, i)) {
      window_left = i;
      break;
    }
  }
  int window_right = -1;
  for (int i = mid_search_point; i < right_search_point; i++) {
    if (hist_clear.at<float>(0, i)) {
      window_right = i;
      break;
    }
  }
  assert(window_left != -1 && window_right != -1);
#ifdef DEBUG
  std::cout << "window_left = " << window_left << ", window_right = " << window_right << std::endl;
#endif

  /*
   * Now initial window is set. We will slide the window to find all possible areas to drive on
   */
  // For each lane, its normal width is between 30px to 40px
  // So we add -10px to our window, and make our detector window width is 25px
  // it should fit for most situations
  const int32_t lane_window_width = 15;
  const int32_t lane_window_offset = -10;
  const int32_t lane_window_height = 10;
  std::vector<cv::Point> left_lane, right_lane;
  // Find left boundary first
  for (int32_t window_x = window_left + lane_window_offset, window_y = current_pos_row;
       window_y + lane_window_height < edge.rows && window_y > 0 &&
           window_x > 0 && window_x + lane_window_width < edge.cols;
      ) {
#ifdef DEBUG
//    std::cout << "window_x = " << window_x << ", window_y = " << window_y << std::endl;
//    std::cout << "lane_window_width = " << lane_window_width << ", lane_window_height = " << lane_window_height
//              << std::endl;
#endif
    // for each window:
    // 0. Put current window into the vector
    left_lane.emplace_back(cv::Point(window_x, window_y));
    // 1. Get edge image in the window
    cv::Mat window_image;
    edge(
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
    cv::circle(image, window_point, 5, cv::Scalar(0, 255, 0), 2);
#endif
    // 3. Update window parameter
    window_x = window_point.x + lane_window_offset;
    window_x = window_x < 0 ? 0 :
               window_x + lane_window_width > edge.cols ? edge.cols - lane_window_width : window_x + 2;
    window_y = window_point.y;
    window_y = window_y < 0 ? 0 :
               window_y + lane_window_height > edge.rows ? edge.rows - lane_window_height : window_y + 2;
  }

  // Then right boundary
  for (int32_t window_x = window_right + lane_window_offset, window_y = current_pos_row;
       window_y + lane_window_height < edge.rows && window_y > 0 &&
           window_x > 0 && window_x + lane_window_width < edge.cols;
      ) {
#ifdef DEBUG
//    std::cout << "window_x = " << window_x << ", window_y = " << window_y << std::endl;
//    std::cout << "lane_window_width = " << lane_window_width << ", lane_window_height = " << lane_window_height
//              << std::endl;
#endif
    // for each window:
    // 0. Put current window into the vector
    right_lane.emplace_back(cv::Point(window_x, window_y));
    // 1. Get edge image in the window
    cv::Mat window_image;
    edge(
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
    cv::circle(image, window_point, 5, cv::Scalar(0, 0, 255), 2);
#endif
    // 3. Update window parameter
    window_x = window_point.x + lane_window_offset;
    window_x = window_x < 0 ? 0 :
               window_x + lane_window_width > edge.cols ? edge.cols - lane_window_width : window_x + 2;
    window_y = window_point.y;
    window_y = window_y < 0 ? 0 :
               window_y + lane_window_height > edge.rows ? edge.rows - lane_window_height : window_y + 2;
  }
#ifdef DEBUG
  cv::namedWindow("Lanes", cv::WINDOW_NORMAL);
  cv::resizeWindow("Lanes", 1000, 1000);
  cv::imshow("Lanes", image);
#endif

#ifdef DEBUG
  cv::waitKey(0);
#endif

  return 0;
}

