cmake_minimum_required(VERSION 3.16)
project(LaunchPadManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/LeftPanel.cpp
    src/RightPanel.cpp
    src/models/DesktopFileModel.cpp
    src/delegates/DesktopFileDelegate.cpp
    src/widgets/InfoPage.cpp
    src/widgets/EditPage.cpp
    src/widgets/AddPage.cpp
    src/core/DesktopFile.cpp
    src/core/DesktopFileManager.cpp
    src/utils/IconLoader.cpp
)

set(HEADERS
    src/MainWindow.h
    src/LeftPanel.h
    src/RightPanel.h
    src/models/DesktopFileModel.h
    src/delegates/DesktopFileDelegate.h
    src/widgets/InfoPage.h
    src/widgets/EditPage.h
    src/widgets/AddPage.h
    src/core/DesktopFile.h
    src/core/DesktopFileManager.h
    src/utils/IconLoader.h
)

qt_add_resources(RESOURCES resources/resources.qrc)

qt_add_executable(LaunchPadManager
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_link_libraries(LaunchPadManager PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui)

target_include_directories(LaunchPadManager PRIVATE src)