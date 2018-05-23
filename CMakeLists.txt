cmake_minimum_required(VERSION 3.9)

project(libPBO VERSION 0.0.1)

find_package(OpenSSL REQUIRED)

set(EXECUTABLE_OUTPUT_PATH bin/)
set(LIBRARY_OUTPUT_PATH bin/)
set(LIBPBO_SRCS
    src/entry.cpp
    src/pbo.cpp
    src/productentry.cpp
)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

include_directories(include/)
add_library(pbo SHARED ${LIBPBO_SRCS})

add_executable(pboinfo tools/pboinfo/src/pboinfo.cpp)
add_executable(pbopack tools/pbopack/src/pbopack.cpp)
add_executable(pbounpack tools/pbounpack/src/pbounpack.cpp)

target_include_directories(pbo PUBLIC include PRIVATE include/libpbo)
if(MSVC)
	include_directories(pbo ${OPENSSL_INCLUDE_DIR})
	target_link_libraries(pbo ${OPENSSL_LIBRARIES})
else()
	target_link_libraries(pbo OpenSSL::Crypto stdc++fs)
endif()

target_link_libraries(pboinfo pbo)
target_link_libraries(pbopack pbo)
target_link_libraries(pbounpack pbo)

install(TARGETS pbo LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY include/libpbo DESTINATION include)
