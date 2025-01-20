#include "QrManager.h"

// Constructor: Initializes the QR code scanner for detecting QR codes.
QrManager::QrManager(){
    _scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
}

/*
Displays QR code data and annotations on the provided image.
Draws bounding boxes around detected QR codes.
Logs QR code messages to the console.
Saves the annotated image with a unique name if QR codes are found.
*/
void QrManager::DisplayQrCode(cv::Mat &image, std::vector<DecodedObject> &decodedObjects, int counter) {
    auto message = "";
    
    // Check if the input image is valid.
    if (image.empty()) {
        std::cerr << "Erreur : L'image fournie est vide !" << std::endl;
        return;
    }

    // Iterate through the decoded QR codes to display their messages and draw bounding boxes.
    for (const auto& obj : decodedObjects) {
        if (obj.location.empty()) {
            continue;
        }
        std::cout << "Message contenu dans le Qr code: " << obj.data << std::endl;
        // Draw a rectangle around the detected QR code.
        cv::Rect boundingBox = cv::boundingRect(obj.location);
        cv::rectangle(image, boundingBox, cv::Scalar(0, 0, 255), 3); 
    }

    // Save the annotated image if any QR codes were detected.
    if (!decodedObjects.empty()){
        auto imageName = "Qr-image-" + std::to_string(counter) + ".png";
        cv::imwrite(imageName, image);
    }
}

/*
Decodes QR codes from the provided image.
Converts the image to grayscale.
Uses the zbar library to detect QR codes.
Logs an error if no QR codes are found.
Adds QR code data and locations to decodedObjects.
*/
void QrManager::DecodeQrCode(cv::Mat &image, std::vector<DecodedObject> &decodedObjects)
{
    // Convert the input image to grayscale for better QR code processing.
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    // Create a zbar image from the grayscale data for scanning.
    zbar::Image zbarImage(image.cols, image.rows, "Y800", (uchar *)grayImage.data, image.cols * image.rows);

    // Scan the image for QRCodes
    int n = _scanner.scan(zbarImage);
    if (n == 0){
        std::cerr << "Aucun QR code détecté dans l'image." << std::endl;
        return;
    }

    // Process each detected QR code and extract its data and location.
    for (zbar::Image::SymbolIterator symbol = zbarImage.symbol_begin(); symbol != zbarImage.symbol_end(); ++symbol){
        DecodedObject obj;

        obj.type = symbol->get_type_name();
        obj.data = symbol->get_data();

        // Extract the location of the QR code and add it to the decoded object.
        for (int i = 0; i < symbol->get_location_size(); i++)
        {
            obj.location.push_back(cv::Point(symbol->get_location_x(i), symbol->get_location_y(i)));
        }
        decodedObjects.push_back(obj);
    }
}

/*
Extracts the message from the first decoded QR code in the vector.
Returns the sanitized version of the extracted message.
*/
std::string QrManager::getMessage(std::vector<DecodedObject> &decodedObjects){
    std::string result = "";
    
    // Convert the input string to lowercase and remove non-alphanumeric characters.
    for (const auto& obj : decodedObjects) {
        if (obj.location.empty()) {
            continue;
        }
        result = obj.data;
       
    }

    return sanitizeSentence(result);
}

// Sanitizes a given string by converting to lowercase and removing non-alphanumeric characters.
std::string QrManager::sanitizeSentence(std::string& sentence){
    std::string result;

    for (char c : sentence){
        if (std::isalnum(c) || c == ' '){
            result += std::tolower(c);
        }
    }

    return result;
}