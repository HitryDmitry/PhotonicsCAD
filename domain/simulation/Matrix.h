#pragma once

#include <vector>

// Класс для хранения значений в форме матрицы
class Matrix
{
public:
    Matrix(int dim = 1)
        : mDim(dim)
    {
        mMatrix = std::vector<std::vector<double>>(dim, std::vector<double>(dim, 0.0));
    }

    void setElement(int rowIdx, int colIdx, double val) { mMatrix.at(rowIdx).at(colIdx) = val; }
    double getElement(int rowIdx, int colIdx) const { return mMatrix.at(rowIdx).at(colIdx); }

private:
    int mDim;
    std::vector<std::vector<double>> mMatrix;
};
