#pragma once
#include <memory>

#include "mysql_driver.h"
#include "mysql_connection.h"
#include "cppconn/resultset.h"
#include "cppconn/prepared_statement.h"
#include "crow/json.h"

class MySQLManager
{
public:
	MySQLManager(); // Default constructor
	~MySQLManager(); // Default destructor

	MySQLManager(const MySQLManager&) = delete; // Copy constructor
	MySQLManager(MySQLManager&&) = delete; // Move constructor
	MySQLManager& operator=(const MySQLManager&) = delete; // Copy assignment operator
	MySQLManager& operator=(MySQLManager&&) = delete; // Move assignment operator

	bool Connect(const std::string& hostname, const std::string& username, const std::string& password, const std::string& database = "");
	
	void CreateList(const std::string& email, const std::string& name);
	void CreateTask(const std::string& email, const std::string& list, const std::string& name);
	
	crow::json::wvalue GetLists(const std::string& email);

	bool DeleteList(const std::string& email, const std::string& name);
	bool DeleteTask(const std::string& email, const std::string& list, const std::string& name);

	void UpdateTaskComplete(const std::string& email, const std::string& list, const std::string& name, const bool& complete);
	void UpdateTaskDesc(const std::string& email, const std::string& list, const std::string& name, const std::string& desc);
	
private:
	sql::Driver* m_Driver;
	sql::Connection* m_Connection;

};

//try
//{
//    sql::mysql::MySQL_Driver* driver;
//    sql::Connection* con;
//    sql::Statement* stmt;
//    sql::ResultSet* res;

//    driver = sql::mysql::get_mysql_driver_instance();
//    con = driver->connect("tcp://127.0.0.1:3306", SQLHostName, SQLPassword); // TODO move password to console input?
//    con->setSchema("TODOListDataBase");

//    std::cout << "Connected to MySQL successfully!" << std::endl;

//    stmt = con->createStatement();

//    std::string query = "Select TODOList.name FROM TODOList "
//                        "JOIN Users ON TODOList.ownerID = Users.id "
//                        "WHERE Users.email = '" + email + "'";

//    res = stmt->executeQuery(query);

//    std::cout << "Todo lists for user " << email << ":" << std::endl;
//    while (res->next())
//    {
//        std::cout << res->getString("name") << std::endl;
//    }

//    std::cout << std::endl;

//    delete res;
//    delete stmt;
//    delete con;
//}
//catch (sql::SQLException& e)
//{
//    std::cerr << "SQL error: " << e.what() << std::endl;
//    return 1;
//}