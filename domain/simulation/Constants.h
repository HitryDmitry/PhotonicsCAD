// Constants.h
#pragma once

#include <cmath>

namespace Constants {
// ============================================================================
// Фундаментальные физические константы (CODATA 2018)
// ============================================================================
namespace Physics {
// Скорость света в вакууме [м/с]
constexpr double C = 299792458.0;

// Постоянная Планка [Дж·с]
constexpr double H = 6.62607015e-34;

// Редуцированная постоянная Планка [Дж·с]
constexpr double HBAR = 1.054571817e-34;

// Заряд электрона [Кл]
constexpr double E = 1.602176634e-19;

// Постоянная Больцмана [Дж/К]
constexpr double K_B = 1.380649e-23;

// Электрическая постоянная [Ф/м]
constexpr double EPSILON_0 = 8.8541878128e-12;

// Магнитная постоянная [Гн/м]
constexpr double MU_0 = 1.25663706212e-6;

// Импеданс свободного пространства [Ом]
constexpr double Z_0 = 376.730313668;
} // namespace Physics

// ============================================================================
// Преобразование единиц измерения
// ============================================================================
namespace Units {
// --- Частота ---
inline constexpr double Hz = 1.0;
inline constexpr double kHz = 1e3;
inline constexpr double MHz = 1e6;
inline constexpr double GHz = 1e9;
inline constexpr double THz = 1e12;

// --- Длина ---
inline constexpr double m = 1.0;
inline constexpr double cm = 1e-2;
inline constexpr double mm = 1e-3;
inline constexpr double um = 1e-6;
inline constexpr double nm = 1e-9;
inline constexpr double km = 1e3;

// --- Время ---
inline constexpr double s = 1.0;
inline constexpr double ms = 1e-3;
inline constexpr double us = 1e-6;
inline constexpr double ns = 1e-9;
inline constexpr double ps = 1e-12;
inline constexpr double fs = 1e-15;

// --- Мощность ---
inline constexpr double W = 1.0;
inline constexpr double mW = 1e-3;
inline constexpr double uW = 1e-6;
inline constexpr double nW = 1e-9;

// --- Напряжение ---
inline constexpr double V = 1.0;
inline constexpr double mV = 1e-3;
inline constexpr double uV = 1e-6;

// --- Ток ---
inline constexpr double A = 1.0;
inline constexpr double mA = 1e-3;
inline constexpr double uA = 1e-6;
inline constexpr double nA = 1e-9;

// --- Сопротивление ---
inline constexpr double Ohm = 1.0;
inline constexpr double kOhm = 1e3;
inline constexpr double MOhm = 1e6;
} // namespace Units

// ============================================================================
// Удобные функции для конвертации
// ============================================================================
namespace Convert {
// Частота <-> длина волны
inline constexpr double freq_to_wavelength(double freq_hz)
{
    return Constants::Physics::C / freq_hz;
}

inline constexpr double wavelength_to_freq(double wavelength_m)
{
    return Constants::Physics::C / wavelength_m;
}

// Частота в ГГц <-> длина волны в нм
inline constexpr double freq_ghz_to_wavelength_nm(double freq_ghz)
{
    return (Constants::Physics::C / (freq_ghz * Constants::Units::GHz)) / Constants::Units::nm;
}

inline constexpr double wavelength_nm_to_freq_ghz(double wavelength_nm)
{
    return (Constants::Physics::C / (wavelength_nm * Constants::Units::nm)) / Constants::Units::GHz;
}

// ДБ <-> линейные единицы
inline double db_to_linear(double db)
{
    return std::pow(10.0, db / 10.0);
}

inline double linear_to_db(double linear)
{
    return 10.0 * std::log10(linear);
}
} // namespace Convert

// ============================================================================
// Оптические константы для фотоники
// ============================================================================
namespace Optics {
// Стандартные длины волн [нм]
namespace Wavelength {
constexpr double C_BAND_CENTER = 1550.0; // [нм]
constexpr double C_BAND_MIN = 1530.0;
constexpr double C_BAND_MAX = 1578.0;
constexpr double L_BAND_CENTER = 1590.0;
constexpr double O_BAND_CENTER = 1310.0;
constexpr double VISIBLE_RED = 632.8;
constexpr double VISIBLE_GREEN = 532.0;
constexpr double VISIBLE_BLUE = 473.0;
} // namespace Wavelength

// Стандартные частоты [ГГц]
namespace Frequency {
constexpr double C_BAND_CENTER = Convert::wavelength_nm_to_freq_ghz(Wavelength::C_BAND_CENTER);
constexpr double C_BAND_MIN = Convert::wavelength_nm_to_freq_ghz(
    Wavelength::C_BAND_MAX); // обратная зависимость
constexpr double C_BAND_MAX = Convert::wavelength_nm_to_freq_ghz(Wavelength::C_BAND_MIN);
constexpr double L_BAND_CENTER = Convert::wavelength_nm_to_freq_ghz(Wavelength::L_BAND_CENTER);
constexpr double O_BAND_CENTER = Convert::wavelength_nm_to_freq_ghz(Wavelength::O_BAND_CENTER);
} // namespace Frequency

// Стандартные параметры волокна
namespace Fiber {
constexpr double ATTENUATION_1550NM = 0.2;  // [дБ/км]
constexpr double ATTENUATION_1310NM = 0.35; // [дБ/км]
constexpr double REFRACTIVE_INDEX = 1.45;   // [безразм.]
constexpr double DISPERSION_1550NM = 16.0;  // [пс/(нм·км)]
constexpr double DISPERSION_1310NM = 0.0;   // [пс/(нм·км)]
constexpr double NONLINEAR_INDEX = 2.6e-20; // [м²/Вт]
constexpr double EFFECTIVE_AREA = 80.0;     // [мкм²]
} // namespace Fiber

// Стандартные параметры модуляторов
namespace Modulator {
constexpr double TYPICAL_V_PI = 5.0;           // [В]
constexpr double TYPICAL_INSERTION_LOSS = 3.0; // [дБ]
constexpr double TYPICAL_ER = 20.0;            // [дБ]
constexpr double TYPICAL_BANDWIDTH = 40.0;     // [ГГц]
} // namespace Modulator

// Стандартные параметры фотодетекторов
namespace Photodetector {
constexpr double RESPONSIVITY_1550NM = 0.9; // [А/Вт]
constexpr double RESPONSIVITY_1310NM = 0.8; // [А/Вт]
constexpr double DARK_CURRENT = 1.0e-9;     // [А]
constexpr double LOAD_RESISTANCE = 50.0;    // [Ом]
constexpr double CAPACITANCE = 0.5e-12;     // [Ф]
} // namespace Photodetector
} // namespace Optics
} // namespace Constants

// Для удобства использования в коде
using namespace Constants;