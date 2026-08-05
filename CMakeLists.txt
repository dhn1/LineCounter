cmake_minimum_required(VERSION 3.29)

project(lineCounter LANGUAGES CXX VERSION 0.0.2)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_SCAN_FOR_MODULES OFF)

file(STRINGS "/etc/os-release" RESULTS REGEX "^ID=(.*)")
set (DISTRO "${CMAKE_MATCH_1}")
message (STATUS "Linux distro: ${DISTRO}")

if ("${DISTRO}" STREQUAL "linuxmint")
    set(CMAKE_INSTALL_RPATH "/usr/local/qt6.11.0/lib")
    set (CPACK_GENERATOR "DEB")
else()
    set (CPACK_GENERATOR "RPM")
endif ()


if ("${DISTRO}" STREQUAL "fedora")
else()
    set (CPACK_GENERATOR "DEB")
endif ()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

configure_file(leak.supp ${CMAKE_BINARY_DIR}/bin/leak.supp COPYONLY)

add_executable(lineCounter
  main.cpp
)
target_link_libraries(lineCounter Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS lineCounter
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_property (SOURCE main.cpp PROPERTY COMPILE_DEFINITIONS VERSION="${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

#cpack stuff
include(InstallRequiredSystemLibraries)

set (CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set (CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} - code line counter (C/C++)")
set (CPACK_PACKAGE_VENDOR "David Netherwood")
set (CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set (CPACK_PACKAGE_CONTACT "david_netherwood@hotmail.com")

set (CPACK_DEBIAN_PACKAGE_DEPENDS "homebrewqtcore6.11.0")

set (CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set (CPACK_RPM_PACKAGE_GROUP "Development/Tools")
set (CPACK_RPM_PACKAGE_RELEASE "1")

include(CPack)

file (GLOB_RECURSE other "./Desktop/*")
set (other ${other} leak.supp)
add_custom_target (other SOURCES ${other})