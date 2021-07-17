cmake_minimum_required(VERSION 3.16)

project(test-QmlFutures-2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Gui Quick REQUIRED)

add_executable(test-QmlFutures-2 main2.cpp main2.qrc)
set_property(TARGET test-QmlFutures-2 PROPERTY CXX_STANDARD 17)
set_property(TARGET test-QmlFutures-2 PROPERTY AUTOMOC ON)
set_property(TARGET test-QmlFutures-2 PROPERTY AUTORCC ON)
target_link_libraries(test-QmlFutures-2 PRIVATE Qt5::Gui Qt5::Quick QmlFutures)

target_compile_options(test-QmlFutures-2 PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
)

#add_test(NAME test-QmlFutures-2-runner COMMAND test-QmlFutures-2)
