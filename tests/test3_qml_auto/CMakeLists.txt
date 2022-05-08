cmake_minimum_required(VERSION 3.16)

project(test-QmlFutures-3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Quick QuickTest REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Quick QuickTest)

add_executable(test-QmlFutures-3 main3.cpp main3.qrc)
set_property(TARGET test-QmlFutures-3 PROPERTY CXX_STANDARD 17)
set_property(TARGET test-QmlFutures-3 PROPERTY AUTOMOC ON)
set_property(TARGET test-QmlFutures-3 PROPERTY AUTORCC ON)
target_link_libraries(test-QmlFutures-3 PRIVATE Qt${QT_VERSION_MAJOR}::QuickTest Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Quick QmlFutures)
target_compile_definitions(test-QmlFutures-3 PRIVATE QUICK_TEST_SOURCE_DIR=":/QmlFutures/QmlTests/" QML_FUTURES_ROOT_DIR="${QML_FUTURES_ROOT_DIR}")

target_compile_options(test-QmlFutures-3 PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
)

if (QML_FUTURES_ALLOW_QML_TESTS)
    add_test(NAME test-QmlFutures-3-runner COMMAND test-QmlFutures-3)
endif()
