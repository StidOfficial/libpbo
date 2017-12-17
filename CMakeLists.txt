cmake_minimum_required(VERSION 3.0)

project(libPBO)
#set(CMAKE_CXX_STANDARD 17)

include(FindOpenSSL)
find_package(OpenSSL)
if(NOT OPENSSL_FOUND)
    message(FATAL "We need OPENSLL aborting")
endif()

add_library(pbo 
    include/libpbo/entry.hpp
    include/libpbo/pbo.hpp
    include/libpbo/productentry.hpp

    src/entry.cpp
    src/pbo.cpp
    src/productentry.cpp
)

target_include_directories(pbo PUBLIC include
    PRIVATE include/libpbo)    
target_link_libraries(pbo ${OPENSSL_CRYPTO_LIBRARY} stdc++fs)


add_subdirectory(test)

