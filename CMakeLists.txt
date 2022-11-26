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
## Thirdparty Libraries
##

if(LIBCOHOST_BUILD_APPS)

	add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/imtui)

endif()

##
## Text definitions
##

## Thirdparty libraries
set(LIBCOHOST_LIBS_DYNAMIC curl)
set(LIBCOHOST_LIBS_STATIC libcjson.a libnettle.a)
set(LIBCOHOST_LIBS ${LIBCOHOST_LIBS_DYNAMIC} ${LIBCOHOST_LIBS_STATIC})

if(LIBCOHOST_BUILD_APPS)

	## ImGUI / ImTUI
	set(IMGUI_LIBRARIES
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
target_include_directories(cohost PUBLIC ${CMAKE_SOURCE_DIR}/lib_c)
target_link_libraries(cohost ${LIBCOHOST_LIBS})

## CXX version (current)
set(LIBCOHOSTXX_SRC
	${CMAKE_SOURCE_DIR}/lib_cxx/b64.c
	${CMAKE_SOURCE_DIR}/lib_cxx/cohost.cpp
)

add_library(cohostxx STATIC ${LIBCOHOSTXX_SRC})
target_include_directories(cohostxx PUBLIC ${CMAKE_SOURCE_DIR}/lib_cxx)
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
	target_link_libraries(chstterm cohostxx ${IMGUI_LIBRARIES})

endif()