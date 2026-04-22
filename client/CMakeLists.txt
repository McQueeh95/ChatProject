cmake_minimum_required(VERSION 3.16)

project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ENV{PKG_CONFIG_PATH} "/usr/local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
add_compile_definitions(QT_FORCE_STDERR_LOGGING)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core WebSockets)
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_path(SODIUM_INCLUDE_DIRS NAMES sodium.h HINTS /usr/include /usr/local/include)
find_library(SODIUM_LIBRARIES NAMES sodium HINTS /usr/lib64 /usr/lib /usr/local/lib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)

set(PROJECT_SOURCES
        main.cpp
        ui/mainwindow.cpp
        ui/mainwindow.h
        ui/mainwindow.ui

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ui/registrationpage.h ui/registrationpage.cpp ui/registrationpage.ui

        network/networkclient.h network/networkclient.cpp
        models/contactiemdelegate.h
        models/contactiemdelegate.h models/contactiemdelegate.cpp
        models/contactlistmodel.h models/contactlistmodel.cpp
        appcontroller.h appcontroller.cpp
        networktypes.h
        ui/loginpage.h ui/loginpage.cpp ui/loginpage.ui
        resources.qrc
        styles.qss
        ui/mainpage.h ui/mainpage.cpp ui/mainpage.ui
        models/chatdelegate.h models/chatdelegate.cpp
        models/messagedelegate.h models/messagedelegate.cpp
        models/messageviewmodel.h models/messageviewmodel.cpp
        models/approles.h
        models/searchviewmodel.h models/searchviewmodel.cpp
        cryptoservice.h cryptoservice.cpp
        securebuffer.h
        uitypes.h




    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET client APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(client SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(client
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::Core Qt6::WebSockets Qt6::Sql)
target_link_libraries(client PRIVATE Qt6::Core)
target_link_libraries(client PRIVATE Qt6::Core)
target_link_libraries(client PRIVATE Qt6::Core)
target_link_libraries(client PRIVATE Qt6::Core)
target_include_directories(client PRIVATE /usr/local/include ${SODIUM_INCLUDE_DIRS})
target_link_libraries(client PRIVATE ${SODIUM_LIBRARIES})
target_link_libraries(client PRIVATE Qt6::Core)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.client)
endif()
set_target_properties(client PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(client)
endif()
