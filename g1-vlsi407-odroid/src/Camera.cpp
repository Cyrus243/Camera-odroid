#include "Camera.h"
// Initialization of the video capture duration
namespace{
    const int VIDEO_DURATION = 5;
}

/*
Constructs a Camera object with the specified resolution, initializing
VideoCapture on the default camera (ID 0) and setting its width and height.
If the camera fails to open, throws a runtime error to indicate an issue
accessing the webcam.
*/
Camera::Camera(): _capture(0){
    _capture.set(CV_CAP_PROP_FRAME_WIDTH, 176);
    _capture.set(CV_CAP_PROP_FRAME_HEIGHT, 144);

    if (!_capture.isOpened()){
        throw std::runtime_error("Could not open camera.");
    }
}

/*
Updates the camera's resolution settings based on a string input in the 
format widthxheight (e.g., "1920x1080"). If the input format is invalid, 
the method logs an error message and does not update the resolution.
*/
void Camera::UpdateResolution(std::string& resolution){
     size_t pos = resolution.find('x');

        if (pos != std::string::npos){
            auto width = std::stoi(resolution.substr(0, pos));
            auto height = std::stoi(resolution.substr(pos + 1));

            _capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
            _capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
        } else{
            std::cerr << "Format de message incorrect\n";
        }
    
}

/*
Calculates the frames per second (FPS) the camera can capture at the
current resolution by measuring the time to capture 20 frames.
Starts a timer, captures 20 frames, and calculates the time taken.
If a frame capture fails, returns -1.0 and logs an error.
Returns the FPS as frames divided by elapsed time. 
*/
double Camera::CalculateFps(){
    cv::Mat frame;
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int frames = 20;

    for (int i = 0; i < frames; i++){
        _capture.read(frame);

        if (frame.empty()){
            std::cout << "Failed to capture an image" << std::endl;
            return -1.0;
        }
    }
    
    clock_gettime(CLOCK_REALTIME, &end);
    double difference = (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
    std::cout << "It took " << difference << " seconds to process " << frames << " frames" << std::endl;

    return frames/difference;
}

/*
Records a 5 second video at the calculated FPS and specified resolution,
saving it as "capture-liv1.avi" using the MJPG codec.
Verifies FPS is valid; calculates the number of frames to capture, and
initializes a VideoWriter with the set frame size and FPS.
Captures each frame and writes to the video file. If capture fails,
logs an error and returns false. Returns true on successful recording.
*/
bool Camera::RecordVideo(){
    double fps = Camera::CalculateFps();
    int codec = CV_FOURCC('M', 'J', 'P', 'G');
    int frameToCapture = static_cast<int>(fps * VIDEO_DURATION);

    if (fps <= 0){
        std::cout << "Failed to calculate FPS or invalid FPS." << std::endl;
        return false;
    }

    cv::Size frameSize(
        static_cast<int>(_capture.get(CV_CAP_PROP_FRAME_WIDTH)),
        static_cast<int>(_capture.get(CV_CAP_PROP_FRAME_HEIGHT))
    );

    cv::VideoWriter videoWriter("capture-liv1.avi", codec, fps, frameSize);
    
    cv::Mat frame;
    for (int i = 0; i < frameToCapture; i++){
        _capture.read(frame);

        if(frame.empty()){
            std::cout << "Failed to capture a frame." << std::endl;
            return false;
        }

        videoWriter.write(frame);
    }

    std::cout <<"The video was succefully recorded as capture-liv1.avi" << std::endl; 
    return true;
}

/*
Captures a frame from the camera. If successful, returns the image as a `cv::Mat`.
If the capture fails, prints an error and returns an empty `cv::Mat`.
*/
cv::Mat Camera::CaptureImage(){
    cv::Mat image;
    _capture >> image;
    
    if (image.empty()){
        std::cout << "Failed to capture a frame." << std::endl;
    }
    return image;
}

/*
Destructor for the Camera class that releases the VideoCapture 
object if it is currently open, freeing camera resources.
*/
Camera::~Camera(){
    if(_capture.isOpened()){
        _capture.release();
    }
}