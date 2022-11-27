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
set(CMAKE_BUILD_TYPE Release)

## Options
option(LIBCOHOST_BUILD_APPS "Build libcohost example and test applications" ON)

## DJGPP test
if(DJGPP)

## PDCurses
set(PDCURSES_DIR ${CMAKE_SOURCE_DIR}/thirdparty/PDCurses)
include(${CMAKE_SOURCE_DIR}/thirdparty/pdcurses.cmake)

add_compile_options("-std=c++11")
add_compile_definitions("alloca=__builtin_alloca")

## ImTui
set(MINGW TRUE)
set(IMTUI_DIR ${CMAKE_SOURCE_DIR}/thirdparty/imtui)
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/imtui)
target_compile_definitions(imtui-ncurses PUBLIC "_WIN32")

## DOSTEST
add_executable(dostest ${CMAKE_SOURCE_DIR}/apps/dostest.cpp)
target_link_libraries(dostest imtui imtui-ncurses)

else()

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

	set(IMTUI_DIR ${CMAKE_SOURCE_DIR}/thirdparty/imtui)
	add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/imtui)

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
target_link_libraries(cohost ${LIBCOHOST_LIBS})

## CXX version (current)
set(LIBCOHOSTXX_SRC
	${CMAKE_SOURCE_DIR}/lib_cxx/b64.c
	${CMAKE_SOURCE_DIR}/lib_cxx/cohost.cpp
)

add_library(cohostxx STATIC ${LIBCOHOSTXX_SRC})
target_include_directories(cohostxx PUBLIC ${CMAKE_SOURCE_DIR}/lib_cxx ${LIBCOHOST_INCLUDES})
target_link_libraries(cohostxx ${LIBCOHOST_LIBS})

##
## Applications
##

if(LIBCOHOST_BUILD_APPS)

	## Test0001
	add_executable(test0001 ${CMAKE_SOURCE_DIR}/apps/test0001.c)
	target_link_libraries(test0001 cohost)

	## Test0002
	add_executable(test0002 ${CMAKE_SOURCE_DIR}/apps/test0002.cpp)
	target_link_libraries(test0002 cohostxx)

	## CHSTTERM
	add_executable(chstterm ${CMAKE_SOURCE_DIR}/apps/chstterm.cpp)
	target_link_libraries(chstterm cohostxx ${IMTUI_LIBRARIES})

	## CHSTGRPH
	add_executable(chstgrph ${CMAKE_SOURCE_DIR}/apps/chstterm.cpp)
	target_link_libraries(chstgrph cohostxx ${IMGUI_LIBRARIES})
	target_compile_definitions(chstgrph PUBLIC "GRAPHICS=1")

endif()

endif() ## DJGPP
