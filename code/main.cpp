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
#include "config.hpp"
#include "lane_finder.hpp"

int main(int argc, char *argv[]) {

  std::cout << "Welcome to LCTF 2017 Old Driver Platform!" << std::endl;

#if DEBUG_LEVEL >= 10
  std::cout << "!!! UNDER DEBUG MODE !!!" << std::endl;
#endif

  if (argc != 6 + 1) {
    std::cout << "Seems incorrect parameters are used?" << std::endl;
#if DEBUG_LEVEL >= 10
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
#if DEBUG_LEVEL >= 10
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

#if DEBUG_LEVEL >= 10
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

  cv::Mat edge;
  find_edge(image, edge);
#if DEBUG_LEVEL >= 10
  std::cout << "Info of edge: " << edge.size() << ", " << edge.type() << std::endl;
#endif
#if DEBUG_VISION
  // For debug, show the edge
  cv::Mat cedge;  //covered the original image
  cedge.create(image.size(), image.type());
  cedge = cv::Scalar::all(0);
  image.copyTo(cedge, edge);
  cv::namedWindow("High", cv::WINDOW_NORMAL);
  cv::resizeWindow("High", 1000, 1000);
  cv::imshow("High", edge); //show the boundary
#endif

  int window_left, window_right;
  find_init_lane(edge, window_left, window_right);

  /*
   * Now initial window is set. We will slide the window to find all possible areas to drive on
   */
  std::vector<cv::Point> left_lane, right_lane;
  // Find left boundary first
  find_lane(edge, window_left, left_lane);
  // Then right boundary
  find_lane(edge, window_right, right_lane);
#if DEBUG_VISION
  for (cv::Point i : left_lane) {
    cv::circle(image, i, 3, cv::Scalar(0, 255, 0), 2);
  }
  for (cv::Point i : right_lane) {
    cv::circle(image, i, 3, cv::Scalar(0, 0, 255), 2);
  }
  cv::namedWindow("Lanes", cv::WINDOW_NORMAL);
  cv::resizeWindow("Lanes", 1000, 1000);
  cv::imshow("Lanes", image);
#endif

#ifdef DEBUG_VISION
  cv::waitKey(0);
#endif

  return 0;
}

