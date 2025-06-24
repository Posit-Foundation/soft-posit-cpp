#ifndef MATRIX_H
#define MATRIX_H

#include "softposit.h"
#include "posit_ops.h"
#include <vector>
#include <iostream>
#include <random>

// Matrix class template for different number types
template<typename T>
class Matrix {
private:
    std::vector<T> data;
    size_t rows, cols;

public:
    Matrix(size_t r, size_t c) : rows(r), cols(c), data(r * c) {}
    
    Matrix(size_t r, size_t c, const std::vector<T>& values) : rows(r), cols(c), data(values) {
        if (values.size() != r * c) {
            throw std::runtime_error("Matrix initialization size mismatch");
        }
    }

    T& operator()(size_t row, size_t col) {
        return data[row * cols + col];
    }

    const T& operator()(size_t row, size_t col) const {
        return data[row * cols + col];
    }

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }

    // Matrix addition
    Matrix<T> operator+(const Matrix<T>& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::runtime_error("Matrix addition dimension mismatch");
        }
        
        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = (*this)(i, j) + other(i, j);
            }
        }
        return result;
    }

    // Matrix subtraction
    Matrix<T> operator-(const Matrix<T>& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::runtime_error("Matrix subtraction dimension mismatch");
        }
        
        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = (*this)(i, j) - other(i, j);
            }
        }
        return result;
    }

    // Matrix multiplication
    Matrix<T> operator*(const Matrix<T>& other) const {
        if (cols != other.rows) {
            throw std::runtime_error("Matrix multiplication dimension mismatch");
        }
        
        Matrix<T> result(rows, other.cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.cols; ++j) {
                if constexpr (std::is_same_v<T, posit32_t>) {
                    T sum = int_to_posit32(0);
                    for (size_t k = 0; k < cols; ++k) {
                        sum = sum + ((*this)(i, k) * other(k, j));
                    }
                    result(i, j) = sum;
                } else {
                    T sum = 0;
                    for (size_t k = 0; k < cols; ++k) {
                        sum += (*this)(i, k) * other(k, j);
                    }
                    result(i, j) = sum;
                }
            }
        }
        return result;
    }

    // Matrix multiplication with quire accumulator for posit
    Matrix<posit32_t> multiplyWithQuire(const Matrix<posit32_t>& other) const {
        if (cols != other.getRows()) {
            throw std::runtime_error("Matrix multiplication dimension mismatch");
        }
        
        Matrix<posit32_t> result(rows, other.getCols());
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.getCols(); ++j) {
                quire32_t q;
                q = q32_clr(q); // Initialize quire to 0
                for (size_t k = 0; k < cols; ++k) {
                    q = q32_fdp_add(q, (*this)(i, k), other(k, j));
                }
                result(i, j) = q32_to_p32(q);
            }
        }
        return result;
    }

    // Print matrix (for debugging)
    void print() const {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                if constexpr (std::is_same_v<T, posit32_t>) {
                    std::cout << posit32_to_double((*this)(i, j)) << " ";
                } else {
                    std::cout << (*this)(i, j) << " ";
                }
            }
            std::cout << std::endl;
        }
    }

    // Matrix transpose
    Matrix<T> transpose() const {
        Matrix<T> result(cols, rows);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }

    // Matrix scalar multiplication
    Matrix<T> operator*(T scalar) const {
        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = (*this)(i, j) * scalar;
            }
        }
        return result;
    }

    // Matrix inversion using Gauss-Jordan elimination
    // This is a simple implementation for demonstration purposes
    Matrix<T> inverse() const {
        if (rows != cols) {
            throw std::runtime_error("Can only invert square matrices");
        }

        // Create an augmented matrix [A|I]
        Matrix<T> augmented(rows, 2 * cols);
        
        // Initialize augmented matrix
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                augmented(i, j) = (*this)(i, j);
            }
            if constexpr (std::is_same_v<T, posit32_t>) {
                augmented(i, i + cols) = double_to_posit32(1); // Identity matrix on the right
            } else {
                augmented(i, i + cols) = 1; // Identity matrix on the right
            }
        }

        // Perform Gauss-Jordan elimination
        for (size_t i = 0; i < rows; ++i) {
            // Find pivot
            T pivot = augmented(i, i);
            
            // Normalize the pivot row
            for (size_t j = 0; j < 2 * cols; ++j) {
                augmented(i, j) = augmented(i, j) / pivot;
            }
            
            // Eliminate other rows
            for (size_t k = 0; k < rows; ++k) {
                if (k != i) {
                    T factor = augmented(k, i);
                    for (size_t j = 0; j < 2 * cols; ++j) {
                        augmented(k, j) = augmented(k, j) - factor * augmented(i, j);
                    }
                }
            }
        }
        
        // Extract the inverse matrix
        Matrix<T> inverse(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                inverse(i, j) = augmented(i, j + cols);
            }
        }
        
        return inverse;
    }
};

#endif // MATRIX_H