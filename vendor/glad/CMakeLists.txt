cmake_minimum_required(VERSION 3.13)
project(glad)


set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GLAD_INCLUDES ${GLAD_DIR}/include/)

add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC include)
target_include_directories(glad PRIVATE src)

add_library(Glad::Glad ALIAS glad)
