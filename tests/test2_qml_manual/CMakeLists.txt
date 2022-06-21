cmake_minimum_required(VERSION 3.16)

project(test-QmlFutures-2 LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Quick)

add_executable(test-QmlFutures-2 main2.cpp main2.qrc)
set_property(TARGET test-QmlFutures-2 PROPERTY AUTOMOC ON)
set_property(TARGET test-QmlFutures-2 PROPERTY AUTORCC ON)
target_link_libraries(test-QmlFutures-2 PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Quick QmlFutures)

target_compile_options(test-QmlFutures-2 PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
)

#add_test(NAME test-QmlFutures-2-runner COMMAND test-QmlFutures-2)
