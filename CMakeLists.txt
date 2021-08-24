cmake_minimum_required(VERSION 3.10)
project(NewProtocol)

enable_testing()
# set the project name

include_directories(src/** )
include_directories(src )
include_directories(cmd )
file(GLOB_RECURSE SRC  
    src/**/*.cpp 
    src/**/*.h
)
file(GLOB_RECURSE TEST  
    src/**/*.cc
)

# add the executablels
add_executable(nic ${SRC} cmd/nic.cpp )
add_executable(send ${SRC} cmd/send.cpp )
add_executable(recv ${SRC} cmd/receive.cpp )
target_link_libraries(nic pthread)
#add gtest executablels
add_executable(unit_test ${SRC} ${TEST})
target_link_libraries(unit_test  gtest gtest_main pthread)
add_test(NAME unit_test COMMAND unit_test)