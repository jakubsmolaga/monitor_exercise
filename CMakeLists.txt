cmake_minimum_required(VERSION 3.0.0)
project(monitor_exercise VERSION 0.1.0)

add_executable(monitor_exercise src/main.c src/monitor.c src/fifo.c)
target_include_directories(monitor_exercise PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(monitor_exercise PRIVATE Threads::Threads)
