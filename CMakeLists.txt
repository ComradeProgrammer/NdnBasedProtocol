cmake_minimum_required(VERSION 3.10)
project(NewProtocol)

enable_testing()
# set the project name

include_directories(src )
file(GLOB_RECURSE SRC  
    src/**/*.cpp 
    src/**/*.h
)
file(GLOB_RECURSE TEST  
    src/**/*.cc
)


# these e2e test objects are removed to decrease the time consumption of compilation. If you want to run e2e test, remove the #

add_executable(routing ${SRC} src/cmd/routing/main.cpp)
target_link_libraries(routing pthread)


# add_executable(nicObserverTest ${SRC} cmd/nicObserverTest/main.cpp)
# target_link_libraries(nicObserverTest pthread)

#add gtest executablels
add_executable(unittest ${SRC} ${TEST})
target_link_libraries(unittest  gtest gtest_main pthread)

add_test(NAME unit_test COMMAND unit_test)