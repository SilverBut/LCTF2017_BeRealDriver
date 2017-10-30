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
  std::string MapOffset(argv[3]);
  std::string OpOffset(argv[4]);

  // TODO Use methods in this link to put those file into the executable file
  // TODO  in order to add some complexity:
  // TODO  https://stackoverflow.com/questions/4864866/c-c-with-gcc-statically-add-resource-files-to-executable-library
#if DEBUG_LEVEL >= 10
  std::cout << "!!! Debug Hint: Will not use external arguments. Will use hard-code files." << std::endl;

  // Add address for example file
  std::string MapSourceFile("../examples/test-9.jpg");
  std::string MapOffsetFile("../examples/test-9-8-xor.jpg");
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


  // Read file
  cv::Mat image_original;
  image_original = cv::imread(MapSourceFile);
  assert(!image_original.empty());

  cv::Mat road;
  road.create(image_original.size(), CV_8U);
  find_road(image_original, road);
  assert(!road.empty());
#if DEBUG_VISION
  cv::namedWindow("Source Road", cv::WINDOW_NORMAL);
  cv::resizeWindow("Source Road", 1000, 1000);
  cv::imshow("Source Road", road);
#endif

#if DEBUG_VISION
  cv::waitKey(0);
#endif

  return 0;
}

