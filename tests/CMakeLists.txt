macro (build_and_run arg)
	add_executable(${arg} ${arg}.cpp TabSample.h)
	target_link_libraries (${arg} sqlpp-postgresql)
	target_link_libraries (${arg} pq)
	add_test(${arg} ${arg})
endmacro ()

build_and_run(SampleTest)
build_and_run(SelectTest)
