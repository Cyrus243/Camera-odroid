
#include "GpioService.h"

namespace{
    auto gpioPath = "/sys/class/gpio/export";
	auto directionPath = "/sys/class/gpio/gpio228/direction";
	auto buttonValuePath = "/sys/class/gpio/gpio228/value";
	auto adcPath = "/sys/class/saradc/ch0";
}

/*
Constructs the GpioService object and initializes the GPIO pin configuration.
Configures the GPIO pin by writing to the system's export and direction files.
Sets the pin direction to input ("in").
Exits the program with an error message if any file operation fails.
*/
GpioService::GpioService(){
	std::ofstream gpio(gpioPath);

	if (gpio.fail()) {
        std::cout << "Failed to open gpio" << std::endl;
		exit(-1);
	}	
	gpio << 228 << std::endl;

	
	std::ofstream direction(directionPath);
	if (direction.fail()) {
        std::cout << "Failed to open direction file" << std::endl;
		exit(-1);
	}	
	direction << "in" << std::endl;

	gpio.close();
	direction.close();
}

/*
Reads the button value from the GPIO pin file and updates the _buttonValue member variable.
Opens the button value file and reads its contents.
Updates _buttonValue with the value read from the file.
Logs an error message and exits the program if the file cannot be accessed or the value cannot be read.
*/

/// source: ChatGpt (copie tel quel)
void GpioService::readButtonValue() {
    static int lastButtonState = 1; // Initialement relâché (high).
    std::ifstream buttonValue(buttonValuePath, std::ifstream::in);

    if (buttonValue.fail()) {
        std::cout << "Failed to open button value file" << std::endl;
        exit(-1);
    }

    int currentButtonState = 1; // Par défaut, on suppose que le bouton est relâché.

    if (buttonValue >> currentButtonState) {
        // Vérification d'une transition "appuyé" (0) -> "relâché" (1)
        if (lastButtonState == 1 && currentButtonState == 0) {
            _buttonValue = 1; // Mettre à jour l'état en conséquence
        } else {
            _buttonValue = 0; // Pas de transition détectée
        }

        // Mettre à jour l'état précédent
        lastButtonState = currentButtonState;
    } else {
        std::cout << "Failed to read button value" << std::endl;
    }
}


// Returns the current value of the button by calling readButtonValue to update _buttonValue.
int GpioService::getButtonValue(){
	readButtonValue();
	return _buttonValue;
}

/*
Reads the luminosity value from the ADC file and updates the _luminosity member variable.
Opens the ADC file and reads its contents.
Updates _luminosity with the value read from the file.
Logs an error message and exits the program if the file cannot be accessed or the value cannot be read.
*/
void GpioService::readLuminosityValue(){
	std::ifstream resistance(adcPath, std::ifstream::in);

	if (resistance.fail()) 
	{
        std::cout << "Failed to open resistance file" << std::endl;
		exit(-1);
	}	
	auto value = 0.0;
	if (resistance >> value){
		_luminosity = value;
	}else{
		std::cout << "Failed to read resistance value " << std::endl;
	}
}

// Returns the current luminosity value by calling readLuminosityValue to update _luminosity.
double GpioService::getLuminosity(){
	readLuminosityValue();
	return _luminosity;
}

// Destroys the GpioService object.
GpioService::~GpioService(){}