/*
 * Copyright (c) 2013, Roland Bock, Poul Bondo
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "TabSample.h"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/postgresql/postgresql.h>

#include <iostream>
#include <vector>


SQLPP_ALIAS_PROVIDER_GENERATOR(left);
SQLPP_ALIAS_PROVIDER_GENERATOR(right);

namespace postgresql = sqlpp::postgresql;
int main()
{
	auto config = std::make_shared<postgresql::connection_config>();
	config->user = "test";
	config->database = "sqlpp11";
	config->debug = true;
	try
	{
		postgresql::connection db(config);
	}
	catch(const sqlpp::exception& )
	{
		std::clog << "For testing, you'll need to create a database sqlpp with a table tab_sample, as shown in tests/TabSample.sql" << std::endl;
		throw;
	}
	postgresql::connection db(config);
	db.execute(R"(DROP TABLE IF EXISTS tab_sample)");
	db.execute(R"(CREATE TABLE tab_sample (
		alpha BIGSERIAL,
		beta TEXT DEFAULT NULL,
		gamma INT DEFAULT NULL
		))");
	db.execute(R"(DROP TABLE IF EXISTS tab_foo)");
	db.execute(R"(CREATE TABLE tab_foo (
		omega BIGSERIAL
		))");

	TabSample tab;
	// clear the table
	db.run(remove_from(tab));

	// explicit all_of(tab)
	for(const auto& row : select(all_of(tab)).from(tab).run(db))
	{
		std::clog << "row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma <<  std::endl;
	};
	// selecting two multicolumns
	for(const auto& row : db.run(
						select(tab.alpha,
								 multi_column(left, tab.alpha, tab.beta, tab.gamma), 
								 multi_column(tab, all_of(tab)))
						.from(tab)))
	{
		std::clog << "row.left.alpha: " << row.left.alpha 
							<< ", row.left.beta: " << row.left.beta 
							<< ", row.left.gamma: " << row.left.gamma <<  std::endl;
		std::clog << "row.tabSample.alpha: " << row.tabSample.alpha 
							<< ", row.tabSample.beta: " << row.tabSample.beta 
							<< ", row.tabSample.gamma: " << row.tabSample.gamma <<  std::endl;
	};


	// insert
	db.run(insert_into(tab).set(tab.beta = "Hello", tab.gamma = false));
	db.run(insert_into(tab).set(tab.gamma = true));

	// update
	db.run(update(tab).set(tab.gamma = false).where(tab.alpha.in(1)));
	db.run(update(tab).set(tab.gamma = false).where(tab.alpha.in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));

	// remove
	db.run(remove_from(tab).where(tab.alpha == tab.alpha + 3));


	decltype(db.run(select(all_of(tab)))) result;
	result = db.run(select(all_of(tab)).from(tab));
	std::clog << "Accessing a field directly from the result (using the current row): " << result.begin()->alpha << std::endl;
	std::clog << "Can do that again, no problem: " << result.begin()->alpha << std::endl;

	auto tx = start_transaction(db);
	if (const auto& row = *db.run(select(all_of(tab), select(max(tab.alpha)).from(tab)).from(tab)).begin())
	{
		int x = row.alpha;
		int a = row.max;
		std::clog << "-----------------------------" << row.beta << std::endl;
	}
	tx.commit();


	TabFoo foo;
	for (const auto& row : db.run(select(tab.alpha).from(tab.join(foo).on(tab.alpha == foo.omega))))
	{
		std::clog << row.alpha << std::endl;
	}

	for (const auto& row : db.run(select(tab.alpha).from(tab.left_outer_join(foo).on(tab.alpha == foo.omega))))
	{
		std::clog << row.alpha << std::endl;
	}

	return 0;
}
