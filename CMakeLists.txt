cmake_minimum_required(VERSION 3.1)

#set (CMAKE_CXX_STANDARD 14)

project(bip47)

option(BUILD_VERBOSE       "Verbose build output." ON)

option(BUILD_TESTS         "Build the unit tests." ON)

SET(CMAKE_C_COMPILER g++-8)
SET(CMAKE_CXX_COMPILER g++-8)

set(CMAKE_CXX_FLAGS "-fpermissive")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(THREADS_PREFER_PTHREAD_FLAG ON)

if(BUILD_VERBOSE)
  set(CMAKE_VERBOSE_MAKEFILE true)
endif()

#-----------------------------------------------------------------------------
# Check which compiler to use

INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-std=c++17 HAVE_STD17)

if(HAVE_STD17)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
  set(GCC_WARN_FLAGS "-Wnoexcept -Wstrict-null-sentinel -Wlogical-op")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

  if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -frtti")
  endif()
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  set(CMAKE_COMPILER_IS_CLANGXX 1)

  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -stdlib=libc++")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  endif()
endif()

if(WIN32)
  add_definitions(-D_UNICODE)
else()
  if(OT_STRICT)
    set(PEDANTIC_CXX_FLAGS "-Wall -Werror -Wextra -pedantic -Wno-missing-braces -Wcast-align -Wctor-dtor-privacy -Wdisabled-optimization -Winit-self -Wmissing-include-dirs -Wswitch-default -Wold-style-cast -Woverloaded-virtual -Wmissing-declarations -Wunused-macros -Wundef -Wformat=2 -Wno-format-nonliteral ${GCC_WARN_FLAGS}")
  else()
    set(PEDANTIC_CXX_FLAGS "")
  endif()
endif()

if(ANDROID)
  set(PEDANTIC_CXX_FLAGS "${PEDANTIC_CXX_FLAGS} -Wno-useless-cast -Wno-missing-field-initializers")
  add_definitions(-D__STDC_LIMIT_MACROS)
endif()

if(CMAKE_COMPILER_IS_CLANGXX)
  set(PEDANTIC_CXX_FLAGS "${PEDANTIC_CXX_FLAGS} -Wno-unused-parameter -Wno-old-style-cast")
endif()

message(STATUS "C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C Flags:         ${CMAKE_C_FLAGS}")
message(STATUS "C++ Flags:       ${CMAKE_CXX_FLAGS}")
message(STATUS "C++ link flags:  ${CMAKE_CXX_LINK_FLAGS}")

#-----------------------------------------------------------------------------
# Build Unit tests

if(BUILD_TESTS)
  set(GTEST_ROOT ${opentxs_SOURCE_DIR}/deps/gtest)
  set(GTEST_FOUND ON)
  set(GTEST_INCLUDE_DIRS ${GTEST_ROOT}/include)
  set(GTEST_LIBRARY gtest)
  set(GTEST_MAIN_LIBRARY gtest_main)
  set(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})

  enable_testing()
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../abstractions/include")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(bip47 SHARED 
    ../abstractions/src/hd/bip32.cpp
    src/bip47/patterns.cpp
    src/bip47/low.cpp
    src/bip47/hd.cpp
    src/bip47/payment_code.cpp
    src/bip47/mask.cpp
    src/bip47/secret.cpp
    src/bip47/designated.cpp
    src/bip47/notification.cpp
)

# Locate libraries
find_package(libbitcoin REQUIRED)
include_directories(${LIBBITCOIN_INCLUDE_DIRS})

target_link_libraries(bip47 ${LIBBITCOIN_LIBRARIES})

find_package(Boost 1.58.0 COMPONENTS system REQUIRED)

# Locate libraries
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
add_executable(tests 
    test/bip47/main.cpp 
    test/bip47/test_keys.cpp
)

target_link_libraries(tests 
  ${Boost_SYSTEM_LIBRARY}
  ${GTEST_LIBRARIES} 
  ${LIBBITCOIN_LIBRARIES} 
  pthread
  bip47
)
#add_test(tests ${PROJECT_BINARY_DIR}/tests/${name} --gtest_output=xml:gtestresults.xml)

