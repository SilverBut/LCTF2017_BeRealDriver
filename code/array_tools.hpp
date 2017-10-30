//
// Created by bbmagnum on 10/28/17.
//
#ifndef BEREALDRIVER_DEBUG_TOOLS_HPP_H
#define BEREALDRIVER_DEBUG_TOOLS_HPP_H

#include <numeric>
/*
 * Copy & modified from https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
 */
template<typename T>
std::vector<size_t> array_sort_descending(const std::vector<T> &v) {

  // initialize original index locations
  std::vector<size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) { return v[i1] > v[i2]; });

  return idx;
}

#ifdef  DEBUG
#endif

#endif //BEREALDRIVER_DEBUG_TOOLS_HPP_H
