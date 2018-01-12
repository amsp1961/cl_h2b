cmake_minimum_required(VERSION 3.6.3)

project(cl_h2b)

#set (CMAKE_BUILD_TYPE MINSIZEREL)
set (FLTK_SKIP_OPENGL true)
set (FLTK_SKIP_FORMS true)
set (FLTK_SKIP_IMAGES true)
# enviromnent varialble FLTK_DIR must have been set
# if fluid.exe is used PATH must contain directory with fluid.exe
find_package(FLTK REQUIRED NO_MODULE)
if (NOT FLTK_FOUND)
    message (FATAL_ERROR "FLTK not found")
endif (NOT FLTK_FOUND)
message (STATUS "FLTK is found")

#set (CMAKE_CXX_STANDART 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -mwindows -DWIN32 -static -I${FLTK_INCLUDE_DIR}")
#set (CMAKE_LINK_FLAGS "$(CMAKE_LINK_FLAGS}")
#add_compile_options(-std=c++14)

message(STATUS ${CMAKE_CXX_FLAGS})
#message(STATUS ${CMAKE_LINK_FLAGS})
# Define the CXX sources
set ( CXX_SRCS
    ${CMAKE_SOURCE_DIR}/Controller.cpp
    ${CMAKE_SOURCE_DIR}/main.cpp
)

add_custom_command( OUTPUT  hex2bin.cxx hex2bin.h COMMAND ${FLTK_FLUID_EXECUTABLE} -c hex2bin.fl WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fluid COMMENT "Fluid.exe \n" VERBATIM)
set_source_files_properties(" ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.h" " ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.cxx" PROPERTIES GENERATED TRUE)
add_custom_target(fluid_log ALL DEPENDS hex2bin.cxx hex2bin.h)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/my_lib)
set (${LINK_OPTIONS} "${LINK_OPTIONS} my_lib fltk")

add_executable(cl_h2b ${RC_SRCS} ${CXX_SRCS} ${C_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.cxx)
target_link_libraries(cl_h2b my_lib fltk)
#NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.cxx OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.cxx OR
if(   NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.cxx OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.cxx OR
        ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.fl IS_NEWER_THAN ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.cxx  )
    add_dependencies(${PROJECT_NAME} fluid_log)
endif(  NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.cxx OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.cxx OR
        ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.fl IS_NEWER_THAN ${CMAKE_CURRENT_SOURCE_DIR}/fluid/hex2bin.cxx)

target_include_directories(cl_h2b PRIVATE fluid)
target_include_directories(cl_h2b PRIVATE ${FLTK_INCLUDE_DIRS})
target_include_directories(cl_h2b PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(cl_h2b PRIVATE my_lib)

