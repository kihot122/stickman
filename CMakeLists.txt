cmake_minimum_required(VERSION 3.16)
project(Stickman)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++20 -O3 -march=native -flto -Wall -static-libstdc++ -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-std=c18 -O3 -march=native -flto -Wall -static-libstdc++ -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++20 -march=native -Wall -g")
set(CMAKE_C_FLAGS_DEBUG "-std=c18 -march=native -Wall -g")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(Stickman main.cpp)
target_link_libraries(Stickman ${Vulkan_LIBRARIES})
target_link_libraries(Stickman glfw)