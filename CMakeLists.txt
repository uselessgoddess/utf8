cmake_minimum_required(VERSION 3.20)
project(utf8 CXX)

set(CMAKE_CXX_STANDARD 23)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(${PROJECT_NAME}_lib INTERFACE)
target_include_directories(${PROJECT_NAME}_lib INTERFACE include)
set_target_properties(${PROJECT_NAME}_lib PROPERTIES
        CMAKE_CXX_STANDARD_REQUIRED ON
        CMAKE_CXX_EXTENSIONS OFF
        LINKER_LANGUAGE CXX
        CXX_STANDARD 23)

set(${PROJECT_NAME}_EXAMPLES
        basic char string_view parse)
foreach (EXAMPLE ${${PROJECT_NAME}_EXAMPLES})
    add_executable(${EXAMPLE} examples/${EXAMPLE}.cpp)
    target_link_libraries(${EXAMPLE} ${PROJECT_NAME}_lib)
endforeach ()

add_executable(${PROJECT_NAME}_tests
        tests/basic.cpp tests/access.cpp
        tests/iter.cpp tests/operations.cpp)
target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME}_lib)
target_link_libraries(${PROJECT_NAME}_tests ${CONAN_LIBS})