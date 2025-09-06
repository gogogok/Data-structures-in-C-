#ifndef VECTOR_H
#define VECTOR_H
#define VECTOR_MEMORY_IMPLEMENTED

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
      array_ = static_cast<T *>(::operator new(size_ * sizeof(T)));
      try {
        std::uninitialized_value_construct_n(array_, size_);
      } catch (...) {
        ::operator delete(array_);
        array_ = nullptr;
        size_ = 0;
        capacity_ = 0;
        throw;
      }
    }
  }

  explicit Vector(SizeType size, ConstReference value) : size_(size), capacity_(size) {
    if (size > 0) {
      array_ = static_cast<T *>(::operator new(size_ * sizeof(T)));
      try {
        std::uninitialized_fill_n(array_, size, value);
      } catch (...) {
        ::operator delete(array_);
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
    try {
      if (size_ > 0) {
        array_ = static_cast<T *>(::operator new(size_ * sizeof(T)));
        std::uninitialized_copy(first, last, array_);
      }
    } catch (...) {
      ::operator delete(array_);
      array_ = nullptr;
      size_ = 0;
      capacity_ = 0;
      throw;
    }
  }

  Vector(std::initializer_list<ValueType> init) : size_(init.size()), capacity_(size_), array_(nullptr) {
    if (size_ != 0 && init.begin() != init.end()) {
      array_ = static_cast<T *>(::operator new(size_ * sizeof(T)));
      try {
        std::uninitialized_copy(init.begin(), init.end(), array_);
      } catch (...) {
        ::operator delete(array_);
        array_ = nullptr;
        size_ = 0;
        capacity_ = 0;
        throw;
      }
    }
  }

  Vector(const Vector &other) : size_(other.size_), capacity_(other.capacity_) {
    if (capacity_ > 0) {
      array_ = static_cast<T *>(::operator new(size_ * sizeof(T)));
      try {
        std::uninitialized_copy(other.begin(), other.end(), array_);
      } catch (...) {
        ::operator delete(array_);
        array_ = nullptr;
        size_ = 0;
        capacity_ = 0;
        throw;
      }
    }
  }

  Vector(Vector &&other) noexcept {
    array_ = other.array_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    other.size_ = 0;
    other.capacity_ = 0;
    other.array_ = nullptr;
  }

  Vector &operator=(const Vector &other) {
    if (this != &other) {
      if (other.capacity_ == 0) {
        std::destroy_n(array_, size_);
        ::operator delete(array_);
        array_ = nullptr;
        capacity_ = 0;
        size_ = 0;
      } else {
        Vector temp(other);
        Swap(temp);
      }
    }
    return *this;
  }

  Vector &operator=(Vector &&other) noexcept {
    if (this != &other) {
      std::destroy_n(array_, size_);
      ::operator delete(array_);
      array_ = other.array_;
      size_ = other.size_;
      capacity_ = other.capacity_;
      other.size_ = 0;
      other.capacity_ = 0;
      other.array_ = nullptr;
    }
    return *this;
  }

  ~Vector() {
    Clear();
    ::operator delete(array_);
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
    std::destroy(array_, array_ + size_);
    size_ = 0;
  }

  void Resize(const SizeType new_size) {
    if (new_size == 0) {
      std::destroy_n(array_, size_);
      size_ = 0;
      return;
    }
    if (new_size <= size_) {
      std::destroy(array_ + new_size, array_ + size_);
      size_ = new_size;
      return;
    }
    if (new_size > size_ && new_size <= capacity_) {
      std::uninitialized_value_construct_n(array_ + size_, new_size - size_);
      size_ = new_size;
      return;
    }
    Pointer new_array = nullptr;
    SizeType constructed = 0;
    if (new_size > 0) {
      try {
        new_array = static_cast<T *>(::operator new(new_size * sizeof(T)));
        for (; constructed < size_; ++constructed) {
          new (new_array + constructed) T(std::move_if_noexcept(array_[constructed]));
        }
        std::uninitialized_value_construct_n(new_array + size_, new_size - size_);
        std::destroy(array_, array_ + size_);
        ::operator delete(array_);
        array_ = new_array;
        size_ = new_size;
        capacity_ = new_size;
      } catch (...) {
        std::destroy_n(new_array, constructed);
        ::operator delete(new_array);
        throw;
      }
    }
  }

  void Resize(const SizeType new_size, ConstReference value) {
    if (new_size == 0) {
      std::destroy_n(array_, size_);
      size_ = 0;
      return;
    }
    if (new_size <= size_) {
      std::destroy(array_ + new_size, array_ + size_);
      size_ = new_size;
    } else if (new_size > size_ && new_size <= capacity_) {
      std::uninitialized_fill_n(array_ + size_, new_size - size_, value);
      size_ = new_size;
    } else {
      Pointer new_array = nullptr;
      SizeType constructed = 0;
      if (new_size > 0) {
        try {
          new_array = static_cast<T *>(::operator new(new_size * sizeof(T)));
          for (; constructed < size_; ++constructed) {
            new (new_array + constructed) T(std::move_if_noexcept(array_[constructed]));
          }
          std::uninitialized_fill_n(new_array + size_, new_size - size_, value);
          std::destroy(array_, array_ + size_);
          ::operator delete(array_);
          array_ = new_array;
          size_ = new_size;
          capacity_ = new_size;
        } catch (...) {
          std::destroy_n(new_array, constructed);
          ::operator delete(new_array);
          throw;
        }
      } else {
        Clear();
        std::destroy(array_, array_ + size_);
        ::operator delete(array_);
        array_ = nullptr;
        size_ = 0;
        capacity_ = 0;
      }
    }
  }


  void Reserve(const SizeType new_capacity) {
    if (new_capacity <= capacity_) {
      return;
    }
    Pointer new_array = nullptr;
    SizeType constructed = 0;
    try {
      new_array = static_cast<T *>(::operator new(new_capacity * sizeof(T)));
      for (; constructed < size_; ++constructed) {
        new (new_array + constructed) T(std::move_if_noexcept(array_[constructed]));
      }
      capacity_ = new_capacity;
      std::destroy(array_, array_ + size_);
      ::operator delete(array_);
      array_ = new_array;
    } catch (...) {
      std::destroy(new_array, new_array + size_);
      ::operator delete(new_array);
      throw;
    }
  }


  void ShrinkToFit() {
    if (size_ == 0) {
      ::operator delete(array_);
      array_ = nullptr;
      size_ = 0;
      capacity_ = 0;
      return;
    }
    Pointer new_array = nullptr;
    SizeType constructed = 0;
    try {
      new_array = static_cast<T *>(::operator new(size_ * sizeof(T)));
      for (; constructed < size_; ++constructed) {
        new (new_array + constructed) T(std::move_if_noexcept(array_[constructed]));
      }
      std::destroy(array_, array_ + size_);
      ::operator delete(array_);
      array_ = new_array;
      capacity_ = size_;
    } catch (...) {
      std::destroy(new_array, new_array + constructed);
      ::operator delete(new_array);
      throw;
    }
  }

  void PushBack(ConstReference value) {
    if (size_ < capacity_) {
      std::construct_at(array_ + size_, value);
      ++size_;
      return;
    }
    Pointer new_array = nullptr;
    SizeType constructed = 0;
    try {
      const SizeType cap = capacity_ == 0 ? 1 : capacity_ * 2;
      new_array = static_cast<T *>(::operator new(cap * sizeof(T)));
      for (; constructed < size_; ++constructed) {
        new (new_array + constructed) T(std::move(array_[constructed]));
      }
      std::construct_at(new_array + size_, value);
      std::destroy(array_, array_ + size_);
      ::operator delete(array_);
      array_ = new_array;
      ++size_;
      capacity_ = cap;
    } catch (...) {
      std::destroy(new_array, new_array + constructed);
      ::operator delete(new_array);
      throw;
    }
  }

  void PushBack(T &&value) {
    if (size_ < capacity_) {
      std::construct_at(array_ + size_, std::move(value));
      ++size_;
      return;
    }
    Pointer new_array = nullptr;
    SizeType constructed = 0;
    try {
      const SizeType cap = capacity_ == 0 ? 1 : capacity_ * 2;
      new_array = static_cast<T *>(::operator new(cap * sizeof(T)));
      for (; constructed < size_; ++constructed) {
        new (new_array + constructed) T(std::move(array_[constructed]));
      }
      std::construct_at(new_array + size_, std::move(value));
      std::destroy(array_, array_ + size_);
      ::operator delete(array_);
      array_ = new_array;
      ++size_;
      capacity_ = cap;
    } catch (...) {
      std::destroy(new_array, new_array + constructed);
      ::operator delete(new_array);
      throw;
    }
  }

  void PopBack() {
    if (size_ > 0) {
      std::destroy(array_ + size_ - 1, array_ + size_);
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

  Iterator begin() {    //NOLINT
    return array_;
  }

  ConstIterator begin() const {    //NOLINT
    return array_;
  }

  ConstIterator cbegin() const {    //NOLINT
    return array_;
  }

  Iterator end() {    //NOLINT
    return array_ + size_;
  }

  ConstIterator end() const {    //NOLINT
    return array_ + size_;
  }

  ConstIterator cend() const {    //NOLINT
    return array_ + size_;
  }

  ReverseIterator rbegin() {    //NOLINT
    return ReverseIterator(end());
  }

  ReverseIterator rend() {    //NOLINT
    return ReverseIterator(begin());
  }

  ConstReverseIterator crbegin() const {    //NOLINT
    return ConstReverseIterator(end());
  }

  ConstReverseIterator crend() const {    //NOLINT
    return ConstReverseIterator(begin());
  }

  ConstReverseIterator rbegin() const {    //NOLINT
    return ConstReverseIterator(end());
  }

  ConstReverseIterator rend() const {    //NOLINT
    return ConstReverseIterator(begin());
  }

  template<typename... Args>
  void EmplaceBack(Args &&... args) {
    if (size_ < capacity_) {
      new(array_ + size_) T(std::forward<Args>(args)...);
      ++size_;
      return;
    }
    Pointer new_array = nullptr;
    SizeType constructed = 0;
    try {
      const SizeType cap = capacity_ == 0 ? 1 : capacity_ * 2;
      new_array = static_cast<T *>(::operator new(cap * sizeof(T)));
      for (; constructed < size_; ++constructed) {
        new (new_array + constructed) T(std::move_if_noexcept(array_[constructed]));
      }
      std::destroy(array_, array_ + size_);
      ::operator delete(array_);
      array_ = new_array;
      capacity_ = cap;
    } catch (...) {
      std::destroy(new_array, new_array + size_);
      ::operator delete(new_array);
      throw;
    }
    new(array_ + size_) T(std::forward<Args>(args)...);
    ++size_;
  }
};

#endif
