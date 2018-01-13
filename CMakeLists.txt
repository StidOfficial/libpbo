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

include_directories(include/)
add_library(pbo SHARED ${LIBPBO_SRCS})

add_executable(pboinfo tools/pboinfo/src/pboinfo.cpp)
add_executable(pbopack tools/pbopack/src/pbopack.cpp)
add_executable(pbounpack tools/pbounpack/src/pbounpack.cpp)

target_include_directories(pbo PUBLIC include PRIVATE include/libpbo)
target_link_libraries(pbo OpenSSL::Crypto stdc++fs)
target_link_libraries(pboinfo stdc++fs pbo)
target_link_libraries(pbopack stdc++fs pbo)
target_link_libraries(pbounpack stdc++fs pbo)

install(TARGETS pbo LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY include/libpbo DESTINATION include)
