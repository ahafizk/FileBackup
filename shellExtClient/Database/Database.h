#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <string>
#include <vector>
#include "sqlite3.h"

using namespace std;

class Database
{
public:
	Database(char* filename);
	~Database();
	
	bool connect(char* filename);
	vector<vector<string> > executeQuery(char* query);
	void close();
	
private:
	sqlite3 *database;
};

#endif
