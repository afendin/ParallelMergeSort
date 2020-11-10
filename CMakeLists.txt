cmake_minimum_required(VERSION 3.5)

project(ParallelMergeSort LANGUAGES C)

add_executable(ParallelMergeSort main.c)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(ParallelMergeSort PRIVATE Threads::Threads)
