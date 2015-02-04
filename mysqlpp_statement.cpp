//
// Copyright (c) 2014-2015 WASPP (waspp.org@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <mysql/mysql.h>

#include "mysqlpp_exception.hpp"
#include "mysqlpp_statement.hpp"

namespace mysqlpp
{

statement::statement(st_mysql* mysql, const std::string& query)
{
	stmt = mysql_stmt_init(mysql);
	oss.imbue(std::locale::classic());

	try
	{
		if (!stmt)
		{
			throw exception("statement_init failed");
		}

		if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0)
		{
			throw exception(mysql_stmt_error(stmt));
		}

		param_index = 0;
		param_count = mysql_stmt_param_count(stmt);

		params.resize(0);
		params.resize(param_count);

		binds.resize(0);
		binds.resize(param_count, st_mysql_bind());
	}
	catch (...)
	{
		mysql_stmt_close(stmt);
	}
}

statement::~statement()
{
	mysql_stmt_close(stmt);
}

unsigned long long statement::execute()
{
	if (!params.empty())
	{
		for (std::size_t i = 0; i < params.size(); ++i)
		{
			params[i].make_bind(&binds[i]);
		}

		if (mysql_stmt_bind_param(stmt, &binds.front()) != 0)
		{
			throw exception(mysql_stmt_error(stmt));
		}
	}

	if (mysql_stmt_execute(stmt) != 0)
	{
		throw exception(mysql_stmt_error(stmt));
	}

	return mysql_stmt_affected_rows(stmt);
}

result* statement::execute_query()
{
	if (mysql_stmt_execute(stmt) != 0)
	{
		throw exception(mysql_stmt_error(stmt));
	}

	return new result(stmt);
}

} // namespace mysqlpp
