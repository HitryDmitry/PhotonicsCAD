#pragma once
#include "IComponentModel.h"

class LaserModel : public IComponentModel {
private:
    double mPower;
    double mFreq;
    double mRIN;

public:
    LaserModel(double power, double freq, double RIN)
        : mPower(power)
        , mFreq(freq)
        , mRIN(RIN)
    {}
    std::complex<double> transferFunction(double frequency) override
    {
        // Пока что предполагаем, что лазер излучает строго на одной частоте
        if (frequency == mFreq) {
            return {std::sqrt(mPower), 0.0};
        }
        return {0.0, 0.0};
    }

    double getPower() { return mPower; }
    double getFreq() { return mFreq; }
    double getRIN() { return mRIN; }
};

class FiberModel : public IComponentModel
{
private:
    double mLength;          // длина в метрах
    double mDampDecrement;   // декремент затухания (1/м)
    double mRefractiveIndex; // показатель преломления

public:
    FiberModel(double length, double dampingDecrement, double refractiveIndex)
        : mLength(length)
        , mDampDecrement(dampingDecrement)
        , mRefractiveIndex(refractiveIndex)
    {}

    std::complex<double> transferFunction(double frequency) override
    {
        // Расчет потерь: exp(-α * L)
        double loss = exp(-mDampDecrement * mLength);

        // Фазовая задержка: exp(-j * β * L)
        double beta = (2.0 * M_PI * frequency * mRefractiveIndex) / 3e8; // скорость света
        double phase = -beta * mLength;

        return loss * std::complex<double>(cos(phase), sin(phase));
    }
};

class SplitterModel : public IComponentModel
{
private:
    double mSplitRatio;

public:
    SplitterModel(double splitRatio)
        : mSplitRatio(splitRatio)
    {}

    std::complex<double> transferFunction(double frequency) override { return {}; }
};
