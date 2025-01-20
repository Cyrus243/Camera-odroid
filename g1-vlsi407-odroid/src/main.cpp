#include <iostream>
#include "Camera.h"
#include <vector>
#include <string>
#include "TcpService.h"
#include "GpioService.h"

namespace {
  // Define server status
  constexpr auto READY = "R";
  constexpr auto NO_LIGHT = "N";
  constexpr auto BUTTON_PUSHED = "B";

  constexpr auto freqPath = "/sys/devices/pwm-ctrl.42/freq0";
  constexpr auto enablePath = "/sys/devices/pwm-ctrl.42/enable0";

  constexpr auto ditLength = 60*1000;
  constexpr auto frequency = 660;

    std::map<char, std::string> morseCode = {
        {'a', ".-"},    {'b', "-..."},  {'c', "-.-."},  {'d', "-.."},
        {'e', "."},     {'f', "..-."},  {'g', "--."},   {'h', "...."},
        {'i', ".."},    {'j', ".---"},  {'k', "-.-"},   {'l', ".-.."},
        {'m', "--"},    {'n', "-."},    {'o', "---"},   {'p', ".--."},
        {'q', "--.-"},  {'r', ".-."},   {'s', "..."},   {'t', "-"},
        {'u', "..-"},   {'v', "...-"},  {'w', ".--"},   {'x', "-..-"},
        {'y', "-.--"},  {'z', "--.."},
        {'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
        {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."},
        {'8', "---.."}, {'9', "----."}
    };
}

/*
Translates a string into Morse code and stores the result in a vector.
Spaces are added as empty strings; unmapped characters are ignored.
*/
void strToMorse(std::string& sentence, std::vector<std::string>& morseResult){
  for (auto c: sentence){
    if (c == ' '){
      morseResult.push_back("");
    }else{
      if (morseCode.count(c)){
        morseResult.push_back(morseCode[c]);
      }
    }
  }
}

/*
Plays a sequence of Morse code from a vector of strings using GPIO control.
Dots and dashes are represented by activating the GPIO for different durations,
while spaces between codes are represented by delays.
*/
void playMorseCode(std::vector<std::string> &morseStr){

	std::ofstream enable0(enablePath);

	if (enable0.fail()){
        std::cout << "Failed to open enable0" << std::endl;
		exit(-1);
	}	
	
  for (auto code: morseStr){
    for (auto c: code){
      switch (c){
        case '.':{
          enable0 << 1 << std::endl;
          usleep(ditLength);
          enable0 << 0 << std::endl;
        }
          break;
        case '-':{
          enable0 << 1 << std::endl;
          usleep(3*ditLength);
          enable0 << 0 << std::endl;
        }
        break;
          
        default:
          usleep(6*ditLength);
          break;
      }
      usleep(2*ditLength);
    }
  }
 
  enable0 << 1 << std::endl;
  sleep(1);
  enable0 << 0 << std::endl;

  enable0.close();
}

int main(int argc, char **argv){
  int choice;
  // Initialize the TCP server on port 4099
  TcpService server;
  // Initialize the camera with the selected resolution
  Camera cameraService;
  // Initialize the GPIO service to handle button and luminosity inputs
  GpioService gpios;
  auto serverState = READY;

    //Start the server and wait for a client connection
    if (server.Start(4099)) {
      std::cout << "Server started and connected to the first client!\n";

      while (true){
        // Get luminosity reading from GPIO
        auto luminosity = gpios.getLuminosity();

        // Update server state based on luminosity level
        if (luminosity > 1000){
          serverState = NO_LIGHT;
        }else{
          serverState = READY;
        }
        // Send the current server state to the client
        server.SendMessage(serverState, server.getClientId(0));
        // Wait for acknowledgment or further input from the client
        auto ack2 = server.WaitUserEntry(server.getClientId(0));

        if (serverState==READY){
          auto entry = server.WaitUserEntry(server.getClientId(0));

          // Check button state and notify the client if the button is pressed, else send "z"
          if (gpios.getButtonValue() == 1){
            server.SendMessage(BUTTON_PUSHED, server.getClientId(0));
            // Create a child process using fork.
            auto pid = fork();
            if (pid == 0){
              // Start the server on port 4098.
              if (server.Start(4098)){
                std::cout << "Server started and connected to a the second client!\n";
                // Wait for the second client to enter a sentence.
                auto sentence = server.WaitUserEntry(server.getClientId(1));

                if(sentence == ""){
                  std::cout << "The given sentence is empty!" << std::endl;
                  exit(0);
                }
                server.closeLastConnection();
                
                // Open the frequency control file to set the playback frequency.
                std::ofstream freq0(freqPath);
                if (freq0.fail()) {
                  std::cout << "Failed to open freq0" << std::endl;
                	exit(0);
                }

                // Write the desired frequency to the file.
                freq0 << frequency << std::endl;
                freq0.close();

                // Convert the received sentence to Morse code.
                std::vector<std::string> morseSentence;
                strToMorse(sentence, morseSentence);

                // Play the Morse code using GPIO controls.
                playMorseCode(morseSentence);
                exit(0);
              }

            }
          }else {
            server.SendMessage("z", server.getClientId(0));
          }
          auto ack = server.WaitUserEntry(server.getClientId(0));
          // Capture an image from the camera and send it to the client
          server.SendImage(cameraService.CaptureImage());
          ack = server.WaitUserEntry(server.getClientId(0));
          
          // Exit the loop if the entry is 'q'
          if (ack== "q"){
            break;
          // Wait for resolution input to update camera resolution
          }else if(ack == "r"){
            std::cout << "Client wanna change resolution" << std::endl;
            auto resolution =  server.WaitUserEntry(server.getClientId(0));
            cameraService.UpdateResolution(resolution);
          }
        }else{
          // Recheck light conditions to transition back to READY state if possible
          if (luminosity < 1000 && luminosity > 0){
            serverState = READY;
          }
        }
      }
    } else{
        std::cerr << "Failed to start server\n";
    }
  return 0;
}