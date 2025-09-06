#ifndef OBJECT_FOR_RANGE_H
#define OBJECT_FOR_RANGE_H
#include "iterator.h"

template<class T>
class ObjectForRange {
  T begin_ = T{};
  T end_ = T{};
  T step_ = T{1};

public:
  explicit ObjectForRange(T end) {
    if (end > 0) {
      end_ = end;
    }
  }

  ObjectForRange(T begin, T end) {
    if (begin <= end) {
      begin_ = begin;
      end_ = end;
    }
  }


  ObjectForRange(T begin, T end, T step) {
    if ((begin <= end && step >= 1) || (begin > end && step < 0)) {
      begin_ = begin;
      end_ = end;
      step_ = step;
    }
  }

  Iterator<T> begin() const {    //NOLINT
    return Iterator{begin_, end_, step_};
  }

  Iterator<T> end() const {    //NOLINT
    return Iterator{end_, end_, step_};
  }

  Iterator<T> rbegin() const {    //NOLINT
    if ((end_ - begin_) % step_ != 0) {
      return Iterator{begin_ + (end_ - begin_ + step_ - 1) / step_ * step_ - step_, begin_, step_, true};
    }
    return Iterator{end_ - step_, begin_, step_, true};
  }

  Iterator<T> rend() const {    //NOLINT
    return Iterator{begin_, begin_, step_, true};
  }
};

#endif
