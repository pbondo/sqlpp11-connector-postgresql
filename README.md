sqlpp11-connector-postgresql
============================

A C++ wrapper for postgresql meant to be used in combination with sqlpp11 (https://github.com/rbock/sqlpp11).

NOT to be used for production code. It was just implemented to see how sqlpp11 works. It will probably leak memory and eat your cat.

Sample Code:
------------
See for instance test/SampleTest.cpp

```C++
#include "TabSample.h"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/postgresql/postgresql.h>

namespace postgresql = sqlpp::postgresql;
int main()
{
	auto config = std::make_shared<postgresql::connection_config>();
 	config->user = "test";
 	config->database = "sqlpp";
	config->debug = true;
	postgresql::connection db(config);

	TabSample tab;
	for(const auto& row : sqlpp::select(all_of(tab)).from(tab).run(db))
	{
		std::cerr << "row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma <<  std::endl;
	};
  return 0;
}
```


Requirements:
-------------
__Compiler:__
sqlpp11-connector-postgresql makes use of C++11 and requires a recent compiler and STL. The following compilers are known to compile the test programs:

  * g++-4.8 on Ubuntu-13.10

__C++ SQL Layer:__
sqlpp11-connector-postgresql is meant to be used with sqlpp11 (https://github.com/rbock/sqlpp11).


