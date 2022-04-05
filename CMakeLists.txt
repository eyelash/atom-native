cmake_minimum_required(VERSION 3.13)
project(atom-native)

add_subdirectory(superstring)
add_subdirectory(text-buffer)
add_subdirectory(oniguruma)
add_subdirectory(git-utils)
add_subdirectory(atom)

add_executable(main main.cc)
target_link_libraries(main atom)
