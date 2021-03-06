//
// Created by bbmagnum on 10/31/17.
//

#ifndef BEREALDRIVER_RESOURCE_HPP
#define BEREALDRIVER_RESOURCE_HPP

#include <cstddef>

class Resource {
 public:
  Resource(const char *start, const char *end) : mData(start),
                                                 mSize(end - start) {}

  const char *const &data() const { return mData; }
  const size_t &size() const { return mSize; }

  const char *begin() const { return mData; }
  const char *end() const { return mData + mSize; }

 private:
  const char *mData;
  size_t mSize;
};
#define LOAD_RESOURCE(x) ([]() {                                    \
        extern const char _binary_##x##_start, _binary_##x##_end;   \
        return Resource(&_binary_##x##_start, &_binary_##x##_end);  \
    })()

#endif //BEREALDRIVER_RESOURCE_HPP
