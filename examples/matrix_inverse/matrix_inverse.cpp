/*
This example compares the accuracy of ill-conditioned matrix inversion 
in different number systems: float and posit32.

// inputs: one Matrix A, one vector b
// outputs: determinant of A, inverse of A, solve system Ax = b

// perform all computations with float32
// perform all computations with posit32
// perform all computations with double
*/

#include <posit_ops.h>
#include <matrix.h>
#include <softposit.h>
#include <iostream>
#include <vector>
#include <Eigen/Dense>

// Starting with 2x2 matrix for simple determinant calculation
std::vector<std::vector<double>> A = {
    {-0.0015, 0},
    {0.001, -21.921e25}
};

std::vector<double> b = {1, 1};

// Convert double to the template type
template<typename T>
T convertFromDouble(double value) {
    if constexpr (std::is_same_v<T, posit32_t>) {
        return double_to_posit32(value);
    } else {
        return static_cast<T>(value);
    }
}

// Convert from template type to double for visualization
template<typename T>
double convertToDouble(T value) {
    if constexpr (std::is_same_v<T, posit32_t>) {
        return posit32_to_double(value);
    } else {
        return static_cast<double>(value);
    }
}

// Print a line for better readability
void printLine() {
    std::cout << "----------------------------------------" << std::endl;
}

// Define a 2x2 matrix (you can change this to any size)
double cond(std::vector<std::vector<double>> mat) {
    Eigen::Matrix2d A;

    // Convert input vector of vectors to Eigen matrix
    for (size_t i = 0; i < mat.size(); ++i) {
        for (size_t j = 0; j < mat[i].size(); ++j) {
            A(i, j) = mat[i][j];
        }
    }

    // Perform Singular Value Decomposition
    Eigen::JacobiSVD<Eigen::Matrix2d> svd(A);

    // Extract singular values
    Eigen::Vector2d singularValues = svd.singularValues();

    double sigma_max = singularValues(0);
    double sigma_min = singularValues(1);

    if (sigma_min == 0.0) {
        std::cout << "Matrix is singular — condition number is infinite." << std::endl;
        return 0;
    } else {
        double cond_number = sigma_max / sigma_min;
        return cond_number;
    }
}

// Solve linear system Ax = b using matrix inversion
template<typename T>
void solveSystem(std::vector<std::vector<double>> A_in, std::vector<double> b_in) {
    // Inputs
    Matrix<T> A(A_in.size(), A_in[0].size()); 
    Matrix<T> b(b_in.size(), 1);
    Matrix<T> invA(A_in.size(), A_in.size());  // inverse of A is always square
    Matrix<T> x(A.getCols(), 1);  // solution vector x

    // convert input vectors to Matrix<T> format
    for (size_t i = 0; i < A_in.size(); ++i) {
        for (size_t j = 0; j < A_in[i].size(); ++j) {
            A(i, j) = convertFromDouble<T>(A_in[i][j]);
        }
    }

    for (size_t i = 0; i < b_in.size(); ++i) {
        b(i, 0) = convertFromDouble<T>(b_in[i]);
    }

    // Calculate determinant (2x2 matrix for simplicity)
    T det = A(0, 0) * A(1, 1) - A(0, 1) * A(1, 0);
    
    // Calculate inverse
    if (det == convertFromDouble<T>(0)) {
        std::cerr << "Matrix is singular, no unique solution." << std::endl;
        return;
    } else {
        invA = A.inverse();
        /*
        // For 2x2 matrix, the inverse can be calculated more directly
        invA(0, 0) = A(1, 1) / det;
        invA(0, 1) = convertFromDouble<T>(-1.0) * A(0, 1) / det;
        invA(1, 0) = convertFromDouble<T>(-1.0) * A(1, 0) / det;
        invA(1, 1) = A(0, 0) / det;
        */
    }
    
    // Solve Ax = b
    x = invA * b;

    // Output results
    printLine();
    //std::cout << "Determinant: " << det << std::endl;
    //std::cout << "Inverse Matrix:" << std::endl;
    //invA.print();
    std::cout << "Solution x:" << std::endl;
    x.print();
    printLine();
    std::cout << std::endl;
}

int main() {
    // Calculate condition number
    std::cout << "\nCondition number of matrix A: " << cond(A) << "\n" << std::endl;

    // Solve system with float
    std::cout << "FLOAT" << std::endl;
    solveSystem<float>(A, b);

    // Solve system with posit32
    std::cout << "POSIT32" << std::endl;
    solveSystem<posit32_t>(A, b);

    // Solve system with double
    std::cout << "\nDOUBLE" << std::endl;
    solveSystem<double>(A, b);

    return 0;
}