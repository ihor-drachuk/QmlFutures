cmake_minimum_required(VERSION 3.16)
PROJECT(QmlFutures)

option(QML_FUTURES_ENABLE_TESTS     "QmlFutures: Enable tests" OFF)
option(QML_FUTURES_ALLOW_QML_TESTS  "QmlFutures: Allow QML tests" ON)
option(QML_FUTURES_ENABLE_BENCHMARK "QmlFutures: Enable benchmark" OFF)

set(QML_FUTURES_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Qt5 COMPONENTS Core Quick REQUIRED)

FILE(GLOB_RECURSE SOURCES src/*.cpp src/*.qrc src/*.h headers/*.h)

list(APPEND QML_IMPORT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/Qml/")
list(REMOVE_DUPLICATES QML_IMPORT_PATH)
set(QML_IMPORT_PATH "${QML_IMPORT_PATH}" CACHE STRING "Qt Creator extra qml import paths" FORCE)

add_library(QmlFutures STATIC ${SOURCES})
target_link_libraries(QmlFutures PUBLIC Qt5::Core PRIVATE Qt5::Quick)
set_property(TARGET QmlFutures PROPERTY CXX_STANDARD 17)
set_property(TARGET QmlFutures PROPERTY AUTOMOC ON)
set_property(TARGET QmlFutures PROPERTY AUTORCC ON)

if(MSVC)
    target_link_options(QmlFutures PRIVATE "/ignore:4221")
    set_target_properties(QmlFutures PROPERTIES STATIC_LIBRARY_OPTIONS "/ignore:4221")
endif()

target_include_directories(QmlFutures PUBLIC headers)

if (QML_FUTURES_CI_RUN)
    target_compile_definitions(QmlFutures PUBLIC QML_FUTURES_CI_RUN)
endif()

add_subdirectory(tests)
