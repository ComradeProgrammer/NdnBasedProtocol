cmake_minimum_required(VERSION 3.10)
project(NewProtocol)

enable_testing()
# set the project name

include_directories(./src )


file(GLOB_RECURSE SRC  
    src/**/*.cpp 
    src/**/*.h
)
file(GLOB_RECURSE TEST  
    src/**/*.cc
)

# add the executablels
add_executable(main ${SRC} src/main.cpp)

#add gtest executablels
add_executable(unit_test ${SRC} ${TEST})
target_link_libraries(unit_test  gtest gtest_main pthread)
add_test(NAME unit_test COMMAND unit_test)