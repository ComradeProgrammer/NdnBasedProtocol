cmake_minimum_required(VERSION 3.10)
project(NewProtocol)

enable_testing()
# set the project name

#include_directories(src/** )
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
add_executable(nic ${SRC} cmd/commonTest/nic.cpp )
# these e2e test objects are removed to decrease the time consumption of compilation. If you want to run e2e test, remove the #

# add_executable(ethernetSimpleTestSend ${SRC} cmd/ethernetSimpleTest/send.cpp )
# add_executable(ethernetSimpleTestRecv ${SRC} cmd/ethernetSimpleTest/recv.cpp )

# add_executable(ndnTransmitterTestSend ${SRC} cmd/ndnTransmitterTest/send.cpp )
# add_executable(ndnTransmitterTestRecv ${SRC} cmd/ndnTransmitterTest/recv.cpp )
# target_link_libraries(ndnTransmitterTestSend pthread)
# target_link_libraries(ndnTransmitterTestRecv pthread)

# add_executable(timerTest ${SRC} cmd/timerTest/timerTest.cpp)
# target_link_libraries(timerTest pthread)

# add_executable(ndnInterestTest ${SRC} cmd/ndnInterestTest/main.cpp)
# target_link_libraries(ndnInterestTest pthread)


add_executable(ndnTest ${SRC} cmd/ndnTest/main.cpp)
target_link_libraries(ndnTest pthread)

target_link_libraries(nic pthread)
#add gtest executablels
add_executable(unit_test ${SRC} ${TEST})
target_link_libraries(unit_test  gtest gtest_main pthread)
add_test(NAME unit_test COMMAND unit_test)