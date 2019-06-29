cmake_minimum_required(VERSION 3.1)

project(showface LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick Network REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(faceloader "loader.cpp" "result.cpp")
target_link_libraries(faceloader PRIVATE Qt5::Core Qt5::Quick Qt5::Network OpenSSL::SSL)

add_executable(${PROJECT_NAME} "main.cpp" "qml.qrc")
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Quick faceloader)

add_executable(showface-cli "cli.cpp")
target_link_libraries(showface-cli PRIVATE Qt5::Core Qt5::Quick Qt5::Network faceloader)
