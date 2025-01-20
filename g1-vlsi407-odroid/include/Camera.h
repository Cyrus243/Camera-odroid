#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <time.h>

/// @brief Structure that represent the camera resolution.
struct videoResolution{
  int width;
  int height;
};

/// @class Camera
/// @brief Manage the creation of images and video from a webcam.
class Camera{
  public:
    /// @brief The constructor of the Camera class, which instantiates a VideoCapture object with the specified resolution. 
    /// @param resolution The resolution provided by the user.
    /// @throw std::runtime_error If the camera could not be opened successfully.
    Camera();

    /// @brief Calculate the frames per second the camera can capture for a given resolution.
    /// @return The calculated frames per second or -1.0 if an image capture fails.
    double CalculateFps();

    /// @brief Updates the camera resolution based on the given string in "widthxheight" format.
    /// @param resolution Reference to a string containing the resolution in "widthxheight" format.
    void UpdateResolution(std::string& resolution);

    /// @brief Capture 5 seconds of video and save at the resolution provided.
    /// @return A Boolean indicating whether the operation was successful or not.
    bool RecordVideo();

    /// @brief Captures a single image from the camera.
    /// @return A `cv::Mat` object containing the captured image, or an empty matrix if the capture fails.
    cv::Mat CaptureImage();

    /// @brief Release the created VideoCapture object.
    ~Camera();

  private:
    /// @brief An OpenCV VideoCapture object that captures the image from the webcam. 
    cv::VideoCapture _capture;
};
    