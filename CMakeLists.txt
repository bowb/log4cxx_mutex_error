cmake_minimum_required(VERSION 3.8)
project(test_package CXX)

find_package(log4cxx REQUIRED)
find_package(Threads REQUIRED)
file(COPY "logging.xml" DESTINATION ${CMAKE_BINARY_DIR})

add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME}  Threads::Threads log4cxx)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)