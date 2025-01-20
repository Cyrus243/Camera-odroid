#include "TcpService.h"

namespace{
    // A list of resolution options displayed for the user to choose from.
    const std::vector<std::string> resolutionOptions = {
        "1. 176 x 144", "2. 160 x 120", "3. 800 x 600", "4. 1184 x 656"};

    /*
     * Tables of widths and heights corresponding to each resolution option.
     * These are used to set the resolution dimensions based on the user's choice.
     */
    const int widthTable[13] = {176, 160, 800, 1184};
    const int heightTable[13] = {144, 120, 600, 656};

    // Define server status
    constexpr auto READY = 'R';
    constexpr auto NO_LIGHT = 'N';
    constexpr auto BUTTON_PUSHED = 'B';
}

/*
Initializes the TcpService instance with a server address and port.
Sets _clientId to -1 to indicate no active connection.
*/
TcpService::TcpService(const std::string &server_address) : _server_address(server_address) {}

// Closes the TCP connection if it’s active, resetting _clientId to -1 to mark disconnection.
void TcpService::CloseAllConnections(){
    if (!_serverId.empty()){
        for (auto id: _serverId){
            close(id);
            auto index = std::find(_serverId.begin(), _serverId.end(), id);
            _serverId.erase(index);
        }
    }
}


void TcpService::closeLastConnection(){
    close(_serverId.back());
    _serverId.pop_back();
    _port.pop_back();
}

/*
Establishes a TCP connection to the server.
Returns true if the connection is successful; false if an error occurs.
*/
bool TcpService::ConnectToServer(int port){
    // Create a TCP socket
    auto serverId = socket(AF_INET, SOCK_STREAM, 0);
    if (serverId == -1){
        std::cerr << "Failed to create socket\n";
        return false;
    }

    // Set up the server address structure
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Convert IP address from text to binary
    if (inet_pton(AF_INET, _server_address.c_str(), &server_addr.sin_addr) <= 0){
        std::cerr << "Invalid address/Address not supported\n";
        return false;
    }

    // Connect to the server
    if (connect(serverId, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        std::cerr << "Connection failed\n";
        return false;
    }

    std::cout << "Connected to server at " << _server_address << ":" << port << "\n";
    _port.push_back(port);
    _serverId.push_back(serverId);
    return true;
}

/*
Sends a string message to the server.
Returns true if the message is sent successfully; false otherwise.
*/
bool TcpService::SendMessage(const std::string &message, int serverId){
    if (serverId == -1){
        std::cerr << "No connection to server\n";
        return false;
    }

    // Send the message to the server
    if (send(serverId, message.c_str(), message.length(), 0) < 0){
        std::cerr << "Failed to send message\n";
        return false;
    }
    return true;
}

/*
Continuously requests and receives images from the server.
The loop displays images until the "Esc" key is pressed or an error occurs.
Returns true if images are displayed successfully; false if errors are encountered.
*/
bool TcpService::ObserveServerResponse(){
    auto imagecounter = 0;

    while (true){
        auto state = WaitServerResponse(_serverId[0]);
        SendMessage("z", _serverId[0]);

        if (state.back() == READY){
            SendMessage("c", _serverId[0]);
            auto buttonState = WaitServerResponse(_serverId[0]);
            SendMessage("z", _serverId[0]);

            // Receive image size
            auto img_size = 0;
            if (recv(_serverId[0], &img_size, sizeof(int), 0) == -1){
                std::cerr << "Failed to receive image size\n";
                break;
                return false;
            }

            // Receive image data in chunks until complete
            std::vector<uchar> buffer(img_size);
            auto bytes_received = 0;
            while (bytes_received < img_size){
                int bytes = recv(_serverId[0], buffer.data() + bytes_received, img_size - bytes_received, 0);
                if (bytes == -1){
                    std::cerr << "Failed to receive image data\n";
                    break;
                }
                bytes_received += bytes;
            }
            // Decode the image data
            auto img = cv::imdecode(buffer, cv::IMREAD_COLOR);

            if (img.empty()){
                std::cerr << "Failed to decode image\n";
                break;
                return false;
            }

            if (buttonState.back() == BUTTON_PUSHED){
                auto pid = fork();
                auto imageName = "image-" + std::to_string(imagecounter) + ".png";
                if (pid == 0){
                    cv::imwrite(imageName, img);

                    std::vector<DecodedObject> decodedObjects;
                    _qrManager.DecodeQrCode(img, decodedObjects);
                    _qrManager.DisplayQrCode(img, decodedObjects, imagecounter);
                    
                    auto message = _qrManager.getMessage(decodedObjects);
                    ConnectToServer(4098);
                    SendMessage(message, _serverId[1]);
                    closeLastConnection();
                    exit(0);
                }
                imagecounter++;
                
            }

            // Display the image
            cv::imshow("Received Image", img);
            switch (cv::waitKey(30)){
            case 27:
                SendMessage("q", _serverId[0]);
                cv::destroyAllWindows();
                return true;
            case 114:{
                    SendMessage("r", _serverId[0]);
                    cv::destroyAllWindows();
                    std::string resolution = ChooseResolution();
                    SendMessage(resolution, _serverId[0]);
                }
                break;
            default:
                SendMessage("z", _serverId[0]);
            }
            
        }else{
            cv::destroyAllWindows();
        }
    }
    return true;
}

/*
Waits for a response from the server and returns it as a string.
Blocks until data is received, the connection is closed, or an error occurs.
Returns an empty string if the connection is closed or an error occurs.
*/
std::string TcpService::WaitServerResponse(int serverId){
    char buffer[1024];
    std::string result = "";

    while (true){
        // Receive data from the server
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(serverId, buffer, sizeof(buffer), 0);

        // Exit if the connection is closed or an error occurs
        if (bytes_received <= 0){
            std::cerr << "Connection closed or error occurred\n";
            break;
        }

        result = std::string(buffer, bytes_received);
        // Exit if data is not received
        if (buffer != ""){
            break;
        }
    }
    return result;
}

/*
Allows the user to select a resolution from a predefined list.
Displays the available options, ensures the input is valid, and returns the chosen 
resolution as a string in the format "WidthxHeight".
*/
std::string TcpService::ChooseResolution(){
    auto choice = 0;

    while (true){
        std::cout << "Select a resolution option:" << std::endl;
        for (const auto &option : resolutionOptions){
            std::cout << option << std::endl;
        }
        std::cout << "Enter your option(1-4): ";
        std::cin >> choice;

        if (std::cin.fail() || choice < 1 || choice > 4){
            std::cin.clear();
            std::cin.ignore(1000000000, '\n');

            std::cout << "Invalid choice, please enter a number between 1 and 4." << std::endl;
            continue;
        }
        break;
    }

    return GetVideoResolution(choice-1);
}

/*
Retrieves the resolution dimensions (width and height) corresponding to the given index.
Uses predefined tables of width and height values to return the resolution.
Returns the resolution as a string in the format "WidthxHeight".
*/
std::string TcpService::GetVideoResolution(int index){
    auto width = widthTable[index];
    auto height = heightTable[index];

    return std::to_string(width) + "x" + std::to_string(height);
}

// Destructor that ensures the TCP connection is closed when the TcpService object is destroyed.
TcpService::~TcpService(){
    CloseAllConnections();
}
