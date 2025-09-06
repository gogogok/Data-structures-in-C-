#ifndef SHARED_PTR_H
#define SHARED_PTR_H
#define  WEAK_PTR_IMPLEMENTED

#include <algorithm>

class BadWeakPtr : public std::runtime_error {
public:
  BadWeakPtr() : std::runtime_error("BadWeakPtr") {
  }
};


struct Counter {
  size_t strong_counter_ = 0;
  size_t weak_counter_ = 0;

  ~Counter() = default;

  Counter() = default;

  Counter(const size_t strong_counter, const size_t weak_counter) {
    strong_counter_ = strong_counter;
    weak_counter_ = weak_counter;
  }

  Counter(const Counter &other) {
    strong_counter_ = other.strong_counter_;
    weak_counter_ = other.weak_counter_;
  }

  Counter(Counter &&other) noexcept
    : strong_counter_(other.strong_counter_),
      weak_counter_(other.weak_counter_) {
    other.strong_counter_ = 0;
    other.weak_counter_ = 0;
  }

  Counter &operator=(Counter &&other) noexcept {
    if (this != &other) {
      strong_counter_ = other.strong_counter_;
      weak_counter_ = other.weak_counter_;
      other.strong_counter_ = 0;
      other.weak_counter_ = 0;
    }
    return *this;
  }

  Counter &operator=(const Counter &other) {
    if (this != &other) {
      strong_counter_ = other.strong_counter_;
      weak_counter_ = other.weak_counter_;
    }
    return *this;
  }
};

template<typename T>
class WeakPtr;

template<class T>
class SharedPtr {
  friend class WeakPtr<T>;
  T *ptr_ = nullptr;
  Counter *counter_ = nullptr;

  void Clean() {
    if (counter_) {
      if (counter_->strong_counter_!=0) {
        counter_->strong_counter_--;
        if (counter_->strong_counter_ == 0) {
          delete ptr_;
          if (counter_->weak_counter_ == 0) {
            delete counter_;
          }
        }
      }
      ptr_ = nullptr;
      counter_ = nullptr;
    }
  }

public:
  SharedPtr() = default;

  explicit SharedPtr(T *ptr) : ptr_(ptr) {
    if (ptr) {
      counter_ = new Counter{1, 0};
    }
  }

  SharedPtr(const SharedPtr &other) {
    ptr_ = other.ptr_;
    counter_ = other.counter_;
    if (counter_) {
      if (counter_->strong_counter_ != 0) {
        counter_->strong_counter_++;
      }
    }
  }

  SharedPtr(SharedPtr &&other) noexcept {
    ptr_ = other.ptr_;
    counter_ = other.counter_;
    other.ptr_ = nullptr;
    other.counter_ = nullptr;
  }

  explicit SharedPtr(const WeakPtr<T> &weak) {
    if (weak.Expired()) {
      throw BadWeakPtr();
    }
    counter_ = weak.counter_;
    ptr_ = weak.ptr_;
    if (counter_->strong_counter_) {
      counter_->strong_counter_++;
    }
  }

  ~SharedPtr() {
    Clean();
  }

  SharedPtr &operator=(const SharedPtr &other) {
    if (this != &other) {
      if (ptr_) {
        Clean();
      }
      ptr_ = other.ptr_;
      counter_ = other.counter_;
      if (counter_) {
        if (counter_->strong_counter_ != 0) {
          counter_->strong_counter_++;
        }
      }
    }
    return *this;
  }

  SharedPtr &operator=(SharedPtr &&other) noexcept {
    if (this != &other) {
      if (ptr_) {
        Clean();
      }
      ptr_ = other.ptr_;
      counter_ = other.counter_;
      other.ptr_ = nullptr;
      other.counter_ = nullptr;
    }
    return *this;
  }

  void Reset(T *p = nullptr) {
    Clean();
    ptr_ = p;
    if (ptr_) {
      counter_ = new Counter(1, 0);
    }
  }

  void Swap(SharedPtr &other) {
    SharedPtr temp = std::move(*this);
    *this = std::move(other);
    other = std::move(temp);
  }

  T *Get() const {
    return ptr_;
  }


  [[nodiscard]] size_t UseCount() const {
    if (counter_) {
      return counter_->strong_counter_;
    }
    return 0;
  }

  T &operator*() const {
    return *ptr_;
  }

  T *operator->() const {
    return ptr_;
  }

  explicit operator bool() const {
    return ptr_ != nullptr;
  }
};

template<class T>
class WeakPtr {
  friend class SharedPtr<T>;
  T *ptr_ = nullptr;
  Counter *counter_ = nullptr;

  void Clean() {
    if (counter_) {
      if (counter_->weak_counter_ != 0) {
        counter_->weak_counter_--;
        if (counter_->strong_counter_ == 0) {
            delete counter_;
          }
      }
      ptr_ = nullptr;
      counter_ = nullptr;
    }
  }

public:
  WeakPtr() = default;

  explicit WeakPtr(T *ptr) : ptr_(ptr) {
    if (ptr) {
      counter_ = new Counter{0, 1};
    }
  }

  ~WeakPtr() {
    Clean();
  }

  WeakPtr(const WeakPtr &other) {
    ptr_ = other.ptr_;
    counter_ = other.counter_;
    if (counter_) {
      if (counter_->weak_counter_ != 0) {
        counter_->weak_counter_++;
      }
    }
  }

  WeakPtr(WeakPtr &&other) noexcept {
    ptr_ = other.ptr_;
    counter_ = other.counter_;
    other.ptr_ = nullptr;
    other.counter_ = nullptr;
  }


  WeakPtr &operator=(const WeakPtr &other) {
    if (this != &other) {
      if (ptr_) {
        Clean();
      }
      ptr_ = other.ptr_;
      counter_ = other.counter_;
      if (counter_) {
        if (counter_->weak_counter_ != 0) {
          counter_->weak_counter_++;
        }
      }
    }
    return *this;
  }

  WeakPtr &operator=(WeakPtr &&other) noexcept {
    if (this != &other) {
      if (ptr_) {
        Clean();
      }
      ptr_ = other.ptr_;
      counter_ = other.counter_;
      other.ptr_ = nullptr;
      other.counter_ = nullptr;
    }
    return *this;
  }

  WeakPtr &operator=(SharedPtr<T> &&other) noexcept {
    if (ptr_) {
      Clean();
    }
    counter_ = other.counter_;
    ptr_ = other.ptr_;
    if (counter_) {
      counter_->weak_counter_++;
    }
    return *this;
  }

  WeakPtr &operator=(const SharedPtr<T> &other) {
    if (ptr_) {
      Clean();
    }
    counter_ = other.counter_;
    ptr_ = other.ptr_;
    if (counter_) {
      counter_->weak_counter_++;
    }
    return *this;
  }

   WeakPtr(const SharedPtr<T> &shared) { //NOLINT
    ptr_ = shared.ptr_;
    counter_ = shared.counter_;
    if (counter_) {
      if (counter_->weak_counter_ != 0) {
        counter_->weak_counter_++;
      } else {
        counter_->weak_counter_ = 1;
      }
    }
  }

  void Swap(WeakPtr &other) {
    SharedPtr temp = std::move(*this);
    *this = std::move(other);
    other = std::move(temp);
  }

  void Reset() {
    Clean();
  }

  [[nodiscard]] size_t UseCount() const {
    if (counter_) {
      return counter_->strong_counter_;
    }
    return 0;
  }

  [[nodiscard]] bool Expired() const {
    if (counter_) {
      return counter_->strong_counter_ == 0;
    }
    return true;
  }

  SharedPtr<T> Lock() const {
    if (Expired()) {
      return SharedPtr<T>();
    }
    return SharedPtr<T>(*this);
  }
};

template<typename T, typename... Args>
SharedPtr<T> MakeShared(Args &&... args) {
  return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#endif //SHARED_PTR_H
