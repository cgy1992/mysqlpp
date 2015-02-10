//
// Copyright (c) 2014-2015 WASPP (waspp.org@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MYSQLPP_HPP
#define MYSQLPP_HPP

#include <ctime>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

#include <mysql/mysql.h>

namespace mysqlpp
{

	class exception : public std::runtime_error
	{
	public:
		exception(const std::string& what) : std::runtime_error(what)
		{

		}
	};

	//typedef st_mysql_time datetime;

	/*
	struct datetime
	{
		datetime()
		{
			std::time_t time_ = std::time(0);
			std::tm time = *std::localtime(&time_);

			convert(time);
		}

		datetime(st_mysql_time time_) : value(time_)
		{

		}

		datetime(const std::string& str)
		{
			std::tm time;
			int count = sscanf(str.c_str(), "%d-%d-%d %d:%d:%d", &time.tm_year, &time.tm_mon, &time.tm_mday, &time.tm_hour, &time.tm_min, &time.tm_sec);
			if (count != 3 && count != 6)
			{
				throw exception("datetime cast failed");
			}

			time.tm_year -= 1900;
			time.tm_mon -= 1;
			time.tm_isdst = -1;

			if (mktime(&time) == -1)
			{
				throw exception("datetime cast failed");
			}

			convert(time);
		}

		void convert(std::tm time)
		{
			value.year = static_cast<unsigned int>(time.tm_year) + 1900;
			value.month = static_cast<unsigned int>(time.tm_mon) + 1;
			value.day = static_cast<unsigned int>(time.tm_mday);
			value.hour = static_cast<unsigned int>(time.tm_hour);
			value.minute = static_cast<unsigned int>(time.tm_min);
			value.second = static_cast<unsigned int>(time.tm_sec);
		}

		std::tm convert()
		{
			std::tm time;

			time.tm_year = static_cast<int>(value.year) - 1900;
			time.tm_mon = static_cast<int>(value.month) - 1;
			time.tm_mday = static_cast<int>(value.day);
			time.tm_hour = static_cast<int>(value.hour);
			time.tm_min = static_cast<int>(value.minute);
			time.tm_sec = static_cast<int>(value.second);
			time.tm_isdst = -1;

			return time;
		}

		std::string str()
		{
			char buf[32] = { 0 };

			std::tm time = convert();
			strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &time);

			return std::string(buf);
		}

		st_mysql_time value;
	};
	*/

	struct st_mysql_column
	{
		st_mysql_column() : buffer(0), length(0), is_null(0), error(0)
		{
		}

		enum_field_types type;
		std::string name;

		std::vector<char> buffer;
		unsigned long length;

		char is_null;
		char error;
	};

	class result
	{
	public:
		result(st_mysql_stmt* stmt_);
		~result();

		unsigned long long num_rows();
		bool fetch();

		template<typename T>
		T get(unsigned int index)
		{
			st_mysql_column& column = this_column(index);
			if (column.is_null)
			{
				throw exception("null value field");
			}

			T value = T();
			fetch_column(column, value);

			return value;
		}

		template<typename T>
		T get(const std::string& name)
		{
			st_mysql_column& column = this_column(name);
			if (column.is_null)
			{
				throw exception("null value field");
			}

			T value = T();
			fetch_column(column, value);

			return value;
		}

		void fetch_column(const st_mysql_column& column, unsigned char& value);
		void fetch_column(const st_mysql_column& column, short int& value);
		void fetch_column(const st_mysql_column& column, int& value);
		void fetch_column(const st_mysql_column& column, long long int& value);
		void fetch_column(const st_mysql_column& column, float& value);
		void fetch_column(const st_mysql_column& column, double& value);
		void fetch_column(const st_mysql_column& column, std::string& value);
		void fetch_column(const st_mysql_column& column, st_mysql_time& value);

	private:
		st_mysql_column& this_column(unsigned int index);
		st_mysql_column& this_column(const std::string& name);

		st_mysql_stmt* stmt;
		st_mysql_res* metadata;
		st_mysql_field* fields;

		unsigned int field_count;

		std::vector<st_mysql_bind> binds;
		std::vector<st_mysql_column> columns;
	};

	class statement
	{
	public:
		statement(st_mysql* mysql, const std::string& query);
		~statement();

		void param(const unsigned char& value);
		void param(const short int& value);
		void param(const int& value);
		void param(const long long int& value);
		void param(const float& value, unsigned long length = 0);
		void param(const double& value, unsigned long length = 0);
		void param(const st_mysql_time& value);
		void param(const std::string& value, unsigned long& length);
		void param_null(char is_null = 1);

		unsigned long long execute();
		result* query();

	private:
		st_mysql_bind& this_bind();
		
		st_mysql_stmt* stmt;

		int param_count;
		int bind_index;

		std::vector<st_mysql_bind> binds;
	};

	class connection
	{
	public:
		connection(const std::string& host, const std::string& userid, const std::string& passwd, const std::string& dbname, unsigned int port = 3306, bool pooled_ = false);
		~connection();

		statement* prepare(const std::string& query);

	private:
		st_mysql* mysql;

		std::tm released;
		bool pooled;

	};

} // namespace mysqlpp

#endif // MYSQLPP_HPP
