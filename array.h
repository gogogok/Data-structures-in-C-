#ifndef ARRAY_H
#define ARRAY_H
#define ARRAY_TRAITS_IMPLEMENTED
#include <stdexcept>

class ArrayOutOfRange : public std::out_of_range {
public:
  ArrayOutOfRange() : std::out_of_range("ArrayOutOfRange") {
  }
};

template<typename T>
size_t GetSize(const T &) {
  return 0;
}

template<typename T, size_t N>
size_t GetSize(T (&)[N]) {
  return N;
}

template<typename T>
size_t GetRank(const T &) {
  return 0;
}

template<typename T, size_t N>
size_t GetRank(const T (&array)[N]) {
  return 1 + GetRank(array[0]);
}


template<typename T>
size_t GetNumElements(const T &) {
  return 1;
}

template<typename T, size_t N>
size_t GetNumElements(const T (&array)[N]) {
  size_t counter_of_elements = 0;
  for (size_t i = 0; i < N; i++) {
    counter_of_elements += GetNumElements(array[i]);
  }
  return counter_of_elements;
}


template<class T, size_t N>
class Array {
public:
  T massive[N];

  const T &operator[](const size_t idx) const {
    return massive[idx];
  }

  T &operator[](const size_t idx) {
    return massive[idx];
  }

  const T &At(const size_t idx) const {
    if (idx >= N) {
      throw ArrayOutOfRange();
    }
    return massive[idx];
  }

  T &At(const size_t idx) {
    if (idx >= N) {
      throw ArrayOutOfRange();
    }
    return massive[idx];
  }

  T &Front() {
    return massive[0];
  }

  const T &Front() const {
    return massive[0];
  }

  const T &Back() const {
    return massive[N - 1];
  }

  T &Back() {
    return massive[N - 1];
  }

  const T *Data() const {
    return &massive[0];
  }

  T *Data() {
    return &massive[0];
  }

  static size_t Size() {
    return N;
  }

  static bool Empty() {
    return N == 0;
  }

  void Fill(const T &value) {
    for (size_t i = 0; i < N; i++) {
      massive[i] = value;
    }
  }

  void Swap(Array &other) {
    for (size_t i = 0; i < N; i++) {
      std::swap(massive[i], other.massive[i]);
    }
  }
};
#endif //ARRAY_H
