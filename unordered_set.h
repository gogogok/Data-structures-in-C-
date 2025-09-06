#ifndef UNORDERED_SET_H
#define UNORDERED_SET_H
#include <list>
#include <vector>
#include <algorithm>

template <class Key>
class UnorderedSet {
  std::vector<std::list<Key> > baskets_;
  size_t elements_count_ = 0;

 public:
  size_t Bucket(const Key &key) const {
    return std::hash<Key>{}(key) % baskets_.size();
  }

  [[nodiscard]] size_t BucketCount() const {
    if (baskets_.empty()) {
      return 0;
    }
    return baskets_.size();
  }

  [[nodiscard]] size_t BucketSize(size_t id) const {
    if (id < BucketCount()) {
      return baskets_[id].size();
    }
    return 0;
  }

  [[nodiscard]] float LoadFactor() const {
    if (baskets_.empty()) {
      return 0.0f;
    }
    return static_cast<float>(elements_count_) / BucketCount();
  }

  void Rehash(const size_t new_bucket_count) {
    if (new_bucket_count == BucketCount() || new_bucket_count < elements_count_) {
      return;
    }
    std::vector<std::list<Key> > new_baskets(new_bucket_count);
    for (auto bucket : baskets_) {
      auto it = bucket.begin();
      while (it != bucket.end()) {
        size_t new_index = std::hash<Key>{}(*it) % new_bucket_count;
        new_baskets[new_index].splice(new_baskets[new_index].end(), bucket, it++);
      }
    }
    baskets_ = std::move(new_baskets);
  }

  UnorderedSet() = default;

  explicit UnorderedSet(size_t bucket_count) : baskets_(bucket_count) {
  }

  template <class Forward>
  UnorderedSet(Forward first, Forward last) {
    if (first != last) {
      baskets_.resize(std::distance(first, last));
      for (auto it = first; it != last; ++it) {
        Insert(*it);
      }
    }
  }

  UnorderedSet(const UnorderedSet &other) {
    baskets_ = other.baskets_;
    elements_count_ = other.elements_count_;
  }

  UnorderedSet(UnorderedSet &&other) noexcept {
    baskets_ = std::move(other.baskets_);
    elements_count_ = other.elements_count_;
    other.elements_count_ = 0;
    other.baskets_.clear();
  }

  UnorderedSet &operator=(const UnorderedSet &other) {
    if (this != &other) {
      baskets_ = other.baskets_;
      elements_count_ = other.elements_count_;
    }
    return *this;
  }

  UnorderedSet &operator=(UnorderedSet &&other) noexcept {
    if (this != &other) {
      baskets_ = std::move(other.baskets_);
      elements_count_ = other.elements_count_;
      other.baskets_.clear();
      other.elements_count_ = 0;
    }
    return *this;
  }

  [[nodiscard]] size_t Size() const {
    return elements_count_;
  }

  [[nodiscard]] bool Empty() const {
    return elements_count_ == 0;
  }

  void Clear() {
    for (auto basket : baskets_) {
      basket.clear();
    }
    elements_count_ = 0;
  }

  bool Find(const Key &key) const {
    if (Empty()) {
      return false;
    }
    size_t index = Bucket(key);
    if (index >= baskets_.size()) {
      return false;
    }
    auto &basket = baskets_[index];
    return std::find(basket.begin(), basket.end(), key) != basket.end();
  }

  void Insert(const Key &key) {
    if (Find(key)) {
      return;
    }
    if (elements_count_ >= BucketCount()) {
      const size_t new_size = Empty() ? static_cast<size_t>(1) : BucketCount() * 2;
      Rehash(new_size);
    }
    size_t index = Bucket(key);
    baskets_[index].push_back(key);
    ++elements_count_;
  }

  void Insert(Key &&key) {
    if (Find(key)) {
      return;
    }
    if (elements_count_ >= BucketCount()) {
      const size_t new_size = Empty() ? static_cast<size_t>(1) : BucketCount() * 2;
      Rehash(new_size);
    }
    size_t index = Bucket(key);
    baskets_[index].push_back(std::move(key));
    ++elements_count_;
  }

  void Erase(const Key &key) {
    size_t index = Bucket(key);
    auto &basket = baskets_[index];
    auto it = std::find(basket.begin(), basket.end(), key);
    if (it != basket.end()) {
      basket.erase(it);
      --elements_count_;
    }
  }

  void Reserve(size_t new_bucket_count) {
    if (new_bucket_count > BucketCount()) {
      Rehash(new_bucket_count);
    }
  }
};

#endif
