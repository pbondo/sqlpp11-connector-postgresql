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


#ifndef SQLPP_POSTGRESQL_RESULT_H
#define SQLPP_POSTGRESQL_RESULT_H

#include <memory>
#include <vector>
#include <sqlpp11/raw_result_row.h>

namespace sqlpp
{
	namespace postgresql
	{
		namespace detail
		{
			struct result_handle;
		}

		class result
		{
			std::unique_ptr<detail::result_handle> _handle;
			std::vector<const char*> _raw_fields;
			std::vector<size_t> _raw_sizes;
			bool _debug;
			int _row_index = -1;

		public:
		
			std::unique_ptr<detail::result_handle> & get() { return _handle; }

			result();
			result(std::unique_ptr<detail::result_handle>&& handle, const bool debug);
			result(const result&) = delete;
			result(result&& rhs);
			result& operator=(const result&) = delete;
			result& operator=(result&&);
			~result();

			bool operator==(const result& rhs) const
			{
				return _handle == rhs._handle;
			}

			//! return the next row from the result or nullptr, if there is no next row
			raw_result_row_t next();
			size_t num_cols() const;
		};

	}
}
#endif
