cmake_minimum_required(VERSION 3.16)
project(CAM3 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets)

# Enable automatic MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/OpenGLWidget.cpp
    src/RibbonWidget.cpp
    src/ThemeManager.cpp
)

set(HEADERS
    src/MainWindow.h
    src/OpenGLWidget.h
    src/RibbonWidget.h
    src/ThemeManager.h
)

# Resource files
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(CAM3 ${SOURCES} ${HEADERS} ${RESOURCES})

# Link Qt libraries
target_link_libraries(CAM3
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
)

# Set target properties
set_target_properties(CAM3 PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)