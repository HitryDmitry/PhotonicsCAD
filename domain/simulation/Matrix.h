#pragma once

#include <stdexcept>
#include <vector>

// Класс для хранения значений в форме матрицы
class Matrix
{
public:
    Matrix(int rows = 1, int cols = 1)
        : mRows(rows)
        , mCols(cols)
    {
        mMatrix.resize(rows, std::vector<double>(cols, 0.0));
    }

    Matrix(std::initializer_list<std::vector<double>> rows)
    {
        mRows = rows.size();
        if (mRows > 0) {
            mCols = rows.begin()->size();
        } else {
            mCols = 0;
        }

        mMatrix.resize(mRows, std::vector<double>(mCols));

        size_t rowIdx = 0;
        for (const auto &row : rows) {
            if (row.size() != mCols) {
                throw std::invalid_argument("All rows must have the same number of columns");
            }
            for (size_t colIdx = 0; colIdx < mCols; ++colIdx) {
                mMatrix[rowIdx][colIdx] = row[colIdx];
            }
            ++rowIdx;
        }
    }

    void setElement(int rowIdx, int colIdx, double val) { mMatrix.at(rowIdx).at(colIdx) = val; }
    double getElement(int rowIdx, int colIdx) const { return mMatrix.at(rowIdx).at(colIdx); }

private:
    int mRows;
    int mCols;
    std::vector<std::vector<double>> mMatrix;
};
