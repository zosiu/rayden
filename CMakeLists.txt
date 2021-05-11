cmake_minimum_required(VERSION 3.16)

project(Rayden)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/")

file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HDR_FILES include/rayden/*.hpp)

set(CMAKE_BINARY_DIR "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/vendor/")
set(SHADER_DIR "${PROJECT_SOURCE_DIR}/shaders/")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_subdirectory(${GLAD_DIR})

# glm
set(GLM_DIR "${LIB_DIR}/glm")
add_subdirectory(${GLM_DIR})

# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES
    OFF
    CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS
    OFF
    CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS
    OFF
    CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL
    OFF
    CACHE INTERNAL "Generate installation target")
add_subdirectory(${GLFW_DIR})

# spdlog
set(SPDLOG_DIR "${LIB_DIR}/spdlog")
set(SPDLOG_BUILD_SHARED
    OFF
    CACHE INTERNAL "Build spdlog shared")
set(SPDLOG_BUILD_EXAMPLE
    OFF
    CACHE INTERNAL "Build spdlog examples")
set(SPDLOG_BUILD_TESTS
    OFF
    CACHE INTERNAL "Build spdlog tests")
set(SPDLOG_BUILD_BENCH
    OFF
    CACHE INTERNAL "Build spdlog benchmarks")
set(SPDLOG_NO_EXCEPTIONS
    ON
    CACHE INTERNAL "Build spdlog no exc")
add_subdirectory(${SPDLOG_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_NAME "rayden")
add_executable(${APP_NAME} ${SRC_FILES} ${HDR_FILES})
target_include_directories(${APP_NAME}
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(${APP_NAME} PRIVATE cxx_std_17)

if(CMAKE_COMPILER_IS_GNUCC)
  target_compile_options(${APP_NAME} PRIVATE -Werror -Wall -Wextra -Wpedantic)
endif(CMAKE_COMPILER_IS_GNUCC)

target_compile_definitions(${APP_NAME} PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")
# target_precompile_headers (${APP_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/pch.hpp")

target_compile_definitions(${APP_NAME} PRIVATE "GLFW_INCLUDE_NONE")
target_include_directories(${APP_NAME} PRIVATE "${GLFW_DIR}/include")
target_include_directories(${APP_NAME} PRIVATE "${GLM_DIR}/include")
target_include_directories(${APP_NAME} PRIVATE "${GLAD_DIR}/include")
target_include_directories(${APP_NAME} PRIVATE "${SPDLOG_DIR}/include")

target_link_libraries(${APP_NAME} "glfw" "${GLFW_LIBRARIES}")
target_link_libraries(${APP_NAME} "glad" ${CMAKE_DL_LIBS})
target_link_libraries(${APP_NAME} "glm")
target_link_libraries(${APP_NAME} "spdlog")

# add_custom_command(
#   TARGET ${APP_NAME}
#   POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_DIR}
#           $<TARGET_FILE_DIR:${APP_NAME}>/shaders
#   COMMENT "Copying shaders"
#   VERBATIM)

add_custom_target(symlink_shaders)

FILE(TO_NATIVE_PATH ${SHADER_DIR} source)
FILE(TO_NATIVE_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/shaders destination)

IF (WIN32)
    add_custom_command(
        TARGET symlink_shaders POST_BUILD
        COMMAND mklink /D ${destination} ${source}
        DEPENDS ${destination}
        COMMENT "symbolic link shaders folder from ${source} => ${destination}"
        VERBATIM)
ELSE()
    add_custom_command(
        TARGET symlink_shaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
        DEPENDS ${destination}
        COMMENT "symbolic link shaders folder from ${source} => ${destination}"
        VERBATIM)
ENDIF()

add_dependencies(${APP_NAME} symlink_shaders)
