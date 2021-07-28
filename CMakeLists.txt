cmake_minimum_required(VERSION 3.10)

# set the project name
project(NewProtocol)
file(GLOB_RECURSE SRC  
    src/**/*.cpp 
    src/*.cpp
    src/**/*.h
    src/*.h
)
# add the executable
add_executable(main ${SRC} main.cpp)