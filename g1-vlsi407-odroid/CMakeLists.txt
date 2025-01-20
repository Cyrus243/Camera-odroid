cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 11)

project(CameraApp CXX)
add_executable(CameraApp src/Camera.cpp src/main.cpp src/TcpService.cpp src/gpio/GpioService.cpp) 
target_include_directories(CameraApp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


find_package (OpenCV 3.1.0 EXACT REQUIRED PATHS /opt/poky/2.1.3/sysroots/aarch64-poky-linux/*)
if (OpenCV_FOUND)
    include_directories (${OpenCV_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${OpenCV_LIBRARIES})
else (OpenCV_FOUND)
    message (FATAL ERROR "Open CV Library Not found !")
endif (OpenCV_FOUND)