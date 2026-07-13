cmake_minimum_required(VERSION 3.19)
project(QtRace LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-float-loop-counter")
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(QtRace
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    gamescene.h gamescene.cpp
    caritem.h caritem.cpp
    gameengine.h gameengine.cpp
)

target_link_libraries(QtRace
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS QtRace
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QtRace
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Test)

qt_add_executable(test_gameengine
    gameengine.cpp
    gameengine.h
    caritem.cpp
    caritem.h
    test/test_gameengine.cpp
)

target_include_directories(test_gameengine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(test_gameengine
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Test
)

add_test(NAME GameEngineTest COMMAND test_gameengine)
