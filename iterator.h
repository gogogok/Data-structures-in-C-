#ifndef ITERATOR_H
#define ITERATOR_H

template<class T>
class Iterator {
  bool flag_reverse_ = false;
  T begin_ = T{};
  T end_ = T{};
  T step_ = T{1};

public:
  Iterator(T begin, T end, T step, const bool reverse = false) {
    begin_ = begin;
    end_ = end;
    step_ = step;
    flag_reverse_ = reverse;
  }

  T operator*() const {
    return begin_;
  }

  Iterator &operator++() {
    if (flag_reverse_) {
      begin_ -= step_;
    } else {
      begin_ += step_;
    }
    return *this;
  }

  bool operator==(const Iterator &other) const {
    return begin_ == other.end_;
  }

  bool operator!=(const Iterator &other) const {
    if (flag_reverse_) {
      if (step_ >= 1) {
        return begin_ >= other.end_;
      }
      return begin_ <= other.end_;
    }
    if (step_ >= 1) {
      return begin_ < other.end_;
    }
    return begin_ > other.end_;
  }
};

#endif
