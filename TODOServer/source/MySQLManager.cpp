#include "MySQLManager.h"


MySQLManager::MySQLManager()
{
	m_Driver = sql::mysql::get_mysql_driver_instance();
}

MySQLManager::~MySQLManager()
{
	if (!m_Connection)
		delete m_Connection;
}

bool MySQLManager::Connect(const std::string& hostname, const std::string& username, const std::string& password, const std::string& database)
{
	
	if (!m_Connection)
		delete m_Connection;

	m_Connection = m_Driver->connect(hostname, username, password);

	if (!database.empty())
		m_Connection->setSchema(database);

	return true;
}

void MySQLManager::CreateUser(const std::string& id, const std::string& email, const std::string& name)
{
	std::string query = "INSERT INTO Users (gID, email, userName) "
		"VALUES(? , ? , ? )";

	sql::PreparedStatement* statement = m_Connection->prepareStatement(query);

	statement->setString(1, id);
	statement->setString(2, email);
	statement->setString(3, name);

	statement->execute();

	delete statement;
}

void MySQLManager::CreateList(const std::string& id, const std::string& email, const std::string& name)
{
	std::string query = "INSERT INTO TODOList (ownerId, name) "
		"VALUES((SELECT id FROM Users WHERE Users.gID=? AND Users.email=?), ?)";

	sql::PreparedStatement* statement = m_Connection->prepareStatement(query);
	
	statement->setString(1, id);
	statement->setString(2, email);
	statement->setString(3, name);

	statement->execute();

	delete statement;
}

void MySQLManager::CreateTask(const std::string& id, const std::string& email, const std::string& list, const std::string& name)
{
	std::string query = "INSERT INTO Tasks (ownerId, name) "
		"VALUES((SELECT id FROM TODOList WHERE name=? AND ownerId = (SELECT id FROM Users WHERE email=? AND gID=?)), ?)";

	sql::PreparedStatement* statement = m_Connection->prepareStatement(query);

	statement->setString(1, list);
	statement->setString(2, email);
	statement->setString(3, id);
	statement->setString(4, name);

	statement->execute();

	delete statement;
}

crow::json::wvalue MySQLManager::GetLists(const std::string& id, const std::string& email)
{
	std::string query = "SELECT todoTitle, taskName, taskDescription, complete, dueDate FROM `User Data` WHERE gID=? AND userEmail=?";
	sql::PreparedStatement* statement = m_Connection->prepareStatement(query);
	statement->setString(1, id);
	statement->setString(2, email);

	sql::ResultSet* res = statement->executeQuery();
	std::string title{ "" };

	crow::json::wvalue jsonData;
	crow::json::wvalue::list titlesArray;
	crow::json::wvalue::list taskArray;

	while (res->next())
	{
		std::string currentTitle = res->getString("todoTitle");
		std::string task = res->getString("taskName");
		bool complete = res->getBoolean("complete");
		std::string taskDesc = res->getString("taskDescription");
		std::string dueDate = res->getString("dueDate");

		if (title != currentTitle)
		{
			if (!title.empty())
			{
				jsonData["list"][title]["tasks"] = std::move(taskArray);
				titlesArray.push_back(title);
			}
			title = currentTitle;
			taskArray.clear();
		}

		if (!task.empty())
		{
			crow::json::wvalue taskObject;
			taskObject["taskName"] = task;
			taskObject["complete"] = complete;
			taskObject["taskDescription"] = taskDesc;
			taskObject["dueDate"] = dueDate;

			taskArray.push_back(std::move(taskObject));
		}

	}
	
	if (!title.empty())
	{
		jsonData["list"][title]["tasks"] = std::move(taskArray);
		titlesArray.push_back(title);
	}


	delete statement;
	delete res;

	return jsonData;
}

bool MySQLManager::DeleteList(const std::string& id,const std::string& email, const std::string& name)
{
	std::string query = "DELETE FROM TODOList "
						"WHERE name=? AND ownerID = (SELECT id FROM Users WHERE email=? AND gID=?)";

	sql::PreparedStatement* statement = m_Connection->prepareStatement(query);
	statement->setString(1, name);
	statement->setString(2, email);
	statement->setString(3, id);

	int affectedRows = statement->executeUpdate(); // TODO get delete confirmation


	if (affectedRows > 0)
		return true;

	delete statement;
	return false;
}

bool MySQLManager::DeleteTask(const std::string& id, const std::string& email, const std::string& list, const std::string& name)
{
	std::string query = "DELETE FROM Tasks "
		"WHERE name =? AND ownerID = (SELECT id FROM TODOList WHERE name =? AND TODOList.ownerID = (SELECT id FROM Users WHERE email=? AND gID=?))";

	sql::PreparedStatement* statement = m_Connection->prepareStatement(query);
	statement->setString(1, name);
	statement->setString(2, list);
	statement->setString(3, email);
	statement->setString(4, id);

	int affectedRows = statement->executeUpdate(); // TODO get delete confirmation

	if (affectedRows > 0)
		return true;

	delete statement;
	return false;
}

void MySQLManager::UpdateTaskComplete(const std::string& id, const std::string& email, const std::string& list, const std::string& name, const bool& complete)
{
	std::string query = "UPDATE Tasks SET complete=? "
		"WHERE name =? AND ownerID = (SELECT id FROM TODOList WHERE name =? AND TODOList.ownerID = (SELECT id FROM Users WHERE email =? AND gID=?))";

	sql::PreparedStatement* statement = m_Connection->prepareStatement(query);
	statement->setBoolean(1, complete);
	statement->setString(2, name);
	statement->setString(3, list);
	statement->setString(4, email);
	statement->setString(5, id);

	statement->execute();

	delete statement;
}

void MySQLManager::UpdateTaskDesc(const std::string& id, const std::string& email, const std::string& list, const std::string& name, const std::string& desc)
{
	std::string query = "UPDATE Tasks SET taskDesc=? "
		"WHERE name =? AND ownerID = (SELECT id FROM TODOList WHERE name =? AND TODOList.ownerID = (SELECT id FROM Users WHERE email =? AND gID=?))";

	sql::PreparedStatement* statement = m_Connection->prepareStatement(query);
	statement->setString(1, desc);
	statement->setString(2, name);
	statement->setString(3, list);
	statement->setString(4, email);
	statement->setString(5, id);

	std::cout << desc;

	statement->execute();

	delete statement;
}

void MySQLManager::UpdateTaskDate(const std::string& id, const std::string& email, const std::string& list, const std::string& name, const std::string& date)
{
	std::string query = "UPDATE Tasks SET dueDate=? "
		"WHERE name =? AND ownerID = (SELECT id FROM TODOList WHERE name =? AND TODOList.ownerID = (SELECT id FROM Users WHERE email =? AND gID=?))";

	sql::PreparedStatement* statement = m_Connection->prepareStatement(query);
	statement->setDateTime(1, date);
	statement->setString(2, name);
	statement->setString(3, list);
	statement->setString(4, email);
	statement->setString(5, id);

	statement->execute();

	delete statement;
}

bool MySQLManager::DoesUserExist(const std::string& id)
{
	bool result;
	std::string query = "SELECT EXISTS(SELECT 1 FROM Users WHERE gID=?);";
	sql::PreparedStatement* statement = m_Connection->prepareStatement(query);
	statement->setString(1, id);

	sql::ResultSet* res = statement->executeQuery();

	while (res->next())
	{
		result = res->getBoolean(1);
	}

	delete res;
	delete statement;

	return result;
}

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