#include <iostream>
#include "TcpService.h"

/*
Main entry for TCP client app: initializes TcpService with server IP and port, 
connects to server, starts observing for image data on success, prints error on failure.
*/
int main(int argc, char **argv){
    // Create a TcpService client instance with server IP address "192.168.7.2" and port 4099.
    TcpService client("192.168.7.2");

    // Attempt to connect to the server
    if (client.ConnectToServer(4099)){
        // Start observing and displaying server responses (images).
        client.ObserveServerResponse();
    } 
    else{
        // Display an error message if the connection fails.
        std::cerr << "Failed to connect to server\n";
    }

    return 0;
}