#pragma once
#include <zbar.h>
#include <opencv2/opencv.hpp>
#include <vector>

struct DecodedObject{
    /// @brief Type of the decoded object.
    std::string type;

    /// @brief Data contained in the decoded object.
    std::string data;

    /// @brief Location of the object's points in the image.
    std::vector<cv::Point> location;
};

/// @class QrManager
/// @brief A class for managing QR Code detection, decoding, and visualization.
class QrManager{
public:
    /// @brief Default constructor for the QrManager class.
    QrManager();

    /// @brief Decodes QR Codes from the provided image.
    /// @param image Reference to the image (cv::Mat) in which QR Codes will be detected.
    /// @param decodedObjects Reference to a vector that will store the detected QR Code objects.
    void DecodeQrCode(cv::Mat &image, std::vector<DecodedObject> &decodedObjects);

    /// @brief Displays the locations of decoded QR Codes on the input image.
    /// @param image Reference to the image (cv::Mat) where QR Codes will be displayed.
    /// @param decodedObjects Reference to a vector containing the decoded QR Code objects.
    /// @param counter An integer used to create unique filenames for the annotated images.
    void DisplayQrCode(cv::Mat &image, std::vector<DecodedObject> &decodedObjects, int counter);

    /// @brief Retrieves the message from the decoded QR Codes.
    /// @param decodedObjects Reference to a vector containing the decoded QR Code objects.
    /// @return A string containing the data of the last decoded QR Code, sanitized if necessary.
    std::string getMessage(std::vector<DecodedObject> &decodedObjects);
    

private:
    /// @brief Cleans and standardizes a sentence by removing unwanted characters.
    /// @param sentence Reference to the string that needs to be sanitized.
    /// @return A sanitized string containing only lowercase alphanumeric characters and spaces.
    std::string sanitizeSentence(std::string& sentence);

    /// @brief ZBar scanner used for detecting and decoding QR Codes.
    zbar::ImageScanner _scanner;
};