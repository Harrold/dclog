cmake_minimum_required(VERSION 3.19)
project(dclog LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(dclog
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    containermodel.cpp
    containermodel.h
    dockerclient.cpp
    dockerclient.h
    logformatter.cpp
    logformatter.h
)

target_link_libraries(dclog
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS dclog
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET dclog
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
