#pragma once
#include <cstdlib>
#include <fstream>
#include <iostream>

/// @class GpioService
/// @brief A service class for managing GPIO interactions, including reading button and luminosity values.
class GpioService{
public:
    /// @brief Constructs the GpioService object and initializes GPIO configurations.
    GpioService();

    /// @brief Destructs the GpioService object.
    ~GpioService();

    /// @brief Reads the button value from the GPIO file and returns it.
    /// @return The value of the button (0 or 1).
    int getButtonValue();

    /// @brief Reads the luminosity value from the ADC (analog-to-digital converter) file and returns it.
    /// @return The luminosity value as a double.
    double getLuminosity();

private:
    /// @brief Reads the button value from the GPIO file and updates `_buttonValue`.
    void readButtonValue();

    /// @brief Reads the luminosity value from the ADC file and updates `_luminosity`.
    void readLuminosityValue();

    /// @brief The most recently read button value (0 or 1).
    int _buttonValue{0};
    
    /// @brief The most recently read analog luminosity value.
    double _luminosity{0.0};
};


