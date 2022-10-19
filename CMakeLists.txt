cmake_minimum_required(VERSION 3.13)

project(zmqdemo)

# set(CMAKE_PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})




include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_executable(server server.cpp)
target_link_libraries(server PRIVATE
    zmq
    )

add_executable(client client.cpp)
target_link_libraries(client PRIVATE
    zmq
    )
