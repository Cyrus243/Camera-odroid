#pragma once
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <opencv2/opencv.hpp>
#include "QrManager.h"


/// @class TcpService
/// @brief Handle TCP client-server connections.
class TcpService{
public:
    /// @brief Constructs a TcpService object and initializes the server to listen on the specified port.
    /// @param server_address IP address of the server as a string.
    /// @param _port The port on which the server will listen for incoming connections.
    TcpService(const std::string& server_address);

    /// @brief Destructor for TcpService, closes any open connections and releases resources.
    ~TcpService();

    /// @brief Closes the client connection and frees the associated socket resources.
    void CloseAllConnections();

    /// @brief Closes the last server connection and updates internal lists.
    void closeLastConnection();

    /// @brief Sends a message to the server.
    /// @param message The message to send to the server.
    /// @return true if the message was sent successfully, false otherwise.
    bool SendMessage(const std::string &message, int serverId);

    /// @brief Establishes a connection to the server.
    /// @return true if the connection was successfully established, false otherwise.
    bool ConnectToServer(int port);

    /// @brief Observes and receives responses from the server.
    /// @return true if the response was successfully received, false otherwise.
    bool ObserveServerResponse();

    /// @brief Prompts the user to select a video resolution from a predefined list.
    /// @return The chosen resolution in the format "WidthxHeight" (e.g., "800x600").
    std::string ChooseResolution();

    /// @brief Retrieves the resolution dimensions (width and height) based on the given index.
    /// @param index The index corresponding to a resolution option (0-based).
    /// @return The resolution in the format "WidthxHeight" (e.g., "800x600").
    std::string GetVideoResolution(int index);

    /// @brief Receives and returns a response from the server as a string.
    /// @return The received server response, or an empty string on error or connection closure.
    std::string WaitServerResponse(int serverId);

private:
    /// @brief Server's IP address
    std::string _server_address;

    /// @brief Server's port
    std::vector<int> _port;

    /// @brief Client socket ID
    std::vector<int> _serverId;

    /// @brief Manages QR code-related operations.
    QrManager _qrManager;
};
