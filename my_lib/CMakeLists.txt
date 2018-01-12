add_library(my_lib MyTable.cpp COM_port.cpp)

target_include_directories(my_lib PUBLIC ${FLTK_INCLUDE_DIRS})
