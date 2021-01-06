cmake_minimum_required(VERSION 3.16)
project(Stickman)

file(MAKE_DIRECTORY bin data)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++20 -O3 -march=native -flto -Wall -static-libstdc++ -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++20 -march=native -Wall -g")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(unofficial-box2d REQUIRED)

add_executable(Client 
    src/Client.cpp 
    src/render/Renderer.cpp
    src/net/NetManager.cpp)

add_executable(Server 
    src/Server.cpp
    src/net/NetManager.cpp
    src/game/Entity.cpp
    src/render/Renderer.cpp)

add_executable(Testman 
    src/Testman.cpp 
    src/physics/Box.cpp
    src/physics/Ground.cpp
    src/physics/ContactListener.cpp
    src/render/Renderer.cpp)

target_link_libraries(Client pthread ${Vulkan_LIBRARIES} glfw unofficial::box2d::box2d)
target_link_libraries(Server pthread ${Vulkan_LIBRARIES} glfw unofficial::box2d::box2d)
target_link_libraries(Testman pthread ${Vulkan_LIBRARIES} glfw unofficial::box2d::box2d)

add_custom_command(TARGET Client POST_BUILD COMMAND ${CMAKE_SOURCE_DIR}/src/script/compileshader.sh WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_command(TARGET Client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Client> ${CMAKE_SOURCE_DIR}/bin)
add_custom_command(TARGET Server POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Server> ${CMAKE_SOURCE_DIR}/bin)
add_custom_command(TARGET Testman POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Testman> ${CMAKE_SOURCE_DIR}/bin)