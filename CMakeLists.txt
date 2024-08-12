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

add_custom_target(run DEPENDS run-cmd)

add_custom_command(
	OUTPUT run-cmd
	COMMAND ${CMAKE_COMMAND} -E echo "run cppatomic tests suite"
)

function(add_slt_ts_exe TEST_NAME)
	add_executable(${TEST_NAME} src/${TEST_NAME}.cpp)

	set_target_properties(${TEST_NAME} PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
	)

	add_dependencies(${TEST_NAME} slt_ts_utils_lib)

	target_link_libraries(${TEST_NAME} slt_ts_utils_lib)

	install(
		TARGETS ${TEST_NAME}
		EXPORT  ${CMAKE_PROJECT_NAME}
		LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
	)

	add_dependencies(run ${TEST_NAME})

	add_custom_command(
		OUTPUT run-cmd APPEND
		COMMAND ./${TEST_NAME})
endfunction()

add_slt_ts_exe(memory_order_relaxed_arr_max)
add_slt_ts_exe(memory_order_relaxed_arr_sum)
add_slt_ts_exe(memory_order_relaxed_inc_counter)

# run-cmd is not actually generated, set it as symbolic
set_source_files_properties(run-cmd PROPERTIES SYMBOLIC "true")

