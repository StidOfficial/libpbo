cmake_minimum_required(VERSION 3.9)

project(libPBO VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
find_package(PkgConfig)

set(EXECUTABLE_OUTPUT_PATH bin/)
set(LIBRARY_OUTPUT_PATH bin/)
set(LIBPBO_SRCS
	src/cryptokey.cpp
	src/entry.cpp
	src/pbo.cpp
	src/properties.cpp
	src/signature.cpp
	src/signature_generator.cpp
)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-Wall -Wextra -pedantic-errors -ansi -Wextra -Wall -Winit-self -Wold-style-cast -Woverloaded-virtual -Wuninitialized -Wmissing-declarations -Winit-self)
endif()

include_directories(include/)
add_library(pbo SHARED ${LIBPBO_SRCS})
target_compile_definitions(pbo PUBLIC -DPBODLL_EXPORTS)

# gtkmm
pkg_check_modules(GTKMM gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})

add_executable(pboinfo tools/pboinfo/src/pboinfo.cpp)
add_executable(pbopack tools/pbopack/src/pbopack.cpp)
add_executable(pbounpack tools/pbounpack/src/pbounpack.cpp)
add_executable(pbocreatekey tools/pbocreatekey/src/pbocreatekey.cpp)
add_executable(pbosignfile tools/pbosignfile/src/pbosignfile.cpp)
add_executable(pbocheck tools/pbocheck/src/pbocheck.cpp)
add_executable(pbomanager tools/pbomanager/src/pbomanager.cpp tools/pbomanager/src/PBOManagerWindow.cpp tools/pbomanager/src/PBOManagerProperties.cpp)

target_include_directories(pbo PUBLIC include PRIVATE include/libpbo)
if(MSVC)
	target_link_libraries(pbo OpenSSL::Crypto)

	target_link_libraries(pboinfo pbo)
	target_link_libraries(pbopack pbo)
	target_link_libraries(pbounpack pbo)
	target_link_libraries(pbocheck pbo)
	target_link_libraries(pbomanager pbo ${GTKMM_LIBRARIES})
else()
	target_link_libraries(pbo OpenSSL::Crypto stdc++fs)

	target_link_libraries(pboinfo stdc++fs pbo)
	target_link_libraries(pbopack stdc++fs pbo)
	target_link_libraries(pbounpack stdc++fs pbo)
	target_link_libraries(pbocheck stdc++fs pbo)
	target_link_libraries(pbomanager stdc++fs pbo ${GTKMM_LIBRARIES})
endif()

target_link_libraries(pbocreatekey OpenSSL::Crypto pbo)
target_link_libraries(pbosignfile OpenSSL::Crypto pbo)

install(TARGETS pbo LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY include/libpbo DESTINATION include)
