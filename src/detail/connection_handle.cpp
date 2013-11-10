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


#include <memory>
#include <sqlpp11/postgresql/connection_config.h>
#include "connection_handle.h"

namespace sqlpp
{
	namespace postgresql
	{
		namespace detail
		{
			connection_handle::connection_handle(const std::shared_ptr<connection_config>& conf)
			: config(conf)
			{
				std::string connst;
				if (!conf->host.empty())
					connst += "host='" + conf->host + "' ";
				if (!conf->database.empty())
					connst += "dbname='" + conf->database + "' ";
				if (!conf->user.empty())
					connst += "user='" + conf->user + "' ";
				if (!conf->password.empty())
					connst += "password='" + conf->password + "' ";
				if (conf->debug)
					std::cerr << "connection string: " << connst << std::endl;

				this->_conn = PQconnectdb(connst.c_str());
				if (0 == this->_conn || CONNECTION_OK != PQstatus(this->_conn))
				{
					if (this->_conn != 0)
					{
						PQfinish(this->_conn);
						this->_conn = 0;
					}
					throw std::runtime_error("PostgreSQL: could not connect: " + connst); 
				}
			}

			connection_handle::~connection_handle()
			{
				if (this->_conn != 0)
				{
					PQfinish(this->_conn);
					this->_conn = 0;
				}
			}
		}
	}
}


