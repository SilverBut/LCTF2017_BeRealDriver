#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <cassert>

//Include things for OpenCV
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <numeric>

#include "config.hpp"
#include "lane_finder.hpp"
#include "Resource.hpp"

int main(int argc, char *argv[]) {

  std::cout << "Welcome to LCTF 2017 Old Driver Platform!" << std::endl;

#if DEBUG_LEVEL >= 10
  std::cout << "!!! UNDER DEBUG MODE !!!" << std::endl;
#endif

#if DEBUG || DEBUG_SOURCE_FILE
  if (argc != 6 + 1) {
#else
  if (argc != 4 + 1) {
#endif
    std::cout << "Seems incorrect parameters are used?" << std::endl;
#if !((DEBUG || DEBUG_SOURCE_FILE) && (DEBUG_LEVEL >= 10))
    return EINVAL;
#endif
  }

  std::string Token(argv[1]);
  std::string Date(argv[2]);
  std::string MapOffset(argv[3]);
  std::string OpOffset(argv[4]);
#if DEBUG_SOURCE_FILE
  std::string MapSourceFile(argv[5]);
  std::string OpSourceFile(argv[6]);
#endif

  // TODO Use methods in this link to put those file into the executable file
  // TODO  in order to add some complexity:
  // TODO  https://stackoverflow.com/questions/4864866/c-c-with-gcc-statically-add-resource-files-to-executable-library
#if DEBUG_SOURCE_FILE
  std::cout << "!!! Debug Hint: Notice the files!" << std::endl;
  // Add address for example file
//  MapSourceFile = "../examples/road.jpg";
//  MapOffset = "../examples/road-cir-xor.jpg";
//  OpSourceFile = "../examples/op.jpg";
//  OpOffset = "../examples/op-cir-xor.jpg";
#endif

  std::cout << "Input parameters: " << std::endl;
  std::cout << " - Token:         " << Token << std::endl;
  std::cout << " - Date:          " << Date << std::endl;
  std::cout << " - MapOffsetFile: " << MapOffset << std::endl;
  std::cout << " - OpOffsetFIle:  " << OpOffset << std::endl;
#if DEBUG_SOURCE_FILE
  std::cout << " - MapSourceFile: " << MapSourceFile << std::endl;
  std::cout << " - OpSourceFile:  " << OpSourceFile << std::endl;
#endif

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
#if DEBUG_SOURCE_FILE
  image_original = cv::imread(MapSourceFile);
#else
  Resource _image_original = LOAD_RESOURCE(road_jpg);
  char *_image_original_tmp_name = std::tmpnam(nullptr);
  std::FILE *_image_original_tmp = std::fopen(_image_original_tmp_name, "wb");
  std::fwrite(_image_original.data(), _image_original.size(), 1, _image_original_tmp);
  std::fflush(_image_original_tmp);
  image_original = cv::imread(_image_original_tmp_name);
  std::fclose(_image_original_tmp);
  remove(_image_original_tmp_name);
#endif
  assert(!image_original.empty());

  // Read offset
  cv::Mat image_offset;
  image_offset = cv::imread(MapOffset);
  assert(!image_offset.empty());

  /* Generate real road */

  assert(image_offset.size() == image_original.size());
  cv::Mat image_new;
  image_new.create(image_original.size(), CV_8U);
  cv::bitwise_xor(image_original, image_offset, image_new);
//  // If we need a original image, do xor with 0.
//  cv::bitwise_xor(image_original, cv::Scalar(0), image_new);

  cv::Mat road_new;
  // Some code to get a original path and correct offset pairs
# if 0
  road_new = cv::Mat::zeros(image_original.size(), CV_8U);
  find_road(image_original, road_new);
  cv::imwrite("../examples/cv_op.jpg",road_new);
  road_new = cv::Mat::zeros(image_new.size(), CV_8U);
  find_road(image_new, road_new);
  cv::imwrite("../examples/cv_op-cir.jpg",road_new);
#endif
  road_new = cv::Mat::zeros(image_new.size(), CV_8UC1);
  find_road(image_new, road_new);
  assert(!road_new.empty());  //now road_new is the real road
#if 1 && DEBUG_VISION
  cv::namedWindow("New Road", cv::WINDOW_NORMAL);
  cv::resizeWindow("New Road", 1000, 1000);
  cv::imshow("New Road", road_new);
#endif



  // Load real path and path offset now
  cv::Mat op_original;
  cv::Mat op_offset;
  cv::Mat op_new, _op_new;
#if DEBUG_SOURCE_FILE
  op_original = cv::imread(OpSourceFile);
#else
  Resource _op_original = LOAD_RESOURCE(op_jpg);
  char *_op_original_tmp_name = std::tmpnam(nullptr);
  std::FILE *_op_original_tmp = std::fopen(_op_original_tmp_name, "wb");
  std::fwrite(_op_original.data(), _op_original.size(), 1, _op_original_tmp);
  std::fflush(_op_original_tmp);
  op_original = cv::imread(_op_original_tmp_name);
  std::fclose(_op_original_tmp);
  remove(_op_original_tmp_name);
#endif
  assert(!op_original.empty());

  op_offset = cv::imread(OpOffset);
  assert(!op_offset.empty());
  assert(op_original.size() == op_offset.size());
  assert(op_original.type() == op_offset.type());
  assert(op_original.size() == image_original.size());
  // get the real path
  _op_new = cv::Mat::zeros(op_original.size(), CV_8U);
  cv::bitwise_xor(op_original, op_offset, _op_new);
////   Same above!
//  cv::bitwise_xor(op_original, cv::Scalar(0), _op_new);
  op_new = cv::Mat::zeros(op_original.size(), CV_8UC1);
  cvtColor(_op_new, op_new, CV_BGR2GRAY);
#if 1 && DEBUG_VISION
  cv::namedWindow("New Op", cv::WINDOW_NORMAL);
  cv::resizeWindow("New Op", 1000, 1000);
  cv::imshow("New Op", op_new);
#endif

  // Make a LDW warning map
  cv::Mat ldw_good;
  assert(op_new.size() == road_new.size());
//  std::cout << _op_new.type() << "," << op_new.type() << "," << road_new.type() << std::endl;
  assert(op_new.type() == road_new.type());
  ldw_good = cv::Mat::zeros(op_new.size(), op_new.type());
  cv::bitwise_and(op_new, road_new, ldw_good);
  // Judge if area(LDW)/area(PATH)<=threshold, then disallow
  if ((double) (cv::countNonZero(ldw_good)) / (double) (cv::countNonZero(op_new)) <= ldw_warn_threshold) {
    std::cout << "Nope, you are not a driver at all. Do u have a license? Crack one!" << std::endl;
    return EAGAIN;
  }

  // load crash_areas
  cv::Mat crash_detector;
  cv::Mat crash_result;

  // Judge if any crash happened
  for (const uint16_t *crash_area : crash_areas_initial_value) {
#if DEBUG_LEVEL >= 10
    std::cout << "crash_area is (" << crash_area[0] << ", " << crash_area[1] << ", " << crash_area[2] << ", "
              << crash_area[3] << ")" << std::endl;
#endif
    crash_detector = cv::Mat::zeros(road_new.size(), road_new.type());
    crash_result = cv::Mat::zeros(road_new.size(), road_new.type());
    // Fill a poly according to the arguments
    std::vector<cv::Point> _crash_rect;
    _crash_rect.emplace_back(cv::Point(crash_area[0], crash_area[1]));
    _crash_rect.emplace_back(cv::Point(crash_area[0] + crash_area[2], crash_area[1]));
    _crash_rect.emplace_back(cv::Point(crash_area[0] + crash_area[2], crash_area[1] + crash_area[3]));
    _crash_rect.emplace_back(cv::Point(crash_area[0], crash_area[1] + crash_area[3]));
    const cv::Point *crash_rect[] = {_crash_rect.data()};
    int crash_rect_count[] = {static_cast<int>(_crash_rect.size())};
    cv::fillPoly(crash_detector, crash_rect, crash_rect_count, 1, cv::Scalar(255));
# if 0 && DEBUG_VISION
    cv::namedWindow("Detector Block", cv::WINDOW_NORMAL);
    cv::resizeWindow("Detector Block", 1000, 1000);
    cv::imshow("Detector Block", crash_detector);
    cv::waitKey(0);
#endif
    cv::bitwise_and(crash_detector, op_new, crash_result);
# if 1 && DEBUG_VISION
    cv::namedWindow("Detector Result", cv::WINDOW_NORMAL);
    cv::resizeWindow("Detector Result", 1000, 1000);
    cv::imshow("Detector Result", crash_result);
    cv::waitKey(0);
#endif
#if DEBUG_LEVEL >= 10
    std::cout << "Val " << ((double) (cv::countNonZero(crash_result)) / (double) (cv::countNonZero(crash_detector)))
              << " with std " << (double(crash_area[4]) / 100) << std::endl;
#endif
    if ((double) (cv::countNonZero(crash_result)) / (double) (cv::countNonZero(crash_detector))
        <= (double(crash_area[4]) / 100)) {
      std::cout << "You are a driver, but you can not control it well. You are FIRED!" << std::endl;
      return EAGAIN;
    }
  }

  // If comes here, means everything is okay. Read the flag
  std::cout << flag << std::endl;

#if 0 && DEBUG_VISION
  cv::waitKey(0);
#endif

  return 0;
}

