#ifndef RANGE_H
#define RANGE_H
#include "ObjectForRange.h"
#define REVERSE_RANGE_IMPLEMENTED

template<class T>
ObjectForRange<T> Range(T end) {
  return ObjectForRange<T>(end);
}

template<class T>
ObjectForRange<T> Range(T begin, T end) {
  return ObjectForRange<T>(begin, end);
}

template<class T>
ObjectForRange<T> Range(T begin, T end, T step) {
  return ObjectForRange<T>(begin, end, step);
}

#endif
