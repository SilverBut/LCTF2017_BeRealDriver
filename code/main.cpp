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
  std::string MapSourceFile(argv[3]);
  std::string MapOffset(argv[4]);
  std::string OpSourceFile(argv[5]);
  std::string OpOffset(argv[6]);

  // TODO Use methods in this link to put those file into the executable file
  // TODO  in order to add some complexity:
  // TODO  https://stackoverflow.com/questions/4864866/c-c-with-gcc-statically-add-resource-files-to-executable-library
#if DEBUG_LEVEL >= 10
  std::cout << "!!! Debug Hint: Will not use external arguments. Will use hard-code files." << std::endl;
  // Add address for example file
  MapSourceFile = "../examples/test-9.jpg";
  MapOffset = "../examples/test-9-8-xor.jpg";
  OpSourceFile = "../examples/test-op.jpg";
  OpOffset = "../examples/test-op-offset.jpg";
#endif

  std::cout << "Input parameters: " << std::endl;
  std::cout << " - Token:         " << Token << std::endl;
  std::cout << " - Date:          " << Date << std::endl;
  std::cout << " - MapSourceFile: " << MapSourceFile << std::endl;
  std::cout << " - MapOffsetFile: " << MapOffset << std::endl;
  std::cout << " - OpSourceFile:  " << OpSourceFile << std::endl;
  std::cout << " - OpOffsetFIle:  " << OpOffset << std::endl;

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


  // Read file
  cv::Mat image_original;
  image_original = cv::imread(MapSourceFile);
  assert(!image_original.empty());

  // Read offset
  cv::Mat image_offset;
  image_offset = cv::imread(MapOffset);
  assert(!image_offset.empty());

  /* Generate real road */

  assert(image_offset.size() == image_original.size());
  cv::Mat image_new;
  image_new.create(image_original.size(), CV_8U);
//  cv::bitwise_xor(image_original, image_offset, image_new);
  cv::bitwise_xor(image_original, cv::Scalar(0), image_new);

  cv::Mat road_new;
  road_new.create(image_new.size(), CV_8U);
  find_road(image_new, road_new);
  assert(!road_new.empty());  //now road_new is the real road
#if 1 & DEBUG_VISION
  cv::namedWindow("New Road", cv::WINDOW_NORMAL);
  cv::resizeWindow("New Road", 1000, 1000);
  cv::imshow("New Road", road_new);
#endif


  // Load real path and path offset now
  cv::Mat path_original;
  cv::Mat path_offset;
  cv::Mat path_new;
  // get the real path
  cv::bitwise_xor(path_original, path_offset, path_new);

  // Make a LDW warning
  cv::Mat ldw_warn;
  cv::bitwise_and(path_new, road_new, ldw_warn);

  // Judge if area(LDW)/area(ROAD)<=threshold, then disallow
  if ((double) (cv::countNonZero(ldw_warn)) / (double) (cv::countNonZero(road_new)) <= ldw_warn_threshold) {
    std::cout << "Nope, you are not driving according to the rules." << std::endl;
  }

  // load crash_areas
  cv::Mat crash_areas[];

  // Judge if any crash happened
  for (auto crash_area : crash_areas) {
    cv::Mat crash_result;
    cv::bitwise_and(crash_area, path_new, crash_result);
    if ((double) (cv::countNonZero(crash_result)) / (double) (cv::countNonZero(crash_area)) <= crash_warn_threshold) {
      std::cout << "You are a good driver, but you can not control it well. You are FIRED!" << std::endl;
    }
  }

#if 0 && DEBUG_VISION
  cv::namedWindow("Srce Road", cv::WINDOW_NORMAL);
  cv::resizeWindow("Srce Road", 1000, 1000);
  cv::imshow("Srce Road", path_original);
#endif

#if DEBUG_VISION
  cv::waitKey(0);
#endif

  return 0;
}

