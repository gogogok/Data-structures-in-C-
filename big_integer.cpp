#include "big_integer.h"
#include <string>
#include <ostream>
#include <iostream>
#include <cstdint>
#include <cstring>

BigInteger::BigInteger() {
  digits_.push_back(0);
};

BigInteger::BigInteger(const char *value) {
  if (value == nullptr) {
    digits_.push_back(0);
    return;
  }
  uint32_t i = 0;
  if (value[0] == '-') {
    sign_ = true;
    i = 1;
  } else if (value[0] == '+') {
    i = 1;
  }
  const size_t len = std::strlen(value);
  int64_t l = 0;
  for (int64_t j = static_cast<int64_t>(len) - 1; j >= static_cast<int64_t>(i); j -= rank_) {
    uint32_t one_part = 0;
    uint32_t pow10 = 1;
    l = std::max<int64_t>(i, j - rank_ + 1);
    for (int64_t k = j; k >= l; --k) {
      one_part += static_cast<uint32_t>(value[k] - '0') * pow10;
      pow10 *= 10;
    }
    digits_.push_back(one_part);
  }
  WithoutZero();
}

BigInteger::BigInteger(const BigInteger &other) {
  sign_ = other.sign_;
  digits_ = other.digits_;
}


bool BigInteger::IsNegative() const {
  return sign_;
}

BigInteger &BigInteger::operator=(const BigInteger &other) {
  if (this != &other) {
    sign_ = other.sign_;
    digits_ = other.digits_;
  }
  return *this;
}


BigInteger BigInteger::operator+() const {
  return *this;
}

BigInteger BigInteger::operator-() const {
  BigInteger result = *this;
  result.sign_ = !sign_;
  return result;
}

BigInteger BigInteger::operator+(const BigInteger &other) const {
  BigInteger result;
  if (sign_ == other.sign_) {
    result.sign_ = sign_;
    result.digits_.resize(std::max(digits_.size(), other.digits_.size()));
    uint64_t digit = 0;
    uint64_t other_digit = 0;
    uint64_t sum = 0;
    uint32_t mod = 0;
    uint64_t elem = 0;
    for (uint32_t i = 0; i < result.digits_.size(); i++) {
      digit = i < digits_.size() ? digits_[i] : 0;
      other_digit = i < other.digits_.size() ? other.digits_[i] : 0;
      sum = digit + other_digit + mod;
      elem = sum % base_;
      mod = static_cast<uint32_t>(sum / base_);
      result.digits_[i] = static_cast<uint32_t>(elem);
    }
    if (mod == 1) {
      result.digits_.push_back(1);
    }
    result.WithoutZero();
    if (result.IsOverflow()) {
      throw BigIntegerOverflow{};
    }
  } else {
    if (other.sign_) {
      result = *this - (-other);
    } else {
      result = other - (-*this);
      result.sign_ = !other.sign_;
      result.WithoutZero();
      if (result.IsOverflow()) {
        throw BigIntegerOverflow{};
      }
    }
  }
  return result;
}

BigInteger operator+(const BigInteger &lhs, const int64_t rhs) {
  return lhs + BigInteger(rhs);
}

BigInteger operator+(const int64_t lhs, const BigInteger &rhs) {
  return BigInteger(lhs) + rhs;
}

BigInteger operator-(const BigInteger &lhs, const int64_t rhs) {
  return lhs - BigInteger(rhs);
}

BigInteger operator-(const int64_t lhs, const BigInteger &rhs) {
  return BigInteger(lhs) - rhs;
}

BigInteger &operator+=(BigInteger &lhs, const int64_t rhs) {
  return lhs += BigInteger(rhs);
}

BigInteger &operator-=(BigInteger &lhs, const int64_t rhs) {
  return lhs -= BigInteger(rhs);
}

BigInteger BigInteger::operator-(const BigInteger &other) const {
  BigInteger result;
  result.digits_.resize(std::max(digits_.size(), other.digits_.size()));
  if (sign_ != other.sign_) {
    result.sign_ = sign_;
    uint64_t digit = 0;
    uint64_t other_digit = 0;
    uint64_t sum = 0;
    uint32_t mod = 0;
    uint32_t elem = 0;
    for (uint32_t i = 0; i < result.digits_.size(); i++) {
      digit = i < digits_.size() ? digits_[i] : 0;
      other_digit = i < other.digits_.size() ? other.digits_[i] : 0;
      sum = digit + other_digit + mod;
      elem = static_cast<uint32_t>(sum % base_);
      mod = static_cast<uint32_t>(sum / base_);
      result.digits_[i] = elem;
    }
    if (mod == 1) {
      result.digits_.push_back(1);
    }
  } else {
    bool first_more_second = false;
    if (sign_) {
      if (*this <= other) {
        first_more_second = true;
      }
    } else {
      if (*this >= other) {
        first_more_second = true;
      }
    }
    const std::vector<uint32_t> &a = first_more_second ? digits_ : other.digits_;
    const std::vector<uint32_t> &b = first_more_second ? other.digits_ : digits_;
    uint32_t mod = 0;
    uint32_t x = 0;
    uint32_t y = 0;
    for (size_t i = 0; i < result.digits_.size(); ++i) {
      x = i < a.size() ? a[i] : 0;
      y = i < b.size() ? b[i] : 0;
      if (x < y + mod) {
        result.digits_[i] = base_ + x - y - mod;
        mod = 1;
      } else {
        result.digits_[i] = x - y - mod;
        mod = 0;
      }
    }
    result.sign_ = (first_more_second ? sign_ : !sign_);
  }
  result.WithoutZero();
  if (result.IsOverflow()) {
    throw BigIntegerOverflow{};
  }
  return result;
}

BigInteger &BigInteger::operator+=(const BigInteger &other) {
  return *this = *this + other;
}

BigInteger &BigInteger::operator-=(const BigInteger &other) {
  return *this = *this - other;
}

bool BigInteger::operator==(const BigInteger &other) const {
  if (digits_.size() == other.digits_.size() && other.sign_ == sign_) {
    for (int i = static_cast<int>(digits_.size() - 1); i >= 0; --i) {
      if (digits_[i] != other.digits_[i]) {
        return false;
      }
    }
    return true;
  }
  return false;
}

bool BigInteger::operator!=(const BigInteger &other) const {
  return !(*this == other);
}

bool BigInteger::operator>(const BigInteger &other) const {
  if (sign_ != other.sign_) {
    return !sign_;
  }
  if (digits_.size() == other.digits_.size()) {
    for (int i = static_cast<int>(digits_.size() - 1); i >= 0; --i) {
      if (digits_[i] == other.digits_[i]) {
        continue;
      }
      if (sign_) {
        return digits_[i] < other.digits_[i];
      }
      return digits_[i] > other.digits_[i];
    }
    return false;
  }
  if (digits_.size() > other.digits_.size()) {
    return !sign_;
  }
  return false;
};

bool BigInteger::operator>=(const BigInteger &other) const {
  return *this > other || *this == other;
}

bool BigInteger::operator<(const BigInteger &other) const {
  return !(*this >= other);
}

bool BigInteger::operator<=(const BigInteger &other) const {
  return !(*this > other);
}

std::ostream &operator<<(std::ostream &os, const BigInteger &rhs) {
  if (rhs.digits_.empty()) {
    os << '0';
    return os;
  }
  if (rhs.sign_) {
    os << '-';
  }
  os << rhs.digits_.back();
  uint32_t num = 0;
  uint32_t temp = 0;
  int zeros_needed = 0;
  for (int i = static_cast<int>(rhs.digits_.size()) - 2; i >= 0; --i) {
    num = rhs.digits_[i];
    zeros_needed = rhs.rank_ - 1;
    temp = num;
    while (temp /= 10) {
      zeros_needed--;
    }
    while (zeros_needed > 0) {
      os << '0';
      zeros_needed--;
    }
    os << rhs.digits_[i];
  }

  return os;
}

std::istream &operator>>(std::istream &is, BigInteger &rhs) {
  std::string s;
  is >> s;
  rhs = BigInteger(s.c_str());
  return is;
}

BigInteger &BigInteger::operator++() {
  *this += BigInteger(1);
  return *this;
}

BigInteger &BigInteger::operator--() {
  return *this -= BigInteger(1);
}

BigInteger BigInteger::operator++(int) {
  BigInteger temp = *this;
  *this += BigInteger(1);
  return temp;
}

BigInteger BigInteger::operator--(int) {
  BigInteger temp = *this;
  *this -= BigInteger(1);
  return temp;
}

BigInteger::operator bool() const {
  return !digits_.empty() && (digits_.size() != 1 || digits_[0] != 0);
}

BigInteger BigInteger::operator*(const BigInteger &other) const {
  BigInteger result;
  result.digits_.resize(digits_.size() + other.digits_.size() + 2, 0);
  result.sign_ = sign_ ^ other.sign_;
  for (uint32_t i = 0; i < digits_.size(); ++i) {
    uint64_t mod = 0;
    uint64_t curr = 0;
    uint32_t j = 0;
    while (j < other.digits_.size() || mod != 0) {
      if (i + j >= result.digits_.size()) {
        result.digits_.push_back(0);
      }
      curr = static_cast<uint64_t>(result.digits_[i + j]) +
             static_cast<uint64_t>(digits_[i]) * (j < other.digits_.size() ? other.digits_[j] : 0) + mod;
      result.digits_[i + j] = static_cast<uint32_t>(curr % base_);
      mod = curr / base_;
      ++j;
    }
  }
  result.WithoutZero();
  if (result.IsOverflow()) {
    throw BigIntegerOverflow{};
  }
  return result;
}


BigInteger &BigInteger::operator*=(const BigInteger &other) {
  return *this = *this * other;
}

BigInteger operator*(const BigInteger &lhs, const int64_t rhs) {
  return lhs * BigInteger(rhs);
}

BigInteger operator*(const int64_t lhs, const BigInteger &rhs) {
  return BigInteger(lhs) * rhs;
}

BigInteger &operator*=(BigInteger &lhs, const int64_t rhs) {
  return lhs *= BigInteger(rhs);
}

BigInteger BigInteger::operator/(const BigInteger &other) const {
  if (other == BigInteger(0)) {
    throw BigIntegerDivisionByZero{};
  }
  const BigInteger first = sign_ ? -*this : *this;
  const BigInteger second = other.sign_ ? -other : other;

  if (first < second) {
    return {0};
  }

  BigInteger result;
  result.digits_.resize(first.digits_.size());
  BigInteger cur;
  cur.digits_.clear();

  for (int i = static_cast<int>(first.digits_.size()) - 1; i >= 0; --i) {
    cur.digits_.insert(cur.digits_.begin(), first.digits_[i]);
    uint32_t left = 0;
    uint32_t right = base_ - 1;
    uint32_t x = 0;
    uint32_t mid = 0;
    while (left <= right) {
      mid = (left + right) / 2;
      if (BigInteger t = second * mid; t <= cur) {
        x = mid;
        left = mid + 1;
      } else {
        right = mid - 1;
      }
    }
    result.digits_[i] = x;
    cur = cur - second * x;
  }
  result.sign_ = sign_ ^ other.sign_;
  result.WithoutZero();
  return result;
}

BigInteger &BigInteger::operator/=(const BigInteger &other) {
  return *this = *this / other;
}

BigInteger BigInteger::operator%(const BigInteger &other) const {
  if (other == BigInteger(0)) {
    throw BigIntegerDivisionByZero{};
  }
  const BigInteger z = *this / other;
  BigInteger result = *this - z * other;
  if (result != 0 && sign_) {
    result.sign_ = true;
  }
  result.WithoutZero();
  return result;
}

BigInteger &BigInteger::operator%=(const BigInteger &other) {
  return *this = *this % other;
}

void BigInteger::WithoutZero() {
  while (digits_.size() > 1 && digits_.back() == 0) {
    digits_.pop_back();
  }
  if (digits_.size() == 1 && digits_.back() == 0) {
    sign_ = false;
  }
}

bool BigInteger::IsOverflow() const {
  constexpr size_t kMaxDecimalDigits = 30009;
  if (digits_.empty()) {
    return false;
  }
  size_t decimal_digits = (digits_.size() - 1) * rank_;
  uint32_t highest_digit = digits_.back();
  int high = 0;
  do {
    highest_digit /= 10;
    ++high;
  } while (highest_digit > 0);
  decimal_digits += high;
  return decimal_digits > kMaxDecimalDigits;
}
