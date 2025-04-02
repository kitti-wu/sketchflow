cmake_minimum_required(VERSION 3.5)

project(project12 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加这一行来禁用 NEON
add_definitions(-DEIGEN_DONT_ALIGN)

set(OpenCV_DIR /opt/homebrew/Cellar/opencv/4.9.0_8/lib/cmake/opencv4)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
include_directories(core)
include_directories(gui)
include_directories(utils)
include_directories(icons)
include_directories(canvas)
include_directories("/opt/homebrew/Cellar/opencv/4.9.0_8/include/opencv4")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Xml Gui Svg OpenGL OpenGLWidgets Core5Compat)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        iconres.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(project12
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        core/arap.h core/arap.cpp
        core/quad.h core/quad.cpp
        core/point.h
        core/corner.h
        core/dialsandknobs.h core/dialsandknobs.cpp
        core/tools/tool.h
        gui/dialsandknobs_ui.h gui/dialsandknobs_ui.cpp
        core/strokeinterval.h core/strokeinterval.cpp
        core/stroke.h
        core/tools/tool.cpp
        utils/geom.h
        core/inbetweens.h core/inbetweens.cpp
        core/group.h core/group.cpp
        core/uvhash.h

        iconres.qrc
        canvas/shape.h canvas/shape.cpp
        canvas/line.h canvas/line.cpp
        canvas/rect.h canvas/rect.cpp
        canvas/paintwidget.h canvas/paintwidget.cpp
        canvas/tri.h canvas/tri.cpp
        canvas/drawcommand.h canvas/drawcommand.cpp
        canvas/rubber.h canvas/rubber.cpp
        core/siftmap.h core/siftmap.cpp
        core/rectredistration.h core/rectredistration.cpp
        core/mesh.h core/mesh.cpp
        core/laplaciandeform.h core/laplaciandeform.cpp
        canvas/dragmanager.h canvas/dragmanager.cpp
        canvas/paintarea.h canvas/paintarea.cpp
        canvas/switchbtn.h canvas/switchbtn.cpp
        core/commondeform.h core/commondeform.cpp


    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET project12 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(project12 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(project12
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(project12
  PUBLIC
  Qt6::Widgets Qt6::Xml Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Core5Compat
  OpenGL::GL
)
target_link_libraries(project12 PRIVATE ${OpenCV_LIBS})
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.project12)
endif()
set_target_properties(project12 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS project12
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(project12)
endif()
