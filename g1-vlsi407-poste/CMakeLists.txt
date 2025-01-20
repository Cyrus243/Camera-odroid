cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE MATCHES Release)
	message("Release Build")
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/release)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/release)
else(CMAKE_BUILD_TYPE MATCHES Release)
	message("Debug Build")
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug)
endif(CMAKE_BUILD_TYPE MATCHES Release)

project(CameraApp CXX C)
add_executable(CameraApp src/main.cpp src/TcpService.cpp src/QrManager.cpp)

if(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif(MSVC)

target_include_directories(CameraApp PUBLIC $(CMAKE_CURRENT_SOURCE_DIR)/include)

find_package(OpenCV 3.4.6 EXACT REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(CameraApp ${OpenCV_LIBRARIES} ${ZBAR_LIBRARIES} zbar)


