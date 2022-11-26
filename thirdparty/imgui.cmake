## ========================================================
##
## FILE:			/thirdparty/imgui.cmake
##
## AUTHORS:			Jaycie Ewald
##
## PROJECT:			libcohost
##
## LICENSE:			ACSL v1.4
##
## DESCRIPTION:		ImGui CMake project definition
##
## LAST EDITED:		November 26th, 2022
##
## ========================================================

##
## Definitions
##

## Project
project(ImGui LANGUAGES CXX)

##
## External
##

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

##
## Libraries
##

## ImGui
set(IMGUI_SRC
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC ${IMGUI_DIR})

if(NOT WIN32)
	target_link_libraries(imgui m)
endif()

## ImGui backends
add_library(imgui-backend-sdl STATIC ${IMGUI_DIR}/backends/imgui_impl_sdl.cpp)
target_include_directories(imgui-backend-sdl PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends ${SDL2_INCLUDE_DIRS})
target_link_libraries(imgui-backend-sdl PUBLIC ${SDL2_LIBRARIES})

add_library(imgui-backend-opengl3 STATIC ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui-backend-opengl3 PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends ${OPENGL_INCLUDE_DIRS})
target_link_libraries(imgui-backend-opengl3 PUBLIC ${CMAKE_DL_LIBS} ${OPENGL_LIBRARIES})
