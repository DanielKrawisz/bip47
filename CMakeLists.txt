cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)

project(bip47)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(bip47 SHARED 
    src/bip47/patterns.cpp
    src/bip47/low.cpp
    src/bip47/hd.cpp
    src/bip47/payment_code.cpp
    src/bip47/secret.cpp
    src/bip47/notification.cpp
)

# Locate libraries
find_package(libbitcoin REQUIRED)
include_directories(${LIBBITCOIN_INCLUDE_DIRS})
 
# Link runTests with what we want to test and the GTest and pthread library
target_link_libraries(bip47 ${LIBBITCOIN_LIBRARIES})
