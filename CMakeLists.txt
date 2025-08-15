CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(fancy)

file(GLOB_RECURSE SRC_FILES src/*.c)
add_executable(${PROJECT_NAME} ${SRC_FILES})

find_package(PkgConfig REQUIRED)
find_package(loggy REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(GTK_LAYER_SHELL REQUIRED "gtk4-layer-shell-0")
pkg_check_modules(MSGPACK REQUIRED "msgpack-c")

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${GTK4_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTK_LAYER_SHELL_INCLUDE_DIRS}
    ${MSGPACK_LIBRARY_DIRS}
    loggy::loggy
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${GTK_LAYER_SHELL_LIBRARIES}
    ${GTK4_LIBRARIES}
    ${MSGPACK_LIBRARIES}
    loggy::loggy
)
