cmake_minimum_required(VERSION 3.16)

project(QRsync LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(QRsync
        main.cpp
        MainWindow.hpp
        MainWindow.cpp
        HelpViewer.hpp
        HelpViewer.cpp
)

target_link_libraries(QRsync PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

install(TARGETS QRsync
        RUNTIME DESTINATION bin
)