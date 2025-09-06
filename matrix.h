#ifndef MATRIX_H
#define MATRIX_H
#define MATRIX_SQUARE_MATRIX_IMPLEMENTED

#include <stdexcept>

class MatrixIsDegenerateError : public std::runtime_error {
public:
  MatrixIsDegenerateError() : std::runtime_error("MatrixIsDegenerateError") {
  }
};

class MatrixOutOfRange : public std::out_of_range {
public:
  MatrixOutOfRange() : std::out_of_range("MatrixOutOfRange") {
  }
};

template<typename T, size_t Rows, size_t Cols>
class Matrix {
  static T AbsVal(const T &value) {
    return value < static_cast<T>(0) ? -value : value;
  };

public:
  T massive[Rows][Cols];

  static constexpr size_t RowsNumber() {
    return Rows;
  }

  static constexpr size_t ColumnsNumber() {
    return Cols;
  }

  T &operator()(const int i, const int j) {
    return massive[i][j];
  }

  const T &operator()(const int i, const int j) const {
    return massive[i][j];
  }

  T &At(const size_t i, const size_t j) {
    if (i >= RowsNumber() || j >= ColumnsNumber()) {
      throw MatrixOutOfRange();
    }
    return massive[i][j];
  }


  const T &At(const size_t i, const size_t j) const {
    if (i >= RowsNumber() || j >= ColumnsNumber()) {
      throw MatrixOutOfRange();
    }
    return massive[i][j];
  }

  friend Matrix<T, Cols, Rows> GetTransposed(const Matrix &matrix) {
    Matrix<T, Cols, Rows> transposed;
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        transposed(j, i) = matrix(i, j);
      }
    }
    return transposed;
  }

  //+-*
  Matrix operator+(const Matrix &other) const {
    Matrix result;
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        result(i, j) = At(i, j) + other(i, j);
      }
    }
    return result;
  }

  Matrix &operator+=(const Matrix &rhs) {
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        At(i, j) = At(i, j) + rhs.At(i, j);
      }
    }
    return *this;
  }

  Matrix operator-(const Matrix &rhs) const {
    Matrix result;
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        result(i, j) = At(i, j) - rhs.At(i, j);
      }
    }
    return result;
  }


  Matrix &operator-=(const Matrix &rhs) {
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        At(i, j) = At(i, j) - rhs.At(i, j);
      }
    }
    return *this;
  }

  template<size_t Cols2>
  Matrix<T, Rows, Cols2> operator*(const Matrix<T, Cols, Cols2> &rhs) const {
    Matrix<T, Rows, Cols2> result;
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols2; ++j) {
        T sum = T{};
        for (size_t k = 0; k < Cols; ++k) {
          sum += At(i, k) * rhs(k, j);
        }
        result(i, j) = sum;
      }
    }
    return result;
  }

  Matrix &operator*=(const Matrix<T, Cols, Cols> &rhs) {
    Matrix result;
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        T sum = T{};
        for (size_t k = 0; k < Cols; ++k) {
          sum += At(i, k) * rhs(k, j);
        }
        result(i, j) = sum;
      }
    }
    *this = result;
    return *this;
  }

  template<class S, typename = std::enable_if_t<!std::is_class_v<S>> >
  friend Matrix operator*(S delta, const Matrix &matr) {
    Matrix result;
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        result(i, j) = matr(i, j) * delta;
      }
    }
    return result;
  }

  template<class S, typename = std::enable_if_t<!std::is_class_v<S>> >
  friend Matrix operator*(const Matrix &matr, S delta) {
    Matrix result;
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        result(i, j) = matr(i, j) * delta;
      }
    }
    return result;
  }

  template<class S, typename = std::enable_if_t<!std::is_class_v<S>> >
  friend Matrix &operator*=(Matrix &matr, S delta) {
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        matr(i, j) = matr(i, j) * delta;
      }
    }
    return matr;
  }

  template<class S>
  friend Matrix operator/(const Matrix &matr, S delta) {
    Matrix result;
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        result(i, j) = matr(i, j) / delta;
      }
    }
    return result;
  }

  template<class S>
  friend Matrix operator/(S delta, const Matrix &matr) {
    Matrix result;
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        result(i, j) = delta / matr(i, j);
      }
    }
    return result;
  }

  template<class S>
  friend Matrix &operator/=(Matrix &matr, S delta) {
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        matr(i, j) = matr(i, j) / delta;
      }
    }
    return matr;
  }

  bool operator==(const Matrix &rhs) const {
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        if (At(i, j) != rhs.At(i, j)) {
          return false;
        }
      }
    }
    return true;
  }

  bool operator!=(const Matrix &rhs) const {
    return !(*this == rhs);
  }

  friend std::ostream &operator<<(std::ostream &os, const Matrix &value) {
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        os << value(i, j);
        if (j != Cols - 1) {
          os << " ";
        }
      }
      os << '\n';
    }
    return os;
  }

  friend std::istream &operator>>(std::istream &is, Matrix &value) {
    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Cols; ++j) {
        is >> value(i, j);
      }
    }
    return is;
  }

  friend void Transpose(Matrix &matrix) {
    Matrix result = GetTransposed(matrix);
    matrix = result;
  }

  friend T Trace(const Matrix &matrix) {
    T trace = T{};
    size_t i = 0;
    while (i < matrix.RowsNumber()) {
      trace += matrix.At(i, i);
      ++i;
    }
    return trace;
  }

  friend T Determinant(const Matrix &mat) {
    if constexpr (Rows == 1) {
      return mat(0, 0);
    } else if constexpr (Rows == 2) {
      return mat(0, 0) * mat(1, 1) - mat(0, 1) * mat(1, 0);
    } else if constexpr (Rows == 3) {
      return mat(0, 0) * mat(1, 1) * mat(2, 2) +
             mat(0, 1) * mat(1, 2) * mat(2, 0) +
             mat(0, 2) * mat(1, 0) * mat(2, 1) -
             mat(0, 2) * mat(1, 1) * mat(2, 0) -
             mat(0, 0) * mat(1, 2) * mat(2, 1) -
             mat(0, 1) * mat(1, 0) * mat(2, 2);
    } else {
      T det = 0;
      int sign = 1;

      for (size_t col = 0; col < Rows; ++col) {
        Matrix<T, Rows - 1, Rows - 1> minor;
        for (size_t i = 1; i < Rows; ++i) {
          for (size_t j = 0, k = 0; j < Rows; ++j) {
            if (j == col) {
              continue;
            }
            minor(i - 1, k++) = mat(i, j);
          }
        }

        det += sign * mat(0, col) * Determinant(minor);
        sign = -sign;
      }
      return det;
    }
  }

  friend Matrix<T, Rows, Rows> GetInversed(const Matrix &matrix) {
    Matrix result;
    Matrix temp = matrix;

    for (size_t i = 0; i < Rows; ++i) {
      for (size_t j = 0; j < Rows; ++j) {
        result(i, j) = (i == j) ? T{1} : T{0};
      }
    }

    for (size_t i = 0; i < Rows; ++i) {
      size_t main_element = i;
      for (size_t j = i + 1; j < Rows; ++j) {
        if (AbsVal(temp(j, i)) > AbsVal(temp(main_element, i))) {
          main_element = j;
        }
      }

      if (temp(main_element, i) == T{0}) {
        throw MatrixIsDegenerateError();
      }

      if (i != main_element) {
        std::swap(temp.massive[i], temp.massive[main_element]);
        std::swap(result.massive[i], result.massive[main_element]);
      }

      T k_for_main_element_to_one = T{1} / temp(i, i);

      for (size_t j = 0; j < Cols; ++j) {
        temp(i, j) *= k_for_main_element_to_one;
        result(i, j) *= k_for_main_element_to_one;
      }

      for (size_t j = 0; j < Rows; ++j) {
        if (j == i) {
          continue;
        }
        T factor = temp(j, i);
        for (size_t k = 0; k < Cols; ++k) {
          temp(j, k) -= factor * temp(i, k);
          result(j, k) -= factor * result(i, k);
        }
      }
    }
    return result;
  }

  friend void Inverse(Matrix &matrix) {
    Matrix result = GetInversed(matrix);
    matrix = result;
  }
};
#endif
