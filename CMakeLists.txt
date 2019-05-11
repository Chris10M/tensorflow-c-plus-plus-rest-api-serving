# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.1)

project(cropper)

set(CMAKE_GXX_FLAGS "-Wall -Wconversion -fexception -std=c++11 -g")
find_package(OpenCV REQUIRED)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package(Boost 1.68.0 COMPONENTS filesystem) #log

include_directories(${Boost_INCLUDE_DIRS}) 
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("./spdlog")

message(${Boost_INCLUDE_DIRS})

# Set the output folder where your program will be created

# execute_process (
#     COMMAND bash -c ${CMAKE_SOURCE_DIR}/dependancies/install_dependancies.sh
# )

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}")

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${SOURCE_DIR}")

add_executable(app ${SOURCE_DIR}/app.cpp ${SOURCE_DIR}/tf_utils.cpp) 

target_link_libraries(app -lcurl -lcurlpp -pthread -lpistache -lboost_program_options -pthread)
target_link_libraries(app ${OpenCV_LIBS} ${Boost_LIBRARIES} tensorflow)