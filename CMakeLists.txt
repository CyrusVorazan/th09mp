cmake_minimum_required(VERSION 3.14.0)
project(th09mp VERSION 0.0.1)

add_executable(th09mp exe/main.cpp)
add_library(inject SHARED 
	inject/callback.cpp 	inject/callback.h 
	inject/inject.cpp 		inject/inject.h 
	inject/th09address.cpp 	inject/th09address.h 
	inject/th09types.h 
	inject/main.cpp
	)

target_link_libraries(th09mp shlwapi)

target_include_directories(th09mp PRIVATE 
	${CMAKE_SOURCE_DIR}/inject
    ${CMAKE_SOURCE_DIR}/exe
    )