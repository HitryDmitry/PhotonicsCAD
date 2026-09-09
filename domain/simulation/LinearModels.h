#pragma once
#include "Constants.h"
#include "IComponentModel.h"

#include <complex>

class LaserModel : public IComponentModel {
private:
    double mPowerWatt;
    double mFreqHz;
    double mRIN;

    int mNumPins;

public:
    LaserModel(double mWpower, double freqGHz, double RIN, int numPins)
        : mPowerWatt(mWpower * Units::mW)
        , mFreqHz(freqGHz * Units::GHz)
        , mRIN(RIN)
        , mNumPins(numPins)

    {}
    Matrix<double> transferFunction(double frequencyHz) override
    {
        Matrix<double> result(mNumPins, mNumPins);
        // Пока что предполагаем, что лазер излучает строго на одной частоте
        if (frequencyHz == mFreqHz) {
            result(0, 0) = mPowerWatt;
        }
        return result;
    }

    double getPowerWatt() { return mPowerWatt; }
    double getFreqHz() { return mFreqHz; }
    double getRIN() { return mRIN; }
};

class FiberModel : public IComponentModel
{
private:
    double mLengthMeters;    // длина в метрах
    double mDampDecrement;   // декремент затухания (1/м)
    double mRefractiveIndex; // показатель преломления
    int mNumPins;

public:
    FiberModel(double lengthMeters, double dampingDecrement, double refractiveIndex, int numPins)
        : mLengthMeters(lengthMeters)
        , mDampDecrement(dampingDecrement)
        , mRefractiveIndex(refractiveIndex)
        , mNumPins(numPins)
    {}

    Matrix<double> transferFunction(double frequencyHz) override
    {
        // Расчет потерь (по мощности): exp(-2 * α * L)
        double loss = exp(-2.0 * mDampDecrement * mLengthMeters);

        // Фазовая задержка: exp(-j * β * L)
        double beta = (2.0 * M_PI * frequencyHz * mRefractiveIndex) / Physics::C; // скорость света
        double phase = -beta * mLengthMeters;

        double S21 = std::abs(loss * std::complex<double>(cos(phase), sin(phase)));

        Matrix<double> result(2, 2);
        result(0, 0) = 0.0; // Отражение на входе (идеальное согласование)
        result(0, 1) = 0.0; // Обратная передача
        result(1, 0) = S21; // Прямая передача
        result(1, 1) = 0.0; // Отражение на выходе (идеальное согласование)

        return result;
    }
};

class SplitterModel : public IComponentModel
{
private:
    double mSplitRatio;
    int mNumPins;

public:
    SplitterModel(double splitRatio, int numPins)
        : mSplitRatio(splitRatio)
        , mNumPins(numPins)
    {}

    Matrix<double> transferFunction(double frequencyHz) override
    {
        Matrix<double> result(mNumPins, mNumPins);
        double S21Sqr = mSplitRatio;
        double S31Sqr = 1 - mSplitRatio;

        result(0, 0) = 0.0; // (S11)^2 - Отражение на входе (идеальное согласование)
        result(0, 1) = 0.0; // (S12)^2 Обратная передача со второго порта на вход
        result(0, 2) = 0.0; // (S13)^2 Обратная передача с третьего порта на вход

        result(1, 0) = S21Sqr; // (S21)^2
        result(1, 1) = 0.0;    // (S22)^2
        result(1, 1) = 0.0;    // (S23)^2

        result(1, 1) = S31Sqr; // (S31)^2
        result(1, 1) = 0.0;    // (S32)^2
        result(1, 1) = 0.0;    // (S33)^2
        return result;
    }
};
