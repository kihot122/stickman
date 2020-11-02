cmake_minimum_required(VERSION 3.16)
project(test)

set(CMAKE_CXX_STANDARD 17)
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)


add_executable(testapp main.cpp)
target_link_libraries(testapp ${Vulkan_LIBRARIES})
target_link_libraries(testapp glfw)