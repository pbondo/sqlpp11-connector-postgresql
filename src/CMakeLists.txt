add_library(sqlpp-postgresql 
		connection.cpp
		result.cpp
		detail/connection_handle.cpp
		detail/connection_handle.h
		detail/result_handle.h
		../include/sqlpp11/postgresql/postgresql.h
		../include/sqlpp11/postgresql/connection_config.h
		../include/sqlpp11/postgresql/connection.h
		../include/sqlpp11/postgresql/result.h
		)

install(TARGETS sqlpp-postgresql DESTINATION lib)
