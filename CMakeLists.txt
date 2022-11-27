## ========================================================
##
## FILE:			/CMakeLists.txt
##
## AUTHORS:			Jaycie Ewald
##
## PROJECT:			libcohost
##
## LICENSE:			ACSL v1.4
##
## DESCRIPTION:		libcohost CMake project definition
##
## LAST EDITED:		November 26th, 2022
##
## ========================================================

##
## Project Definitions
##

## CMake version gate
cmake_minimum_required(VERSION 3.13)

## Project
project(libcohost LANGUAGES C CXX)

## Build type
set(CMAKE_BUILD_TYPE Debug)

## Options
option(LIBCOHOST_BUILD_APPS "Build libcohost example and test applications" ON)

##
## Global compile definitions
##

## Release mode
if(CMAKE_BUILD_TYPE STREQUAL Release)

	add_compile_options("-O3")

endif()

##
## Thirdparty Libraries
##

## Curl
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/curl)

## cJSON
option(ENABLE_CJSON_TEST "Enable building cJSON test" OFF)
set(ENABLE_CJSON_TEST OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/cjson)

## Nettle
set(NETTLE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/nettle)
include(${CMAKE_SOURCE_DIR}/thirdparty/nettle.cmake)

if(LIBCOHOST_BUILD_APPS)

	set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/thirdparty/imgui)
	include(${CMAKE_SOURCE_DIR}/thirdparty/imgui.cmake)

	if(NOT MINGW)

		set(IMTUI_DIR ${CMAKE_SOURCE_DIR}/thirdparty/imtui)
		add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/imtui)

	endif()

endif()

##
## Text definitions
##

## Thirdparty libraries
set(LIBCOHOST_LIBS
	libcurl
	cjson
	nettle
)

set(LIBCOHOST_INCLUDES
	${CMAKE_SOURCE_DIR}/thirdparty
	${CMAKE_SOURCE_DIR}/thirdparty/curl/include
)

if(LIBCOHOST_BUILD_APPS)

	## ImGui libraries
	set(IMGUI_LIBRARIES
		imgui
		imgui-backend-sdl
		imgui-backend-opengl3
	)

	## ImTui libraries
	set(IMTUI_LIBRARIES
		imtui
		imtui-ncurses
		imgui-for-imtui
	)

endif()

##
## Libraries
##

## C version (deprecated, for now)
set(LIBCOHOST_SRC
	${CMAKE_SOURCE_DIR}/lib_c/cohost.c
)

add_library(cohost STATIC ${LIBCOHOST_SRC})
target_include_directories(cohost PUBLIC ${CMAKE_SOURCE_DIR}/lib_c ${LIBCOHOST_INCLUDES})
target_link_libraries(cohost PUBLIC ${LIBCOHOST_LIBS})

## CXX version (current)
set(LIBCOHOSTXX_SRC
	${CMAKE_SOURCE_DIR}/lib_cxx/b64.c
	${CMAKE_SOURCE_DIR}/lib_cxx/cohost.cpp
)

add_library(cohostxx STATIC ${LIBCOHOSTXX_SRC})
target_include_directories(cohostxx PUBLIC ${CMAKE_SOURCE_DIR}/lib_cxx ${LIBCOHOST_INCLUDES})
target_link_libraries(cohostxx PUBLIC ${LIBCOHOST_LIBS})

##
## Applications
##

if(LIBCOHOST_BUILD_APPS)

		## Test0001
		add_executable(test0001 WIN32 ${CMAKE_SOURCE_DIR}/apps/test0001.c)
		target_link_libraries(test0001 cohost)

		## Test0002
		add_executable(test0002 WIN32 ${CMAKE_SOURCE_DIR}/apps/test0002.cpp)
		target_link_libraries(test0002 PUBLIC cohostxx)

	if(NOT MINGW)

		## CHSTTERM
		add_executable(chstterm WIN32 ${CMAKE_SOURCE_DIR}/apps/chstterm.cpp)
		target_link_libraries(chstterm PUBLIC cohostxx ${IMTUI_LIBRARIES})

	endif()

	## CHSTGRPH
	add_executable(chstgrph WIN32 ${CMAKE_SOURCE_DIR}/apps/chstterm.cpp)
	target_link_libraries(chstgrph PUBLIC cohostxx ${IMGUI_LIBRARIES})
	target_compile_definitions(chstgrph PUBLIC "GRAPHICS=1")

endif()
