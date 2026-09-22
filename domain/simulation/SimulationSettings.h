struct FrequencySweepSettings
{
    double startFrequency_Hz;
    double stopFrequency_Hz;
    int numPoints;
    enum class SweepType { Linear, Logarithmic };
    SweepType type = SweepType::Linear;
    // Можно добавить центр и диапазон, как в INTERCONNECT
    // double centerFrequency_Hz;
    // double frequencyRange_Hz;
};

class SimulationSettings
{
public:
    FrequencySweepSettings frequencySweep;
    // ... другие настройки (временное окно, тип анализа и т.д.)
};