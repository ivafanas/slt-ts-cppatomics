cmake_minimum_required (VERSION 3.10.0)

project(slt-ts-cppatomics)

#
# Common utils library
#
add_library(slt_ts_utils_lib
	src/utils.cpp
	src/utils.h
)

set_target_properties(slt_ts_utils_lib PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

#
# Tests
#

function(add_slt_ts_exe TEST_NAME)
	add_executable(${TEST_NAME} src/${TEST_NAME}.cpp)

	set_target_properties(${TEST_NAME} PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
	)

	add_dependencies(${TEST_NAME} slt_ts_utils_lib)

	target_link_libraries(${TEST_NAME} slt_ts_utils_lib)
endfunction()

add_slt_ts_exe(memory_order_relaxed_arr_max)
add_slt_ts_exe(memory_order_relaxed_arr_sum)
add_slt_ts_exe(memory_order_relaxed_inc_counter)
