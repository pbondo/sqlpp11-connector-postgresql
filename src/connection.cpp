/*
 * Copyright (c) 2013, Roland Bock, Poul Bondo
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <iostream>
#include <sqlpp11/exception.h>
#include <sqlpp11/postgresql/connection.h>
#include "detail/result_handle.h"
#include "detail/connection_handle.h"

namespace sqlpp
{
	namespace postgresql
	{
		namespace
		{

			result execute_query(detail::connection_handle& handle, const std::string& query)
			{
				if (handle.config->debug)
					std::cerr << "PostgreSQL debug: Executing: '" << query << "'" << std::endl;

				PGresult * res = PQexec(handle._conn, query.c_str());
				std::unique_ptr<detail::result_handle> result_handle(new detail::result_handle(res));
				return result(std::move(result_handle), handle.config->debug);
			}


			size_t get_affected_rows( result & _result )
			{
				const char * const resultStr = PQcmdTuples(_result.get()->_result);	//NB!! This stinks
				char * end;
				long long result1 = std::strtoll(resultStr, &end, 0);
				if (end != resultStr)
				{
				  return result1;
				}
				return -1;
			}

		}

		connection::connection(const std::shared_ptr<connection_config>& config):
			_handle(new detail::connection_handle(config))
		{
		}

		connection::~connection()
		{
		}

		connection::_result_t connection::select(const std::string& query)
		{
			return execute_query(*_handle, query);
		}

		size_t connection::insert(const std::string& query)
		{
			result res = execute_query(*_handle, query);
			return 0; //NB!! we should return the OID if available. RETURNING ID will not work in this case.
		}

		result connection::execute(const std::string& command)
		{
			return execute_query(*_handle, command);
		}

		size_t connection::update(const std::string& query)
		{
			result res = execute_query(*_handle, query);
			return get_affected_rows(res);
		}

		size_t connection::remove(const std::string& query)
		{
			result res = execute_query(*_handle, query);
			return get_affected_rows(res);
		}

		std::string connection::escape(const std::string& s) const
		{
			return s;
		}

		void connection::start_transaction()
		{
			if (_transaction_active)
			{
				throw sqlpp::exception("Cannot have more than one open transaction per connection");
			}
			execute_query(*_handle, "START TRANSACTION");
			_transaction_active = true;
		}

		void connection::commit_transaction()
		{
			if (not _transaction_active)
			{
				throw sqlpp::exception("Cannot commit a finished or failed transaction");
			}
			_transaction_active = false;
			execute_query(*_handle, "COMMIT");
		}

		void connection::rollback_transaction(bool report)
		{
			if (not _transaction_active)
			{
				throw sqlpp::exception("Cannot rollback a finished or failed transaction");
			}
			if (report)
			{
				std::cerr << "PostgreSQL warning: Rolling back unfinished transaction" << std::endl;
			}
			_transaction_active = false;
			execute_query(*_handle, "ROLLBACK");
		}

		void connection::report_rollback_failure(const std::string message) noexcept
		{
			std::cerr << "PostgreSQL message:" << message << std::endl;
		}
	}
}

