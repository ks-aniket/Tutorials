cmake_minimum_required(VERSION 3.18) 
project(draw_rect)

set(CMAKE_CXX_STANDARD 20)
set(APP_SOURCE_DIR "${PROJECT_SRC_DIR}src")

add_executable(
	${PROJECT_NAME}
	${APP_SOURCE_DIR}/main.cpp
)

if (EMSCRIPTEN)
    target_compile_options(
		${PROJECT_NAME} 
		PRIVATE
        -O3
        -Wall
        -sUSE_SDL=2
    )

    target_link_options(
		${PROJECT_NAME} 
		PRIVATE
        -sUSE_SDL=2
        -sALLOW_MEMORY_GROWTH=1
    )

    set_target_properties( ${PROJECT_NAME} PROPERTIES SUFFIX ".html" )
endif()

