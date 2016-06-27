#include "Database.h"
#include <iostream>
using namespace std;

Database::Database(char* filename)
{
	database = NULL;
	connect(filename);
}

Database::~Database()
{
}

bool Database::connect(char* filename)
{
	if(sqlite3_open(filename, &database) == SQLITE_OK)
		return true;
	
	return false;   
}

vector<vector<string> > Database::executeQuery(char* query)
{
	sqlite3_stmt *statement;
	vector<vector<string> > results;
	
	if(sqlite3_prepare_v2(database, query, -1, &statement, 0) == SQLITE_OK)
	{
		int cols = sqlite3_column_count(statement);//return the total cols.
		int result = 0;
		while(true)
		{
			result = sqlite3_step(statement);//process per row
			
			if(result == SQLITE_ROW)
			{
				vector<string> values;
				for(int col = 0; col < cols; col++) //per row per col
				{
					values.push_back((char*)sqlite3_column_text(statement, col));
				}
				results.push_back(values);
			}
			else
			{
				break;   
			}
		}
		
		sqlite3_finalize(statement);
	}
	
	string error = sqlite3_errmsg(database);
	if(error != "not an error") cout << query << " " << error << endl;
	
	return results;  
}

void Database::close()
{
	sqlite3_close(database);   
}