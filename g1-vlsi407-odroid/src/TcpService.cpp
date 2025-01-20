#include "TcpService.h"

// Returns the client ID at the specified index from the _clientId list.
int TcpService::getClientId(int index) const{
    return _clientId[index];
}

// Closes the active client and server connections if they exist, then resets their IDs to -1
void TcpService::CloseAllConnections(){
    if (!_clientId.empty()){
        for (auto id: _clientId){
            close(id);
            auto index = std::find(_clientId.begin(), _clientId.end(), id);
            _clientId.erase(index);
        }
    }

    if (!_serverId.empty()){
        for (auto id: _serverId){
            close(id);
            auto index = std::find(_serverId.begin(), _serverId.end(), id);
            _serverId.erase(index);
        }
    }
}

/*
Closes the last client and server connection if multiple connections exist.
Removes the corresponding ID from the _clientId and _serverId lists.
*/
void TcpService::closeLastConnection(){
    if (!_clientId.empty() && _clientId.size() > 1){
        close(_clientId.back());
        _clientId.pop_back(); 
    }

    if (!_serverId.empty() && _serverId.size() > 1){
        close(_serverId.back());
        _serverId.pop_back();
    }
}


/*
Starts the server by creating a socket, binding it to the IP and port, and setting it to listen for connections.
Returns false if any step fails, printing an error message.
*/
bool TcpService::Start(int port){
    // Create a TCP socket
    auto serverId = socket(AF_INET, SOCK_STREAM, 0);
    auto opt = 1;

    if (serverId == -1){
        std::cerr << "Failed to create socket\n";
        return false;
    }

    if (setsockopt(serverId, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        std::cerr << "Failed on setsockopt\n";
        exit(0);
    }

    // Bind the socket to an IP address and port
    sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(serverId, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        std::cerr << "Failed to bind socket\n";
        return false;
    }

    // Listen for incoming connections
    if (listen(serverId, 5) < 0){
        std::cerr << "Failed to listen on socket\n";
        return false;
    }
    std::cout << "Listening on port " << port << "...\n";
    // Accept a connection
    sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    auto clientId = accept(serverId, (struct sockaddr *)&client_address, &client_address_len);

    if (clientId < 0){
        std::cerr << "Failed to accept connection\n";
        return false;
    }
    std::cout << "Connection accepted\n";
    _serverId.push_back(serverId);
    _clientId.push_back(clientId);
    _port.push_back(port);

    return true;
}

/*
Waits for the client to send data and returns the first character of the received message.
If the connection closes or an error occurs, it exits the loop.
*/
std::string TcpService::WaitUserEntry(int clientId) {
    std::vector<char> buffer(1024); // Utilisation d'un vecteur pour permettre un redimensionnement dynamique
    std::string result = "";

    while (true) {
        memset(buffer.data(), 0, buffer.size()); // Réinitialise le tampon
        ssize_t bytes_received = recv(clientId, buffer.data(), buffer.size(), 0);

        if (bytes_received < 0) {
            std::cerr << "Error occurred while receiving data\n";
            break;
        } else if (bytes_received == 0) {
            std::cerr << "Connection closed by client\n";
            break;
        }

        // Ajoute les données reçues au résultat
        result.append(buffer.data(), bytes_received);

        // Vérifie si la fin du message est atteinte
        if (bytes_received < static_cast<ssize_t>(buffer.size())) {
            // On suppose que la fin d'un message est atteinte si `recv` retourne moins que la taille du tampon
            break;
        }
    }

    return result;
}


/*
Sends an image to the client over the TCP connection.
First sends the image size, then the image data.
Returns true if both send operations are successful.
*/
bool TcpService::SendImage(const cv::Mat &image){
    std::vector<uchar> buffer;
    cv::imencode(".jpg", image, buffer);

    // Send size of the encoded image first
    auto size = buffer.size();
    if (send(_clientId[0], &size, sizeof(int), 0) == -1){
        std::cerr << "Failed to send image size\n";
        return false;
    }

    // Send the image data
    if (send(_clientId[0], buffer.data(), size, 0) == -1){
        std::cerr << "Failed to send image data\n";
        return false;
    }
    return true;
}

/*
Send a string message to a connected client over a TCP socket. 
Ensures there is an active connection before attempting to send the data. 
If the operation fails, it logs an error message and returns false.
*/
bool TcpService::SendMessage(const std::string &message, int clientId){

    // Send the message to the client
    if (send(clientId, message.c_str(), message.length(), 0) < 0){
        std::cerr << "Failed to send message\n";
        return false;
    }
    return true;
}

// Destructor that ensures the TCP connection is closed when the TcpService object is destroyed.
TcpService::~TcpService(){
    CloseAllConnections();
}
