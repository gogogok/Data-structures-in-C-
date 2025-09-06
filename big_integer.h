#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H
#define BIG_INTEGER_DIVISION_IMPLEMENTED

#include <stdexcept>
#include <cstdint>
#include <vector>

class BigIntegerOverflow : public std::runtime_error {
public:
  BigIntegerOverflow() : std::runtime_error("BigIntegerOverflow") {
  }
};

class BigIntegerDivisionByZero : public std::runtime_error {
public:
  BigIntegerDivisionByZero() : std::runtime_error("BigIntegerDivisionByZero") {
  }
};

class BigInteger {
  const uint32_t base_ = 1000000000;
  const int rank_ = 9;
  bool sign_ = false;
  std::vector<uint32_t> digits_;

  void WithoutZero();

  [[nodiscard]] bool IsOverflow() const;

public:
  BigInteger();

  template<typename T, typename = std::enable_if_t<std::is_integral_v<T> > >
  BigInteger(T value) {//NOLINT
    if (value != 0) {
      if (value < 0) {
        sign_ = true;
        value = -value;
      }
      if (static_cast<uint64_t>(value) < base_) {
        digits_.push_back(static_cast<uint32_t>(value));
      } else {
        while (value != 0) {
          digits_.push_back(value % base_);
          value /= base_;
        }
      }
    } else {
      digits_.push_back(0);
    }
  }

  explicit BigInteger(const char *value);

  BigInteger(const BigInteger &other);

  [[nodiscard]] bool IsNegative() const;

  BigInteger operator+() const;

  BigInteger operator-() const;

  BigInteger operator+(const BigInteger &other) const;

  BigInteger operator-(const BigInteger &other) const;

  BigInteger &operator+=(const BigInteger &other);

  BigInteger &operator-=(const BigInteger &other);

  BigInteger operator*(const BigInteger &other) const;

  BigInteger &operator*=(const BigInteger &other);

  friend BigInteger operator+(const BigInteger &lhs, int64_t rhs);

  friend BigInteger operator+(int64_t lhs, const BigInteger &rhs);

  friend BigInteger operator-(const BigInteger &lhs, int64_t rhs);

  friend BigInteger operator-(int lhs, const BigInteger &rhs);

  friend BigInteger &operator+=(BigInteger &lhs, int64_t rhs);

  friend BigInteger &operator-=(BigInteger &lhs, int64_t rhs);

  friend BigInteger operator*(const BigInteger &lhs, int64_t rhs);

  friend BigInteger operator*(int lhs, const BigInteger &rhs);

  friend BigInteger &operator*=(BigInteger &lhs, int64_t rhs);

  BigInteger &operator++();

  BigInteger &operator--();

  BigInteger operator--(int);

  BigInteger operator++(int);

  explicit operator bool() const;

  bool operator==(const BigInteger &other) const;

  bool operator!=(const BigInteger &other) const;

  bool operator<(const BigInteger &other) const;

  bool operator>(const BigInteger &other) const;

  bool operator<=(const BigInteger &other) const;

  bool operator>=(const BigInteger &other) const;

  friend std::ostream &operator<<(std::ostream &os, const BigInteger &rhs);

  friend std::istream &operator>>(std::istream &is, BigInteger &rhs);

  BigInteger &operator=(const BigInteger &other);

  BigInteger operator/(const BigInteger &other) const;

  BigInteger &operator/=(const BigInteger &other);

  BigInteger operator%(const BigInteger &other) const;

  BigInteger &operator%=(const BigInteger &other);
};


#endif
