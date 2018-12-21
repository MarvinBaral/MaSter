cmake_minimum_required(VERSION 2.8)

project(MaSter)
add_executable(${PROJECT_NAME} "main.cpp" "MaSter.cpp" "Camera.cpp" "Database.cpp")
find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
