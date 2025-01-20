#pragma once
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <vector>


/// @class TcpService
/// @brief Manages TCP server functionalities to handle client connections and image transmissions.
class TcpService{
public:
    /// @brief Destructor for TcpService, closes any open connections and releases resources.
    ~TcpService();

    /// @brief Closes the client and server connection and frees the associated socket resources.
    void CloseAllConnections();
    
    /// @brief Closes the last client and server connections if multiple exist.
    void closeLastConnection();

    /// @brief etrieves the client ID at the specified index in the _clientId list.
    /// @param index The position of the client ID in the _clientId list.
    /// @return The client ID at the specified index.
    int getClientId(int index) const;

    /// @brief Initializes the TCP server and prepares it to accept client connections.
    /// @return `true` if the server successfully starts and accepts a client connection; `false` otherwise.
    bool Start(int port);

    /// @brief Waits for and retrieves a single character input from the connected client.
    /// @return The first character of the received message from the client.
    std::string WaitUserEntry(int waitingTime);

    /// @brief Sends the provided message to the connected client.
    /// @param message The string message to send to the client.
    /// @return `true` if the message was successfully sent, `false` otherwise.
    bool SendMessage(const std::string &message, int clientId);

    /// @brief Sends an image to the connected client via the established TCP connection.
    /// @param image The OpenCV `cv::Mat` image to be sent.
    /// @return `true` if the image is sent successfully, `false` otherwise.
    bool SendImage(const cv::Mat& image);

private:
    /// @brief Server's port
    std::vector<int> _port;

    /// @brief Server socket ID
    std::vector<int> _serverId;
    
    /// @brief Client socket ID
    std::vector<int> _clientId;
};