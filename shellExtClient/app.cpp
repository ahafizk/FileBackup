
#include "stdafx.h"
#include "Database/Database.h"

#include "Database/Database.h"
#include <iostream>
#include "string"
#include "vector"
#include <fstream>
#include <sstream>
#include <stdlib.h>
using namespace std;



BOOL check_directory(CString path)
{
	BOOL ret = FALSE;
	DWORD dwAttr = GetFileAttributes(path);
	if(dwAttr & FILE_ATTRIBUTE_DIRECTORY)
	{
		ret = TRUE;
	}
	return ret;
}

int stringToInt(string str)
{
	int value;
	stringstream buffer(str);
    
	buffer >> value; 
	return value; //return the string
}
void gen_random(char *s, const int len=10) 
{
	char ch;
	static const char alphanum[] ="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	for (int i = 0; i < len; ++i) 
	{
		
		ch = alphanum[rand() % (sizeof(alphanum) - 1)];
		if(ch=='0' && i==0)
		{
			while((ch = alphanum[rand() % (sizeof(alphanum) - 1)])=='0');
		}
		s[i] = ch;
	}
	
	s[len] = '\0';
}

void createDB()
{
	Database *db;
	FILE *fp;
	int wr = -1;
	char buf[4];
	if((fp = fopen("C:\\Program Files\\Data Lock It\\db.txt","r")))
	{
		//fp = fopen("db.txt","w");
		//fclose(fp);
		//wr = -1;
		fgets(buf,2,fp);
		fclose(fp);
		wr = (int)buf[0];
	}
	else
	{
		fp = fopen("C:\\Program Files\\Data Lock It\\db.txt","w");
		fclose(fp);
		wr = -1;
		
	}
	
	
	fp = fopen("C:\\Program Files\\Data Lock It\\db.txt","r+");
		
	
	
	if(wr<=0)
	{
		db = new Database("C:\\Program Files\\Data Lock It\\Temp_Database.db");
		db->executeQuery("CREATE TABLE if not exists paths ( id INTEGER  PRIMARY KEY AUTOINCREMENT, name varchar(266) NOT NULL default '',hasSaved varchar(2) NOT NULL default 'N');");
		db->close();
		
		fprintf(fp,"1");//write 1 for next use
		fclose(fp);
				
	}
	
	
}


void saveFilesToDB(char * path)
{
	
	char sql[1000]; //contains the query
	Database *db;
	
	vector<vector<string> > results; //hold the query result;
	vector<vector<string> >::iterator it;
	vector<string> row;

	db = new Database("C:\\Program Files\\Data Lock It\\Temp_Database.db");

	sprintf(sql,"select count(id)from paths where name = '%s';",path);
	
	results = db->executeQuery(sql);
	int value=-1;
	
	if(!results.empty())
	{
		it = results.begin();
		row = *it;
		string str = row.at(0);
		value = stringToInt(str);
		//value = 1;
	}
	if(value<=0)
	{
		sprintf(sql,"insert into paths (name)values('%s');",path);
		db->executeQuery(sql);

	}
	
			
	db->close();
}


void SearchDirectory(LPCTSTR pszRootPath,CStringArray& arrFiles, bool bRecursive = true)
{
	CString strToFind;
	strToFind.Format(_T("%s\\*.*"), pszRootPath); //search for all files
	
	CFileFind finder;
	BOOL bFound = finder.FindFile(strToFind);
	while(bFound)
	{
		
		
		bFound = finder.FindNextFile();
		
		
		if(finder.IsDirectory() && !finder.IsDots()) //if got the directory
		{
			if(true == bRecursive)
			{
				CString strRootPath = finder.GetFilePath();
				SearchDirectory(strRootPath, arrFiles, bRecursive);
			}
		}
		else if(!finder.IsDots() && !finder.IsDirectory())//if it is an file add to arrFiles
		{
			CString strFilePath;
			
			strFilePath = finder.GetFilePath();
			
			arrFiles.Add(strFilePath);
			
		}
	}
	
}

void createCSV()
{
	Database *db;
	vector<vector<string> > results; //hold the query result;
	vector<vector<string> >::iterator it;
	vector<string> row;
	FILE *fp;
	int i=0;
	fp = fopen("dump_files.csv","w");
	db = new Database("Database.db");
	results = db->executeQuery("select *from files;");
	if(!results.empty())
	{
		for(vector<vector<string> >::iterator it = results.begin(); it < results.end(); ++it)
		{
			row = *it;
			//if(i==10)break;
			//i++;
			char *a = (char*)(row.at(0)).c_str();
			char *b = (char*)(row.at(1)).c_str();
			char *c = (char*)(row.at(2)).c_str();
			char *d = (char*)(row.at(3)).c_str();
			char *e = (char*)(row.at(4)).c_str();
			fprintf(fp,"%s,%s,%s,%s,%s\n",a,b,c,d,e);
			//fprintf(fp,"%s,%s,%s,%s,%s\n",row.at(0),row.at(1),row.at(2),row.at(3),row.at(4));
		}
	}
	fclose(fp);
	fp = fopen("dump_nodes.csv","w");
	results = db->executeQuery("select *from nodes;");
	if(!results.empty())
	{
		for(vector<vector<string> >::iterator it = results.begin(); it < results.end(); ++it)
		{
			row = *it;
			
			char *a = (char*)(row.at(0)).c_str();
			char *b = (char*)(row.at(1)).c_str();
			char *c = (char*)(row.at(2)).c_str();
			//char *d = (char*)(row.at(3)).c_str();
			//char *e = (char*)(row.at(4)).c_str();
			fprintf(fp,"%s,%s,%s\n",a,b,c);
			//fprintf(fp,"%s,%s,%s,%s,%s\n",row.at(0),row.at(1),row.at(2),row.at(3),row.at(4));
		}
	}
	fclose(fp);
	
}
void dumpCSV_TO_DB()
{
	Database *db;
	vector<vector<string> > results; //hold the query result;
	vector<vector<string> >::iterator it;
	vector<string> row;
	
	
	char sql[3048];

	
	db = new Database("Database.db");
	db->executeQuery("delete from files;");
	db->executeQuery("delete from nodes;");

	
	
	ifstream ifs( "dump_nodes.csv" ); //open nodes csv file to dump to db
	ifstream fls("dump_files.csv"); //open files csv file to dump to db
	
	
	string temp;
	BOOL isValid = FALSE;  //is query is valid
	char *temp1;

	//code goes for nodes table dump

	while( getline( ifs, temp ) )
	{
		
		char *temp1 = (char*)(temp.c_str());
		char *values;
		string sql1;
		
		isValid = FALSE;

		sql1 = "insert into nodes (node_id,parent_node_id,node_name)values(";

		values = strtok(temp1,",");
		
		while(values)
		{
			string ts(values);

			sql1 +="'"+ts+"'"; 
			values = strtok(NULL,",");
			if(values==NULL)
			{
				sql1 += ");";
				isValid = TRUE;
			}
			else 
			{
			   sql1 +=",";
			}
		}//end of while

		if(isValid==TRUE)
		{
			char *query = (char*)(sql1.c_str());
			db->executeQuery(query);
		}
			
	}


	while( getline( fls, temp ) )
	{
		
		temp1 = (char*)(temp.c_str());
		char *values;
		string sql1;
		
		isValid = FALSE;
		
		sql1 = "insert into files (file_code,node_id,file_name,file_size,modified_date)values(";
		
		values = strtok(temp1,",");
		
		while(values)
		{
			string ts(values);
			
			sql1 +="'"+ts+"'"; 
			values = strtok(NULL,",");
			if(values==NULL)
			{
				sql1 += ");";
				isValid = TRUE;
			}
			else 
			{
				sql1 +=",";
			}
		}//end of while
		
		if(isValid==TRUE)
		{
			char *query = (char*)(sql1.c_str());
			db->executeQuery(query);
		}
		
	}


	db->close();

}

int main(int argc, char *argv[])
{
	
	char *path ;//= "C:\\Program Files";
	char *finalPath;
	
	
	int len =0;
	
	
	
	
	//finalPath = new char[strlen(path)];
	
	if(argc>1)
	{
		len = strlen(argv[1]);
		path = new char[len];
		
		strcpy(path,argv[1]);
		finalPath = new char[len];
		
		if(len==3&&path[2]==34)
		{
			
			strncpy (finalPath,path,2);
			finalPath[2] ='\0';
			
		}
		else
			strcpy(finalPath,path);
		
		
		createDB();
		saveFilesToDB(finalPath);
		
	}
		
//	createCSV();
	//dumpCSV_TO_DB();
	/*
	ifstream ifs( "dump_nodes.csv" );
	
	char *temp1=new char[1024];
	string temp;
	int len1 = temp.length();
	while( getline( ifs, temp ) )
	{
		temp1 = (char*)(temp.c_str());
		cout<<temp1<<endl;
	}
	*/
//	int i;
	//cin>>i;
	return 0;
}