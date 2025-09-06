#ifndef VECTOR_H
#define VECTOR_H

#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <iterator>

template<class T>
class Vector {
public:
  using ValueType = T;
  using Pointer = T *;
  using ConstPointer = const T *;
  using Reference = T &;
  using ConstReference = const T &;
  using SizeType = std::size_t;
  using Iterator = Pointer;
  using ConstIterator = ConstPointer;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

private:
  SizeType size_ = 0;
  SizeType capacity_ = 0;
  Pointer array_ = nullptr;

public:
  Vector() = default;

  explicit Vector(const SizeType size) : size_(size), capacity_(size) {
    if (capacity_ > 0) {
      array_ = new ValueType[capacity_]{};
    }
  }

  explicit Vector(const SizeType size, ConstReference value) : size_(size), capacity_(size) {
    if (capacity_ > 0) {
      array_ = new ValueType[capacity_];
      try {
        std::fill(array_, array_ + size_, value);
      } catch (...) {
        delete[] array_;
        array_ = nullptr;
        size_ = 0;
        capacity_ = 0;
        throw;
      }
    }
  }

  template<class Iterator, class = std::enable_if_t<std::is_base_of_v<std::forward_iterator_tag,
    typename std::iterator_traits<Iterator>::iterator_category> > >
  explicit Vector(Iterator first, Iterator last) : size_(std::distance(first, last)), capacity_(size_) {
    if (capacity_ > 0 && first != last) {
      array_ = new ValueType[capacity_];
      try {
        std::copy(first, last, array_);
      } catch (...) {
        delete[] array_;
        array_ = nullptr;
        size_ = 0;
        capacity_ = 0;
      }
    }
  }

  Vector(std::initializer_list<ValueType> init) : size_(init.size()), capacity_(size_) {
    if (capacity_ > 0) {
      array_ = new ValueType[capacity_];
      try {
        std::copy(init.begin(), init.end(), array_);
      } catch (...) {
        delete[] array_;
        array_ = nullptr;
        size_ = 0;
        capacity_ = 0;
      }
    }
  }

  Vector(const Vector &other) : size_(other.size_), capacity_(other.capacity_) {
    if (capacity_ > 0) {
      array_ = new ValueType[capacity_];
      try {
        std::copy(other.array_, other.array_ + size_, array_);
      } catch (...) {
        delete[] array_;
        array_ = nullptr;
        size_ = 0;
        capacity_ = 0;
      }
    }
  }

  Vector(Vector &&other) noexcept : size_(std::exchange(other.size_, 0)),
                                    capacity_(std::exchange(other.capacity_, 0)),
                                    array_(std::exchange(other.array_, nullptr)) {
  }

  Vector &operator=(const Vector &other) {
    if (this != &other) {
      Vector temp(other);
      Swap(temp);
    }
    return *this;
  }

  Vector &operator=(Vector &&other) noexcept {
    if (this != &other) {
      delete[] array_;
      size_ = std::exchange(other.size_, 0);
      capacity_ = std::exchange(other.capacity_, 0);
      array_ = std::exchange(other.array_, nullptr);
    }
    return *this;
  }

  ~Vector() {
    delete[] array_;
  }

  [[nodiscard]] SizeType Size() const { return size_; }
  [[nodiscard]] SizeType Capacity() const { return capacity_; }
  [[nodiscard]] bool Empty() const { return size_ == 0; }

  const ValueType &operator[](SizeType i) const { return array_[i]; }
  ValueType &operator[](SizeType i) { return array_[i]; }

  const ValueType &At(SizeType i) const {
    if (i >= size_) {
      throw std::out_of_range("Vector::At");
    }
    return array_[i];
  }

  ValueType &At(SizeType i) {
    if (i >= size_) {
      throw std::out_of_range("Vector::At");
    }
    return array_[i];
  }

  const ValueType &Front() const {
    return array_[0];
  }

  ValueType &Front() {
    return array_[0];
  }

  const ValueType &Back() const {
    return array_[size_ - 1];
  }

  ValueType &Back() {
    return array_[size_ - 1];
  }

  ConstPointer Data() const { return array_; }
  Pointer Data() { return array_; }

  void Swap(Vector &other) {
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(array_, other.array_);
  }

  void Clear() noexcept {
    delete[] array_;
    if (capacity_ > 0) {
      array_ = new ValueType[capacity_];
    }
    else {
      array_ = nullptr;
    }
    size_ = 0;
  }

  void Resize(const SizeType new_size) {
    if (new_size <= capacity_) {
      size_ = new_size;
    } else {
      Pointer new_array = nullptr;
      if (new_size > 0) {
        try {
          new_array = new ValueType[new_size];
          std::move(array_, array_ + size_, new_array);
          for (SizeType i = size_; i < new_size; ++i) {
            new_array[i] = ValueType();
          }
          delete[] array_;
          array_ = new_array;
          size_ = new_size;
          capacity_ = new_size;
        } catch (...) {
          delete[] new_array;
          throw;
        }
      } else {
        delete[] array_;
        array_ = nullptr;
        size_ = 0;
      }
    }
  }

  void Resize(const SizeType new_size, ConstReference value) {
    if (new_size <= size_) {
      size_ = new_size;
    } else if (new_size > capacity_) {
      Pointer new_array = nullptr;
      try {
        if (new_size > 0) {
          new_array = new ValueType[new_size];
          std::move(array_, array_ + size_, new_array);
          for (SizeType i = size_; i < new_size; ++i) {
            new_array[i] = value;
          }
          delete[] array_;
          array_ = new_array;
          size_ = new_size;
          capacity_ = new_size;
        } else {
          delete[] array_;
          array_ = nullptr;
          size_ = new_size;
        }
      } catch (...) {
        delete[] new_array;
        throw;
      }
    } else {
      for (SizeType i = size_; i < new_size; ++i) {
        array_[i] = value;
      }
      size_ = new_size;
    }
  }


  void Reserve(const SizeType new_capacity) {
    if (new_capacity <= capacity_) {
      return;
    }
    ValueType *new_array = nullptr;
    try {
      new_array = new ValueType[new_capacity];
      for (SizeType i = 0; i < size_; ++i) {
        new_array[i] = std::move(array_[i]);
      }
      delete[] array_;
      array_ = new_array;
      capacity_ = new_capacity;
    } catch (...) {
      delete[] new_array;
      throw;
    }
  }


  void ShrinkToFit() {
    if (size_ == 0) {
      delete[] array_;
      array_ = nullptr;
      capacity_ = 0;
      return;
    }
    ValueType *new_array = nullptr;
    try {
      new_array = new ValueType[size_];
      for (SizeType i = 0; i < size_; ++i) {
        new_array[i] = std::move(array_[i]);
      }

      delete[] array_;
      array_ = new_array;
      capacity_ = size_;
    } catch (...) {
      delete[] new_array;
      throw;
    }
  }

  void PushBack(ConstReference value) {
    if (size_ < capacity_) {
      array_[size_] = value;
      ++size_;
      return;
    }
    Pointer new_array = nullptr;
    try {
      const SizeType cap = capacity_ == 0 ? 1 : capacity_ * 2;
      new_array = new ValueType[cap];
      for (SizeType i = 0; i < size_; ++i) {
        new_array[i] = std::move(array_[i]);
      }
      new_array[size_] = value;
      delete[] array_;
      array_ = new_array;
      ++size_;
      capacity_ = cap;
    } catch (...) {
      delete[] new_array;
      throw;
    }
  }

  void PushBack(T &&value) {
    if (size_ < capacity_) {
      array_[size_] = std::move(value);
      ++size_;
      return;
    }
    Pointer new_array = nullptr;
    try {
      const SizeType cap = capacity_ == 0 ? 1 : capacity_ * 2;
      new_array = new ValueType[cap];
      for (SizeType i = 0; i < size_; ++i) {
        new_array[i] = std::move(array_[i]);
      }
      new_array[size_] = std::move(value);
      delete[] array_;
      array_ = new_array;
      ++size_;
      capacity_ = cap;
    } catch (...) {
      delete[] new_array;
      throw;
    }
  }

  void PopBack() {
    if (size_ > 0) {
      array_[size_ - 1] = ValueType();
      --size_;
    }
  }

  bool operator==(const Vector &other) const {
    if (size_ != other.size_) {
      return false;
    }
    for (SizeType i = 0; i < size_; ++i) {
      if (array_[i] != other.array_[i]) {
        return false;
      }
    }
    return true;
  }


  bool operator!=(const Vector &other) const {
    return !(*this == other);
  }

  bool operator<(const Vector &other) const {
    for (SizeType i = 0; i < size_ && i < other.size_; ++i) {
      if (array_[i] != other.array_[i]) {
        return array_[i] < other.array_[i];
      }
    }
    return size_ < other.size_;
  }

  bool operator<=(const Vector &other) const {
    return *this < other || *this == other;
  }

  bool operator>(const Vector &other) const {
    return other < *this;
  }

  bool operator>=(const Vector &other) const {
    return other <= *this;
  }

  Iterator begin() {//NOLINT
    return array_;
  }

  ConstIterator begin() const {//NOLINT
    return array_;
  }

  ConstIterator cbegin() const {//NOLINT
    return array_;
  }

  Iterator end() {//NOLINT
    return array_ + size_;
  }

  ConstIterator end() const {//NOLINT
    return array_ + size_;
  }

  ConstIterator cend() const {//NOLINT
    return array_ + size_;
  }

  ReverseIterator rbegin() {//NOLINT
    return ReverseIterator(end());
  }

  ReverseIterator rend() {//NOLINT
    return ReverseIterator(begin());
  }

  ConstReverseIterator crbegin() const {//NOLINT
    return ConstReverseIterator(end());
  }

  ConstReverseIterator crend() const {//NOLINT
    return ConstReverseIterator(begin());
  }

  ConstReverseIterator rbegin() const {//NOLINT
    return ConstReverseIterator(end());
  }

  ConstReverseIterator rend() const {//NOLINT
    return ConstReverseIterator(begin());
  }
};

#endif
