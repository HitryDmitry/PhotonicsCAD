#pragma once
#include "Constants.h"
#include "IComponentModel.h"

class LaserModel : public IComponentModel {
private:
    double mPowerWatt;
    double mFreqHz;
    double mRIN;

public:
    LaserModel(double mWpower, double freqGHz, double RIN)
        : mPowerWatt(mWpower * Units::mW)
        , mFreqHz(freqGHz * Units::GHz)
        , mRIN(RIN)
    {}
    std::complex<double> transferFunction(double frequencyHz) override
    {
        // Пока что предполагаем, что лазер излучает строго на одной частоте
        if (frequencyHz == mFreqHz) {
            return {mPowerWatt, 0.0};
        }
        return {0.0, 0.0};
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

public:
    FiberModel(double lengthMeters, double dampingDecrement, double refractiveIndex)
        : mLengthMeters(lengthMeters)
        , mDampDecrement(dampingDecrement)
        , mRefractiveIndex(refractiveIndex)
    {}

    std::complex<double> transferFunction(double frequencyHz) override
    {
        // Расчет потерь (по мощности): exp(-2 * α * L)
        double loss = exp(-2.0 * mDampDecrement * mLengthMeters);

        // Фазовая задержка: exp(-j * β * L)
        double beta = (2.0 * M_PI * frequencyHz * mRefractiveIndex) / Physics::C; // скорость света
        double phase = -beta * mLengthMeters;

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

    std::complex<double> transferFunction(double frequencyHz) override { return {}; }
};
