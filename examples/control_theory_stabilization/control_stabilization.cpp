#include <softposit_cpp.h>
#include <sys/stat.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

// Image dimensions
const int IMAGE_WIDTH  = 800;
const int IMAGE_HEIGHT = 600;
const int TIME_STEPS   = 500;
const int SYSTEM_COUNT = 3;  // float, posit, posit+quire

// System parameters for inverted pendulum
const double GRAVITY     = 9.81;  // Gravity acceleration (m/s^2)
const double CART_MASS   = 1.0;   // Mass of the cart (kg)
const double POLE_MASS   = 0.1;   // Mass of the pendulum (kg)
const double POLE_LENGTH = 0.5;   // Half length of the pendulum (m)
const double TIME_STEP   = 0.02;  // Simulation time step (s)

// Matrix class template for different number types
template <typename T>
class Matrix
{
   private:
    std::vector<T> data;
    size_t         rows, cols;

   public:
    Matrix(size_t r, size_t c) : rows(r), cols(c), data(r * c)
    {
    }

    Matrix(size_t r, size_t c, const std::vector<T>& values) : rows(r), cols(c), data(values)
    {
        if (values.size() != r * c)
        {
            throw std::runtime_error("Matrix initialization size mismatch");
        }
    }

    T& operator()(size_t row, size_t col)
    {
        return data[row * cols + col];
    }

    const T& operator()(size_t row, size_t col) const
    {
        return data[row * cols + col];
    }

    size_t getRows() const
    {
        return rows;
    }
    size_t getCols() const
    {
        return cols;
    }

    // Matrix addition
    Matrix<T> operator+(const Matrix<T>& other) const
    {
        if (rows != other.rows || cols != other.cols)
        {
            throw std::runtime_error("Matrix addition dimension mismatch");
        }

        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
            {
                result(i, j) = (*this)(i, j) + other(i, j);
            }
        }
        return result;
    }

    // Matrix subtraction
    Matrix<T> operator-(const Matrix<T>& other) const
    {
        if (rows != other.rows || cols != other.cols)
        {
            throw std::runtime_error("Matrix subtraction dimension mismatch");
        }

        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
            {
                result(i, j) = (*this)(i, j) - other(i, j);
            }
        }
        return result;
    }

    // Matrix multiplication
    Matrix<T> operator*(const Matrix<T>& other) const
    {
        if (cols != other.rows)
        {
            std::stringstream ss;
            ss << "Matrix multiplication dimension mismatch: (" << rows << ", " << cols << ") @ ("
            << other.rows << ", " << other.cols << ")";

            throw std::runtime_error(ss.str());
        }

        Matrix<T> result(rows, other.cols);
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < other.cols; ++j)
            {
                if constexpr (std::is_same_v<T, posit32>)
                {
                    T sum = posit32(0);
                    for (size_t k = 0; k < cols; ++k)
                    {
                        sum = sum + ((*this)(i, k) * other(k, j));
                    }
                    result(i, j) = sum;
                }
                else
                {
                    T sum = 0;
                    for (size_t k = 0; k < cols; ++k)
                    {
                        sum += (*this)(i, k) * other(k, j);
                    }
                    result(i, j) = sum;
                }
            }
        }
        return result;
    }

    // Matrix multiplication with quire accumulator for posit
    Matrix<posit32> multiplyWithQuire(const Matrix<posit32>& other) const
    {
        if (cols != other.getRows())
        {
            throw std::runtime_error("Matrix multiplication dimension mismatch (quire)");
        }

        Matrix<posit32> result(rows, other.getCols());
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < other.getCols(); ++j)
            {
                quire32 q;
                q.clr();  // Initialize quire to 0

                for (size_t k = 0; k < cols; ++k)
                {
                    q = q.qma((*this)(i, k), other(k, j));
                }
                result(i, j) = q.toPosit();
            }
        }
        return result;
    }

    // Print matrix (for debugging)
    void print() const
    {
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
            {
                if constexpr (std::is_same_v<T, posit32_t>)
                {
                    std::cout << posit32_to_double((*this)(i, j)) << " ";
                }
                else
                {
                    std::cout << (*this)(i, j) << " ";
                }
            }
            std::cout << std::endl;
        }
    }

    // Matrix scalar multiplication
    Matrix<T> operator*(T scalar) const
    {
        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
            {
                result(i, j) = (*this)(i, j) * scalar;
            }
        }
        return result;
    }
};

// Function to save simulation data to CSV
void saveSimulationData(const std::vector<double>& floatPositions,
                        const std::vector<double>& floatAngles,
                        const std::vector<double>& positPositions,
                        const std::vector<double>& positAngles,
                        const std::vector<double>& quirePositions,
                        const std::vector<double>& quireAngles)
{
    // Create data directory if it doesn't exist
    mkdir("data", 0777);
    mkdir("data/control_theory", 0777);

    // Open CSV file
    std::ofstream csvFile("data/control_theory/simulation_data.csv");

    // Write header
    csvFile << "Time,Float_Position,Float_Angle,Posit_Position,Posit_Angle,Quire_Position,Quire_"
               "Angle\n";

    // Write data
    for (int t = 0; t < TIME_STEPS; ++t)
    {
        csvFile << t * TIME_STEP << "," << floatPositions[t] << "," << floatAngles[t] << ","
                << positPositions[t] << "," << positAngles[t] << "," << quirePositions[t] << ","
                << quireAngles[t] << "\n";
    }

    csvFile.close();
    std::cout << "Simulation data saved to data/control_theory/simulation_data.csv" << std::endl;
}

// Define the inverted pendulum system
template <typename T>
class InvertedPendulumSystem
{
   private:
    // System matrices
    Matrix<T> A;  // State transition matrix
    Matrix<T> B;  // Control matrix
    Matrix<T> K;  // Control gain matrix
    Matrix<T> x;  // State vector [position, velocity, angle, angular velocity]

    // Keep time step
    T dt;

    // Convert double to the template type
    T convertFromDouble(double value)
    {
        if constexpr (std::is_same_v<T, posit32>)
        {
            return posit32(value);
        }
        else
        {
            return static_cast<T>(value);
        }
    }

    // Convert from template type to double for visualization
    double convertToDouble(T value)
    {
        if constexpr (std::is_same_v<T, posit32>)
        {
            return value.toDouble();
        }
        else
        {
            return static_cast<double>(value);
        }
    }

   public:
    InvertedPendulumSystem() : A(4, 4), B(4, 1), K(1, 4), x(4, 1)
    {
        // Initialize system matrices
        // Linearized model of inverted pendulum around the upright position

        // Initialize A matrix
        // x' = Ax + Bu
        // State: [position, velocity, angle, angular velocity]
        T M  = convertFromDouble(CART_MASS);
        T m  = convertFromDouble(POLE_MASS);
        T l  = convertFromDouble(POLE_LENGTH);
        T g  = convertFromDouble(GRAVITY);
        
        dt = convertFromDouble(TIME_STEP);

        // Simplified continuous-time model
        T one   = convertFromDouble(1.0);
        T denom = one / M;

        // A matrix for continuous system
        A(0, 1) = convertFromDouble(1);
        A(1, 2) = convertFromDouble(-1.0) * m * g * denom;
        A(2, 3) = convertFromDouble(1);
        A(3, 2) = g * ((m + M) * denom) / l;

        // B matrix for control input
        B(1, 0) = denom;
        B(3, 0) = -denom / l;

        // Initialize state
        x(0, 0) = convertFromDouble(0.0);   // Initial position
        x(1, 0) = convertFromDouble(0.0);   // Initial velocity
        x(2, 0) = convertFromDouble(0.10);  // Initial angle (small perturbation)
        x(3, 0) = convertFromDouble(0.0);   // Initial angular velocity

        // Initialize K with LQR-like gains (simplified)
        K(0, 0) = convertFromDouble(-1.0);   // Position gain
        K(0, 1) = convertFromDouble(-1.5);   // Velocity gain
        K(0, 2) = convertFromDouble(-20.0);  // Angle gain (high to prioritize angle stabilization)
        K(0, 3) = convertFromDouble(-3.0);   // Angular velocity gain
    }

    // Step the system forward in time
    void step()
    {
        // Compute control input u = -Kx
        Matrix<T> negK = K * convertFromDouble(-1.0);  // Negative feedback
        Matrix<T> u    = negK * x;

        // Calculate change of state
        Matrix<T> dx = A * x + B * u;

        // Update state using Eulers discretization
        x = x + (dx * dt);
    }

    // Special step for posit with quire accumulation
    void stepWithQuire()
    {
        // Only valid for posit32_t
        static_assert(std::is_same_v<T, posit32>, "stepWithQuire is only valid for posit32");

        // Compute control input u = -Kx
        Matrix<T> negK = K * convertFromDouble(-1.0);
        Matrix<T> u    = negK.multiplyWithQuire(x);

        // Calculate change of state.
        Matrix<T> Ax = A.multiplyWithQuire(x);
        Matrix<T> Bu = B.multiplyWithQuire(u);
        Matrix<T> dx = Ax + Bu;

        // Update state using Eulers discretization
        x = x + (dx * dt);
    }

    // Get cart position
    double getPosition()
    {
        return convertToDouble(x(0, 0));
    }

    // Get pendulum angle
    double getAngle()
    {
        return convertToDouble(x(2, 0));
    }

    // Get state for visualization
    Matrix<T> getState()
    {
        return x;
    }
};

// Function to simulate the system and generate visualization
void runSimulation()
{
    // Create systems with different number types
    InvertedPendulumSystem<float>   floatSystem;
    InvertedPendulumSystem<posit32> positSystem;
    InvertedPendulumSystem<posit32> quireSystem;

    // Store trajectory data for visualization
    std::vector<double> floatPositions(TIME_STEPS);
    std::vector<double> floatAngles(TIME_STEPS);
    std::vector<double> positPositions(TIME_STEPS);
    std::vector<double> positAngles(TIME_STEPS);
    std::vector<double> quirePositions(TIME_STEPS);
    std::vector<double> quireAngles(TIME_STEPS);

    // Run simulation
    for (int t = 0; t < TIME_STEPS; ++t)
    {
        // Store current state
        floatPositions[t] = floatSystem.getPosition();
        floatAngles[t]    = floatSystem.getAngle();
        positPositions[t] = positSystem.getPosition();
        positAngles[t]    = positSystem.getAngle();
        quirePositions[t] = quireSystem.getPosition();
        quireAngles[t]    = quireSystem.getAngle();

        // Step the systems
        floatSystem.step();
        positSystem.step();
        quireSystem.stepWithQuire();
    }

    // Save simulation data to CSV
    saveSimulationData(
        floatPositions, floatAngles, positPositions, positAngles, quirePositions, quireAngles);

    // Calculate errors
    double maxPositError = 0.0;
    double maxQuireError = 0.0;
    double avgPositError = 0.0;
    double avgQuireError = 0.0;

    for (int t = 0; t < TIME_STEPS; ++t)
    {
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
    std::cout << "Float vs Posit - Max Error: " << maxPositError << ", Avg Error: " << avgPositError
              << std::endl;
    std::cout << "Float vs Quire - Max Error: " << maxQuireError << ", Avg Error: " << avgQuireError
              << std::endl;
}

int main()
{
    try
    {
        std::cout << "Running Inverted Pendulum Control System Simulation..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        runSimulation();

        auto                          end     = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Simulation completed in " << elapsed.count() << " seconds." << std::endl;

        std::cout << "Example completed successfully." << std::endl;
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}