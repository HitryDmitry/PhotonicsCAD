#pragma once

#include <vector>

// Класс, описывающий коэффициенты рассеяния по мощности
class SPowerMatrix
{
public:
    SPowerMatrix(int dim = 1)
        : mDim(dim)
    {
        mPowMatrix = std::vector<std::vector<double>>(dim, std::vector<double>(dim, 0.0));
    }

    void setElement(int rowIdx, int colIdx, double val) { mPowMatrix.at(rowIdx).at(colIdx) = val; }
    double getElement(int rowIdx, int colIdx) const { return mPowMatrix.at(rowIdx).at(colIdx); }

private:
    int mDim;
    std::vector<std::vector<double>> mPowMatrix;
};
