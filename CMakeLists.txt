cmake_minimum_required(VERSION 3.10)
project(NewProtocol)

enable_testing()
# set the project name
SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_BUILD_TYPE "Release")


include_directories(src )
file(GLOB_RECURSE SRC  
    src/**/*.cpp 
    src/**/*.h
    src/**/*.hpp
    src/*.cpp 
    src/*.h
    src/*.hpp
)
file(GLOB_RECURSE UNITTEST  
    src/**/*.cc
)
#build binary
add_executable(routing ${SRC} cmd/routing/main.cpp)
target_link_libraries(routing pthread crypto)

add_executable(ndnaddr ${SRC} cmd/ndnaddr/main.cpp)
target_link_libraries(ndnaddr pthread crypto)

# nic is used to show nic information
# add_executable(nic ${SRC} cmd/nic/main.cpp)
# target_link_libraries(nic pthread)



#add gtest executablels
add_executable(unittest ${SRC} ${UNITTEST})
target_link_libraries(unittest  gtest gtest_main pthread crypto)

add_test(NAME unittest COMMAND unittest)


# these e2e test objects are removed to decrease the time consumption of compilation. If you want to run e2e test, remove the #

# add_executable(testNicManager ${SRC} test/testNicManager/main.cpp)
# target_link_libraries(testNicManager pthread)

# add_executable(testTimer ${SRC} test/testTimer/main.cpp)
# target_link_libraries(testTimer pthread)

# add_executable(testNdn ${SRC} test/testNdn/main.cpp)
# target_link_libraries(testNdn pthread)



