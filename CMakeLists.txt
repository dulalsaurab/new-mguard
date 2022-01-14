cmake_minimum_required(VERSION 3.16)
project(mguard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# see https://cmake.org/cmake/help/latest/module/FindBoost.html
# this is only for windows. not wsl
#set(Boost_INCLUDE_DIR C:/Program\ Files/Cpp_Libs/boost/boost_1_76_0)
find_package(Boost)

include_directories(${Boost_INCLUDE_DIR})
# Note: a target should be already defined using 'add_executable' or 'add_library'
#target_link_libraries(TargetName ${Boost_LIBRARIES})
#add_executable(mguard mguard/main.cpp)
#add_executable(mguard mguard/scratch.cpp)
#add_executable(mguard mguard/generator.cpp mguard/generator.hpp)
add_executable(mguard mguard/testing.cpp)
