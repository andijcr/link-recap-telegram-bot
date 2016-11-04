cmake_minimum_required(VERSION 3.6)
project(link_recap_bot)

add_subdirectory(dep/tgbot-cpp)
include_directories(dep/tgbot-cpp/include)

#set( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} cpp-netlib )
#find_package ( cppnetlib 0.11.2 REQUIRED )
#include_directories ( ${CPPNETLIB_INCLUDE_DIRS} )


set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp)
add_executable(link_recap_bot ${SOURCE_FILES})
target_link_libraries(link_recap_bot TgBot ${CPPNETLIB_LIBRARIES})
