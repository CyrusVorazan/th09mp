cmake_minimum_required(VERSION 3.14.0)
project(th09mp VERSION 0.0.1)

add_executable(th09mp exe/main.cpp)
add_library(inject SHARED 
	inject/callback.cpp 	inject/callback.h 
	inject/inject.cpp 		inject/inject.h 
	inject/network.cpp      inject/network.h
	inject/state.cpp        inject/state.h
	inject/th09address.cpp 	inject/th09address.h 
	inject/th09types.h 		
	inject/breakpoint.cpp	inject/breakpoint.h
	inject/menu.cpp			inject/menu.h
	inject/main.cpp			inject/inject.def
	
	lib/imgui/imgui.cpp 
	lib/imgui/imgui_demo.cpp
	lib/imgui/imgui_draw.cpp
	lib/imgui/imgui_widgets.cpp
	lib/imgui/examples/imgui_impl_dx9.cpp
	lib/imgui/examples/imgui_impl_win32.cpp
	)

target_link_libraries(th09mp 
	shlwapi
	)

target_include_directories(th09mp PRIVATE 
	${CMAKE_SOURCE_DIR}/inject
    ${CMAKE_SOURCE_DIR}/exe
    )

target_link_libraries(inject 
	d3d9
	ws2_32
	${CMAKE_SOURCE_DIR}/lib/RakNet/Lib/RakNet_VS2008_LibStatic_Debug_Win32.lib
	)

target_include_directories(inject PRIVATE 
	${CMAKE_SOURCE_DIR}/inject
	${CMAKE_SOURCE_DIR}/lib/imgui
	${CMAKE_SOURCE_DIR}/lib/imgui/examples
	${CMAKE_SOURCE_DIR}/lib/RakNet/Source
    )
	
# Add the path to the game folder if you want to output the project there.
# set_target_properties( th09mp PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "" )
# set_target_properties( inject PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "" )