cmake_minimum_required(VERSION 3.18) 
project(adding_lua)

set(CMAKE_CXX_STANDARD 20)
set(APP_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies")
set(LUA_DIR "${DEPENDENCIES_DIR}/lua")
set(SOL_DIR "${DEPENDENCIES_DIR}/sol2/include")


add_library(
    lua STATIC
    ${LUA_DIR}/onelua.c
)


target_compile_definitions(lua PRIVATE MAKE_LIB)

add_compile_definitions(SOL_ALL_SAFETIES_ON)


add_executable(
	${PROJECT_NAME}
	${APP_SOURCE_DIR}/main.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${APP_SOURCE_DIR}
    PRIVATE ${LUA_DIR}
    PRIVATE ${SOL_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    lua
)

if (EMSCRIPTEN)
    target_compile_options(
		${PROJECT_NAME} 
		PRIVATE
        -O3
        -Wall
        -sUSE_SDL=2
    )

    # target_link_options(
	# 	${PROJECT_NAME} 
	# 	PRIVATE
    #     -sUSE_SDL=2
    #     -sALLOW_MEMORY_GROWTH=1
    #     "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
    # )

    set_target_properties(${PROJECT_NAME} PROPERTIES 
        LINK_FLAGS "-sUSE_SDL=2 -sALLOW_MEMORY_GROWTH=1 -sDISABLE_EXCEPTION_CATCHING=0 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
    )

    set_target_properties( ${PROJECT_NAME} PROPERTIES SUFFIX ".html" )
endif()

