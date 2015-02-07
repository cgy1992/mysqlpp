
#include <iostream>

#include <mysql/mysql.h>

#include "mysqlpp_result.hpp"
#include "mysqlpp_statement.hpp"
#include "mysqlpp_connection.hpp"

int main()
{
	try
	{
		mysqlpp::connection* conn = new mysqlpp::connection("127.0.0.1", "root", "1235", "test");

		mysqlpp::statement* stmt;
		stmt = conn->prepare("DROP TABLE IF EXISTS test");
		stmt->execute();

		stmt = conn->prepare("CREATE TABLE test(col1 TINYINT, col2 SMALLINT, col3 INT, col4 BIGINT, col5 FLOAT, col6 DOUBLE, col7 VARCHAR(10), col8 TEXT, col9 DATETIME, col10 INT NULL)");
		stmt->execute();

		unsigned char param1 = 1;
		short int param2 = 2;
		int param3 = 3;
		long long int param4 = 4;
		float param5 = 5;
		double param6 = 6;

		std::string param7("param7");
		unsigned long param7_size = param7.size();

		std::string param8("param8");
		unsigned long param8_size = param8.size();
		
		st_mysql_time param9;
		{
			param9.year = 1970;
			param9.month = 1;
			param9.day = 1;
			param9.hour = 0;
			param9.minute = 0;
			param9.second = 0;
		}

		stmt = conn->prepare("INSERT INTO test(col1, col2, col3, col4, col5, col6, col7, col8, col9, col10) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		{
			stmt->param(param1);
			stmt->param(param2);
			stmt->param(param3);
			stmt->param(param4);
			stmt->param(param5);
			stmt->param(param6);
			stmt->param(param7, param7_size);
			stmt->param(param8, param8_size);
			stmt->param(param9);
			stmt->param_null();
		}
		stmt->execute();

		// insert one more time
		stmt->execute();

		//stmt = conn->prepare_statement("SELECT * from test");
		//mysqlpp::result* rs = stmt->query();

		//if (rs->next())
		//{
			//std::cout << 1 << std::endl;
		//}

		//while (rs->next())
		//{
			//std::cout << rs->get<int>(0) << std::endl;
			//std::cout << rs->get<int>("id2") << std::endl;
		//}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;

		std::cin.get();
		return 1;
	}

	std::cout << "OK" << std::endl;

	std::cin.get();
	return 0;
}
