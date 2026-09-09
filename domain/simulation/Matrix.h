#pragma once

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

template<typename T>
class Matrix
{
private:
    size_t m_rows;
    size_t m_cols;
    std::vector<T> m_data; // Contiguous memory storage

public:
    // Constructor
    Matrix(size_t rows, size_t cols, const T &initial_value = T())
        : m_rows(rows)
        , m_cols(cols)
        , m_data(rows * cols, initial_value)
    {}

    Matrix(std::vector<T> vec)
        : m_rows(1)
        , m_cols(vec.size())
        , m_data(std::move(vec))
    {
        if (m_cols == 0) {
            throw std::invalid_argument("Вектор не может быть пустым.");
        }
    }

    // Element access (Getter/Setter) - Zero-indexed
    T &operator()(size_t row, size_t col)
    {
        assert(row < m_rows && col < m_cols); // Debug check
        return m_data[row * m_cols + col];    // Row-major mapping
    }

    // Read-only element access for const objects
    const T &operator()(size_t row, size_t col) const
    {
        assert(row < m_rows && col < m_cols);
        return m_data[row * m_cols + col];
    }

    // Getters for dimensions
    size_t rows() const { return m_rows; }
    size_t cols() const { return m_cols; }

    // --- Matrix Operations ---

    // Matrix Addition
    Matrix operator+(const Matrix &other) const
    {
        if (m_rows != other.m_rows || m_cols != other.m_cols) {
            throw std::invalid_argument("Matrix dimensions must match for addition.");
        }

        Matrix result(m_rows, m_cols);
        for (size_t i = 0; i < m_data.size(); ++i) {
            result.m_data[i] = this->m_data[i] + other.m_data[i];
        }
        return result;
    }

    // Matrix Multiplication (Dot Product)
    Matrix operator*(const Matrix &other) const
    {
        if (m_cols != other.m_rows) {
            throw std::invalid_argument("Matrix dimensions incompatible for multiplication.");
        }

        Matrix result(m_rows, other.m_cols, 0);
        // Optimized loop ordering (i, k, j) for high CPU cache utilization
        for (size_t i = 0; i < m_rows; ++i) {
            for (size_t k = 0; k < m_cols; ++k) {
                T temp = (*this)(i, k);
                for (size_t j = 0; j < other.m_cols; ++j) {
                    result(i, j) += temp * other(k, j);
                }
            }
        }
        return result;
    }

    // Transpose Matrix
    Matrix transpose() const
    {
        Matrix result(m_cols, m_rows);
        for (size_t i = 0; i < m_rows; ++i) {
            for (size_t j = 0; j < m_cols; ++j) {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }

    // Helper to print matrix
    void print() const
    {
        for (size_t i = 0; i < m_rows; ++i) {
            for (size_t j = 0; j < m_cols; ++j) {
                std::cout << (*this)(i, j) << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
};
