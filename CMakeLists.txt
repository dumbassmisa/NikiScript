cmake_minimum_required(VERSION 3.10)

project(NikiScript VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(BUILD_SHARED "Whether make it shared or static" ON)

if (NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

	option(BUILD_64BITS "x64 or x86" ON)
	if(BUILD_64BITS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
	endif()
endif()

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

if(BUILD_SHARED)
	add_library(NikiScript SHARED ${SOURCES})
	set_target_properties(NikiScript PROPERTIES POSITION_INDEPENDENT_CODE ON)
	target_compile_definitions(NikiScript PRIVATE NS_EXPORT=1)

	if (MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251") # ignore STL dll interface warning
	endif()
else()
    add_library(NikiScript STATIC ${SOURCES})
endif()

set_target_properties(NikiScript PROPERTIES VERSION ${PROJECT_VERSION})

if (WIN32)
	set_target_properties(NikiScript PROPERTIES PREFIX "")
endif()

target_include_directories(NikiScript PRIVATE ${PROJECT_SOURCE_DIR}/include/NikiScript)

add_subdirectory(${PROJECT_SOURCE_DIR}/examples)