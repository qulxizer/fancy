CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(fancy)

add_executable(${PROJECT_NAME} src/main.c src/ipc.c src/listener.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(GTK_LAYER_SHELL REQUIRED "gtk4-layer-shell-0")

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${GTK4_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTK_LAYER_SHELL_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${GTK_LAYER_SHELL_LIBRARIES}
    ${GTK4_LIBRARIES}
)


message("GTK_LAYER_SHELL_LIBRARIES = ${GTK_LAYER_SHELL_LIBRARIES}")
message("GTK_LAYER_SHELL_INCLUDE_DIRS = ${GTK_LAYER_SHELL_INCLUDE_DIRS}")
