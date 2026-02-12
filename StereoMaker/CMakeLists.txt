cmake_minimum_required(VERSION 3.19)
project(StereoMaker LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Network)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(StereoMaker
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    camerascanner.h camerascanner.cpp
    lumixcameracontroller.h lumixcameracontroller.cpp
    imagedecoder.h imagedecoder.cpp
    imagecomposer.h imagecomposer.cpp
    qtlogging.ini
)

qt_add_translations(
    TARGETS StereoMaker
    TS_FILES StereoMaker_en_US.ts
)

target_link_libraries(StereoMaker
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)
target_link_libraries(StereoMaker PRIVATE Qt6::Core)
target_link_libraries(StereoMaker PRIVATE Qt6::Core)
target_link_libraries(StereoMaker PRIVATE Qt6::Core)
target_link_libraries(StereoMaker PRIVATE Qt6::Core)

include(GNUInstallDirs)

install(TARGETS StereoMaker
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET StereoMaker
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
