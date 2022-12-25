cmake_minimum_required(VERSION 3.6)
project(biziboxMonitor)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("C:/lib/curl/curl-7.54.0/include")
set(SOURCE_FILES main.cpp LimitSingleInstance.h jsoncpp.cpp json.h json-forwards.h rapidxml.hpp curlSender.cpp printProcessNameAndID.cpp deleteSelf.cpp properties.rc)

add_executable(biziboxMonitor ${SOURCE_FILES})
target_link_libraries(biziboxMonitor libcurl.a libcurldll.a Psapi.lib)


