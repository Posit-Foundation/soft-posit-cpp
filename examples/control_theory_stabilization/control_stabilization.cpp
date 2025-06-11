// This example demonstrates a control system simulation of an inverted pendulum using different number systems:
// float, posit, and posit with quire. It simulates the dynamics of the system, applies control inputs, and visualizes the results.

// Formulas and values for the inverted pendulum system are copied from: https://ctms.engin.umich.edu/CTMS/index.php?example=InvertedPendulum&section=ControlStateSpace

// Next steps: 
// Theoretically, stepWithQuire() can perform even more operations before rounding. Need to double-check check softposit.h for supported operations.

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <string>
#include <algorithm>
#include <numeric>
#include <softposit.h>
#include <filesystem>
#include <sys/stat.h>

// Image dimensions
const int IMAGE_WIDTH = 800;
const int IMAGE_HEIGHT = 600;
const int SYSTEM_COUNT = 3; // float, posit, posit+quire

// System parameters for inverted pendulum
const double GRAVITY = 9.8;  // Gravity acceleration (m/s^2) --> "g"
const double CART_MASS = 0.5; // Mass of the cart (kg) --> "M"
const double POLE_MASS = 0.2; // Mass of the pendulum (kg) --> "m"
const double POLE_LENGTH = 0.3; // Length to pendulum center of mass (m) --> "l"
const double CART_FRICTION = 0.1; // Friction coefficient for the cart (N/m/sec) --> "b"
const double MOMENT_OF_INERTIA = (POLE_MASS * POLE_LENGTH * POLE_LENGTH) / 3.0; // Moment of inertia of the pendulum (kg*m^2) --> "I"

// Simulation parameters
const double TIME_STEP = 0.02; // Simulation time step (s)
const int TIME_STEPS = 500; // Number of simulation steps

// Define colors for visualization
struct RGB {
    unsigned char r, g, b;
    RGB(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0) : r(r), g(g), b(b) {}
};

// Forward declaration
void drawLine(std::vector<RGB>& image, int x1, int y1, int x2, int y2, const RGB& color);

// Define overloaded operators for posit32_t
inline posit32_t operator+(const posit32_t& a, const posit32_t& b) {
    return p32_add(a, b);
}

inline posit32_t operator-(const posit32_t& a, const posit32_t& b) {
    return p32_sub(a, b);
}

inline posit32_t operator*(const posit32_t& a, const posit32_t& b) {
    return p32_mul(a, b);
}

inline posit32_t operator/(const posit32_t& a, const posit32_t& b) {
    return p32_div(a, b);
}

// Conversion functions for posit
inline posit32_t ui32_to_p32(uint32_t a) {
    posit32_t p;
    p.v = a;
    return p;
}

inline posit32_t int_to_posit32(int value) {
    return ui32_to_p32(value);
}

inline posit32_t double_to_posit32(double value) {
    return convertDoubleToP32(value);
}

inline double posit32_to_double(posit32_t value) {
    return convertP32ToDouble(value);
}

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
                augmented(i, i + cols) = int_to_posit32(1); // Identity matrix on the right
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

// Function to save a PPM image
void savePPM(const std::string& filename, const std::vector<RGB>& pixels, int width, int height) {
    std::ofstream outfile(filename, std::ios::binary);
    outfile << "P6\n" << width << " " << height << "\n255\n";
    
    for (const auto& pixel : pixels) {
        outfile.write(reinterpret_cast<const char*>(&pixel), 3);
    }
    
    outfile.close();
    std::cout << "Saved image to " << filename << std::endl;
}

// Function to save simulation data to CSV
void saveSimulationData(const std::vector<double>& floatPositions,
                       const std::vector<double>& floatAngles,
                       const std::vector<double>& positPositions,
                       const std::vector<double>& positAngles,
                       const std::vector<double>& quirePositions,
                       const std::vector<double>& quireAngles) {
    // Create data directory if it doesn't exist
    mkdir("data", 0777);
    mkdir("data/control_theory", 0777);
    
    // Open CSV file
    std::ofstream csvFile("data/control_theory/simulation_data.csv");
    
    // Write header
    csvFile << "Time,Float_Position,Float_Angle,Posit_Position,Posit_Angle,Quire_Position,Quire_Angle\n";
    
    // Write data
    for (int t = 0; t < TIME_STEPS; ++t) {
        csvFile << t * TIME_STEP << ","
                << floatPositions[t] << ","
                << floatAngles[t] << ","
                << positPositions[t] << ","
                << positAngles[t] << ","
                << quirePositions[t] << ","
                << quireAngles[t] << "\n";
    }
    
    csvFile.close();
    std::cout << "Simulation data saved to data/control_theory/simulation_data.csv" << std::endl;
}

// Define the inverted pendulum system
template<typename T>
class InvertedPendulumSystem {
private:
    // System matrices
    Matrix<T> A; // State transition matrix
    Matrix<T> B; // Control matrix
    Matrix<T> K; // Control gain matrix
    Matrix<T> x; // State vector [position, velocity, angle, angular velocity]

    // Time step for simulation
    T dt = convertFromDouble(TIME_STEP);

    // Convert double to the template type
    T convertFromDouble(double value) {
        if constexpr (std::is_same_v<T, posit32_t>) {
            return double_to_posit32(value);
        } else {
            return static_cast<T>(value);
        }
    }

    // Convert from template type to double for visualization
    double convertToDouble(T value) {
        if constexpr (std::is_same_v<T, posit32_t>) {
            return posit32_to_double(value);
        } else {
            return static_cast<double>(value);
        }
    }

public:
    InvertedPendulumSystem() 
        : A(4, 4), B(4, 1), K(1, 4), x(4, 1)
    {
        // Initialize system matrices
        // Linearized model of inverted pendulum around the upright position
        
        // Initialize A matrix
        // x' = Ax + Bu
        // State: [position, velocity, angle, angular velocity]
        T M = convertFromDouble(CART_MASS);
        T m = convertFromDouble(POLE_MASS);
        T l = convertFromDouble(POLE_LENGTH);
        T g = convertFromDouble(GRAVITY);
        T b = convertFromDouble(CART_FRICTION);
        T I = convertFromDouble(MOMENT_OF_INERTIA);
        
        // Simplified continuous-time model
        T one = convertFromDouble(1.0);
        T negOne = convertFromDouble(-1.0);
        T denom = one / (I * (m+M) + M * m * l*l);
        
        // A matrix for continuous system
        A(0, 0) = convertFromDouble(0);
        A(0, 1) = convertFromDouble(1);
        A(0, 2) = convertFromDouble(0);
        A(0, 3) = convertFromDouble(0);
        
        A(1, 0) = convertFromDouble(0);
        A(1, 1) = negOne * (I + m * l*l) * b * denom;
        A(1, 2) = m*m * g * l*l * denom;
        A(1, 3) = convertFromDouble(0);
        
        A(2, 0) = convertFromDouble(0);
        A(2, 1) = convertFromDouble(0);
        A(2, 2) = convertFromDouble(0);
        A(2, 3) = convertFromDouble(1);
        
        A(3, 0) = convertFromDouble(0);
        A(3, 1) = negOne * m*l*b * denom;
        A(3, 2) = m * g * l * (M+m) * denom;
        A(3, 3) = convertFromDouble(0);
        
        // B matrix for control input
        B(0, 0) = convertFromDouble(0);
        B(1, 0) = I + m*l*l * denom;
        B(2, 0) = convertFromDouble(0);
        B(3, 0) = m*l * denom;

        // Initialize state 
        // Small initial angle perturbation
        x(0, 0) = convertFromDouble(0.0);    // Initial position
        x(1, 0) = convertFromDouble(0.0);    // Initial velocity
        x(2, 0) = convertFromDouble(0.05);   // Initial angle (small perturbation)
        x(3, 0) = convertFromDouble(0.0);    // Initial angular velocity
        
        // Initialize K with LQR-like gains (simplified)
        K(0, 0) = convertFromDouble(-1.00);    // Position gain
        K(0, 1) = convertFromDouble(-1.66);    // Velocity gain
        K(0, 2) = convertFromDouble(18.7);   // Angle gain (high to prioritize angle stabilization)
        K(0, 3) = convertFromDouble(3.46);    // Angular velocity gain
    }

    // Step the system forward in time
    void step() {
        // Compute control input u = -Kx
        Matrix<T> u = K * x * convertFromDouble(-1.0);  // Negative feedback

        // Update state: x' = Ax + Bu
        Matrix<T> dxdt = A*x + B*u;
        Matrix<T> dx = dxdt * dt; // Compute change in state
        x = x + dx;     // Update state
    }

    // Step system forward in time, but use quire fused-dot product when multiplying matrices
    void stepWithQuire() {
        // Only valid for posit32_t
        static_assert(std::is_same_v<T, posit32_t>, "stepWithQuire is only valid for posit32_t");
        
        // Compute control input u = -Kx
        Matrix<T> u = K.multiplyWithQuire(x);
        Matrix<T> negU(1, 1);
        negU(0, 0) = convertFromDouble(-1.0) * u(0, 0); // Negative feedback

        // Update state: x' = Ax + Bu
        Matrix<T> Ax = A.multiplyWithQuire(x);
        Matrix<T> Bu = B.multiplyWithQuire(negU);
        Matrix<T> dxdt = Ax + Bu;
        Matrix<T> dx = dxdt * dt; // Compute change in state
        x = x + dx; // Update state
    }

    // Get cart position
    double getPosition() {
        return convertToDouble(x(0, 0));
    }

    // Get pendulum angle
    double getAngle() {
        return convertToDouble(x(2, 0));
    }

    // Get state for visualization
    Matrix<T> getState() {
        return x;
    }
};

// Function to simulate the system and generate visualization
void runSimulation() {
    // Create systems with different number types
    InvertedPendulumSystem<float> floatSystem;
    InvertedPendulumSystem<posit32_t> positSystem;
    InvertedPendulumSystem<posit32_t> quireSystem;
    
    // Store trajectory data for visualization
    std::vector<double> floatPositions(TIME_STEPS);
    std::vector<double> floatAngles(TIME_STEPS);
    std::vector<double> positPositions(TIME_STEPS);
    std::vector<double> positAngles(TIME_STEPS);
    std::vector<double> quirePositions(TIME_STEPS);
    std::vector<double> quireAngles(TIME_STEPS);
    
    // Run simulation
    for (int t = 0; t < TIME_STEPS; ++t) {
        // Store current state
        floatPositions[t] = floatSystem.getPosition();
        floatAngles[t] = floatSystem.getAngle();
        positPositions[t] = positSystem.getPosition();
        positAngles[t] = positSystem.getAngle();
        quirePositions[t] = quireSystem.getPosition();
        quireAngles[t] = quireSystem.getAngle();
        
        // Step the systems
        floatSystem.step();
        positSystem.step();
        quireSystem.stepWithQuire();
    }
    
    // Save simulation data to CSV
    saveSimulationData(floatPositions, floatAngles,
                      positPositions, positAngles,
                      quirePositions, quireAngles);
    
    // Find min/max values for normalization
    double minPosition = std::min({
        *std::min_element(floatPositions.begin(), floatPositions.end()),
        *std::min_element(positPositions.begin(), positPositions.end()),
        *std::min_element(quirePositions.begin(), quirePositions.end())
    });
    
    double maxPosition = std::max({
        *std::max_element(floatPositions.begin(), floatPositions.end()),
        *std::max_element(positPositions.begin(), positPositions.end()),
        *std::max_element(quirePositions.begin(), quirePositions.end())
    });
    
    double minAngle = std::min({
        *std::min_element(floatAngles.begin(), floatAngles.end()),
        *std::min_element(positAngles.begin(), positAngles.end()),
        *std::min_element(quireAngles.begin(), quireAngles.end())
    });
    
    double maxAngle = std::max({
        *std::max_element(floatAngles.begin(), floatAngles.end()),
        *std::max_element(positAngles.begin(), positAngles.end()),
        *std::max_element(quireAngles.begin(), quireAngles.end())
    });
    
    // Create visualization images
    std::vector<RGB> floatImage(IMAGE_WIDTH * IMAGE_HEIGHT);
    std::vector<RGB> positImage(IMAGE_WIDTH * IMAGE_HEIGHT);
    std::vector<RGB> quireImage(IMAGE_WIDTH * IMAGE_HEIGHT);
    std::vector<RGB> comparisonImage(IMAGE_WIDTH * IMAGE_HEIGHT);
    
    // Initialize backgrounds to white
    std::fill(floatImage.begin(), floatImage.end(), RGB(255, 255, 255));
    std::fill(positImage.begin(), positImage.end(), RGB(255, 255, 255));
    std::fill(quireImage.begin(), quireImage.end(), RGB(255, 255, 255));
    std::fill(comparisonImage.begin(), comparisonImage.end(), RGB(255, 255, 255));
    
    // Helper function to map a value to the y-coordinate in image
    auto mapToY = [&](double value, double min, double max) -> int {
        return static_cast<int>((1.0 - (value - min) / (max - min)) * (IMAGE_HEIGHT * 0.8) + (IMAGE_HEIGHT * 0.1));
    };
    
    // Draw horizontal grid lines
    for (int i = 1; i < 10; ++i) {
        int y = static_cast<int>(i * IMAGE_HEIGHT / 10.0);
        for (int x = 0; x < IMAGE_WIDTH; ++x) {
            // Light gray lines
            RGB gridColor(230, 230, 230);
            floatImage[y * IMAGE_WIDTH + x] = gridColor;
            positImage[y * IMAGE_WIDTH + x] = gridColor;
            quireImage[y * IMAGE_WIDTH + x] = gridColor;
            comparisonImage[y * IMAGE_WIDTH + x] = gridColor;
        }
    }
    
    // Draw vertical grid lines
    for (int i = 1; i < 10; ++i) {
        int x = static_cast<int>(i * IMAGE_WIDTH / 10.0);
        for (int y = 0; y < IMAGE_HEIGHT; ++y) {
            // Light gray lines
            RGB gridColor(230, 230, 230);
            floatImage[y * IMAGE_WIDTH + x] = gridColor;
            positImage[y * IMAGE_WIDTH + x] = gridColor;
            quireImage[y * IMAGE_WIDTH + x] = gridColor;
            comparisonImage[y * IMAGE_WIDTH + x] = gridColor;
        }
    }
    
    // Add labels at the top of the image
    std::string floatLabel = "Float Precision";
    std::string positLabel = "Posit Precision";
    std::string quireLabel = "Posit+Quire Precision";
    std::string comparisonLabel = "Comparison (Red=Float, Green=Posit, Blue=Quire)";
    
    // Plot trajectories
    for (int t = 1; t < TIME_STEPS; ++t) {
        // For the position
        int x1 = static_cast<int>((t - 1) * IMAGE_WIDTH / TIME_STEPS);
        int x2 = static_cast<int>(t * IMAGE_WIDTH / TIME_STEPS);
        
        // Calculate y-coordinates for cart position
        int floatPosY1 = mapToY(floatPositions[t-1], minPosition, maxPosition);
        int floatPosY2 = mapToY(floatPositions[t], minPosition, maxPosition);
        int positPosY1 = mapToY(positPositions[t-1], minPosition, maxPosition);
        int positPosY2 = mapToY(positPositions[t], minPosition, maxPosition);
        int quirePosY1 = mapToY(quirePositions[t-1], minPosition, maxPosition);
        int quirePosY2 = mapToY(quirePositions[t], minPosition, maxPosition);
        
        // Draw position line for float (blue)
        drawLine(floatImage, x1, floatPosY1, x2, floatPosY2, RGB(0, 0, 255));
        
        // Draw position line for posit (green)
        drawLine(positImage, x1, positPosY1, x2, positPosY2, RGB(0, 255, 0));
        
        // Draw position line for quire (red)
        drawLine(quireImage, x1, quirePosY1, x2, quirePosY2, RGB(255, 0, 0));
        
        // Draw all lines on comparison image
        drawLine(comparisonImage, x1, floatPosY1, x2, floatPosY2, RGB(255, 0, 0));
        drawLine(comparisonImage, x1, positPosY1, x2, positPosY2, RGB(0, 255, 0));
        drawLine(comparisonImage, x1, quirePosY1, x2, quirePosY2, RGB(0, 0, 255));
        
        // Calculate y-coordinates for pendulum angle
        int floatAngleY1 = mapToY(floatAngles[t-1], minAngle, maxAngle);
        int floatAngleY2 = mapToY(floatAngles[t], minAngle, maxAngle);
        int positAngleY1 = mapToY(positAngles[t-1], minAngle, maxAngle);
        int positAngleY2 = mapToY(positAngles[t], minAngle, maxAngle);
        int quireAngleY1 = mapToY(quireAngles[t-1], minAngle, maxAngle);
        int quireAngleY2 = mapToY(quireAngles[t], minAngle, maxAngle);
        
        // Draw angle line for float (red, dashed)
        if (t % 2 == 0) {  // Create dashed effect
            drawLine(floatImage, x1, floatAngleY1, x2, floatAngleY2, RGB(255, 0, 0));
        }
        
        // Draw angle line for posit (red, dashed)
        if (t % 2 == 0) {
            drawLine(positImage, x1, positAngleY1, x2, positAngleY2, RGB(255, 0, 0));
        }
        
        // Draw angle line for quire (blue, dashed)
        if (t % 2 == 0) {
            drawLine(quireImage, x1, quireAngleY1, x2, quireAngleY2, RGB(0, 0, 255));
        }
    }
    
    // Calculate errors
    double maxPositError = 0.0;
    double maxQuireError = 0.0;
    double avgPositError = 0.0;
    double avgQuireError = 0.0;
    
    for (int t = 0; t < TIME_STEPS; ++t) {
        double positError = std::abs(floatPositions[t] - positPositions[t]);
        double quireError = std::abs(floatPositions[t] - quirePositions[t]);
        
        maxPositError = std::max(maxPositError, positError);
        maxQuireError = std::max(maxQuireError, quireError);
        
        avgPositError += positError;
        avgQuireError += quireError;
    }
    
    avgPositError /= TIME_STEPS;
    avgQuireError /= TIME_STEPS;
    
    std::cout << "Error Analysis:" << std::endl;
    std::cout << "Float vs Posit - Max Error: " << maxPositError << ", Avg Error: " << avgPositError << std::endl;
    std::cout << "Float vs Quire - Max Error: " << maxQuireError << ", Avg Error: " << avgQuireError << std::endl;
    
    // Save images
    savePPM("control_system_float.ppm", floatImage, IMAGE_WIDTH, IMAGE_HEIGHT);
    savePPM("control_system_posit.ppm", positImage, IMAGE_WIDTH, IMAGE_HEIGHT);
    savePPM("control_system_quire.ppm", quireImage, IMAGE_WIDTH, IMAGE_HEIGHT);
    savePPM("control_system_comparison.ppm", comparisonImage, IMAGE_WIDTH, IMAGE_HEIGHT);
}

// Bresenham's line algorithm for drawing lines
void drawLine(std::vector<RGB>& image, int x1, int y1, int x2, int y2, const RGB& color) {
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        if (x1 >= 0 && x1 < IMAGE_WIDTH && y1 >= 0 && y1 < IMAGE_HEIGHT) {
            image[y1 * IMAGE_WIDTH + x1] = color;
        }
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

int main() {
    try {
        std::cout << "Running Inverted Pendulum Control System Simulation..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        
        runSimulation();
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Simulation completed in " << elapsed.count() << " seconds." << std::endl;
        
        std::cout << "Example completed successfully. Images saved as PPM files." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 