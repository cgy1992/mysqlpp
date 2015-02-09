//
// Copyright (c) 2014-2015 WASPP (waspp.org@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <string>
#include <vector>
#include <sstream>

#include <mysql/mysql.h>

#include "mysqlpp_exception.hpp"
#include "mysqlpp_result.hpp"

namespace mysqlpp
{

	result::result(st_mysql_stmt* stmt_) : stmt(stmt_), metadata(0), fields(0)
	{
		metadata = mysql_stmt_result_metadata(stmt);

		try
		{
			if (!metadata)
			{
				throw exception(mysql_stmt_error(stmt));
			}

			field_count = mysql_stmt_field_count(stmt);
			if (field_count == 0)
			{
				throw exception("zero field_count");
			}

			fields = mysql_fetch_fields(metadata);

			if (mysql_stmt_store_result(stmt) != 0)
			{
				throw exception(mysql_stmt_error(stmt));
			}
		}
		catch (...)
		{
			mysql_free_result(metadata);
			throw;
		}
	}

	result::~result()
	{
		mysql_free_result(metadata);
	}

	unsigned long long result::num_rows()
	{
		return mysql_stmt_num_rows(stmt);
	}

	bool result::fetch()
	{
		binds.resize(0);
		binds.resize(field_count, st_mysql_bind());

		columns.resize(0);
		columns.resize(field_count, st_mysql_column());

		for (std::size_t i = 0; i < field_count; ++i)
		{
			columns[i].name = std::string(fields[i].name);
			columns[i].type = fields[i].type;
			columns[i].buffer.resize(0);
			columns[i].buffer.resize(fields[i].length);

			binds[i].buffer_type = fields[i].type;
			binds[i].buffer = const_cast<char*>(&columns[i].buffer.front());
			binds[i].length = &columns[i].length;
			binds[i].error = &columns[i].error;
		}

		if (mysql_stmt_bind_result(stmt, &binds.front()) != 0)
		{
			throw exception(mysql_stmt_error(stmt));
		}

		int fetch_result = mysql_stmt_fetch(stmt);

		if (fetch_result == MYSQL_NO_DATA)
		{
			return false;
		}
		else if (fetch_result == MYSQL_DATA_TRUNCATED)
		{
			for (unsigned int i = 0; i < field_count; ++i)
			{
				columns[i].buffer.resize(columns[i].length);

				binds[i].buffer = &columns[i].buffer.front();
				binds[i].buffer_length = columns[i].length;

				if (mysql_stmt_fetch_column(stmt, &binds[i], i, 0) != 0)
				{
					throw exception(mysql_stmt_error(stmt));
				}
			}
		}

		return true;
	}

	void result::fetch_column(const st_mysql_column& column, unsigned char& value)
	{
		value = static_cast<unsigned char>(column.buffer.front());
	}

	void result::fetch_column(const st_mysql_column& column, short int& value)
	{
		value = static_cast<short int>(column.buffer.front());
	}

	void result::fetch_column(const st_mysql_column& column, int& value)
	{
		value = static_cast<int>(column.buffer.front());
	}

	void result::fetch_column(const st_mysql_column& column, long long int& value)
	{
		value = static_cast<long long int>(column.buffer.front());
	}

	void result::fetch_column(const st_mysql_column& column, float& value)
	{
		char* buffer = const_cast<char*>(&column.buffer.front());
		value = *reinterpret_cast<float*>(buffer);
	}

	void result::fetch_column(const st_mysql_column& column, double& value)
	{
		char* buffer = const_cast<char*>(&column.buffer.front());
		value = *reinterpret_cast<double*>(buffer);
	}

	void result::fetch_column(const st_mysql_column& column, std::string& value)
	{
		value.assign(&column.buffer.front(), column.length);
	}

	void result::fetch_column(const st_mysql_column& column, st_time& value)
	{
		char* buffer = const_cast<char*>(&column.buffer.front());
		value = *reinterpret_cast<st_time*>(buffer);
	}

	std::string result::time_to_str(const st_time& time)
	{
		std::ostringstream oss;
		oss << time.year << '-' << time.month << '-' << time.day << ' ' << time.hour << ':' << time.minute << ':' << time.second;

		return oss.str();
	}

	st_mysql_column& result::this_column(unsigned int index)
	{
		if (index >= field_count)
		{
			throw exception("invalid column_index");
		}

		return columns.at(index);
	}

	st_mysql_column& result::this_column(const std::string& name)
	{
		unsigned int i = 0;
		for (; i < field_count; ++i)
		{
			if (name == columns[i].name)
			{
				break;
			}
		}

		if (i == field_count)
		{
			throw exception("invalid column_name");
		}

		return columns.at(i);
	}

} // namespace mysqlpp
