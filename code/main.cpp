#include <iostream>
int main(int argc, char *argv[]) {

  std::cout << "Welcome to LCTF 2017 Old Driver Platform!" << std::endl;

#ifdef DEBUG
  std::cout << "!!! UNDER DEBUG MODE !!!" << std::endl;
#endif

  if (argc != 6+1) {
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
  std::string MapSourceFile(argv[5]);
  std::string OpSourceFile(argv[6]);

  std::cout << "Input parameters: " << std::endl;
  std::cout << " - Token:         " << Token << std::endl;
  std::cout << " - Date:          " << Date << std::endl;
  std::cout << " - MapOffsetFile: " << MapOffset << std::endl;
  std::cout << " - OpOffsetFIle:  " << OpOffset << std::endl;
  std::cout << " - MapSourceFile: " << MapSourceFile << std::endl;
  std::cout << " - OpSourceFile:  " << OpSourceFile << std::endl;


  return 0;
}