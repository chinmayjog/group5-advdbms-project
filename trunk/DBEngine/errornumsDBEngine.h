// errornumsDBEngine.h - Error numbers for the errors returned by the function

#define DBNAMEUNIQUEERROR -2001
//-2001 = "Database Creation: The database name must be unique. The database name you have specified already exists."

#define TABLENAMEUNIQUEERROR -2002
//-2002 = "Table Creation: The table name must be unique. The table name you have specified already exists."

#define COLUMNNAMEUNIQUEERROR -2003
//-2003 = "Alter/Create Table: The column names must be unique."

#define FREEPAGEERROR -2004
//-2004 = "Get Free Page: Unable to get a free page."

#define DIRPAGECREATIONERROR -2005
//-2005 = "Create Table: Unable to create the Directory Page."

#define UNABLETOOPENFOLDERERROR -2006
//-2006 = "Unable to open the data directory."

#define DATABASEFILECREATEERROR -2007
//-2007 = "Create table: Unable to create the database file."

#define DATABASEFILEOPENERRROR -2008
//-2008 = "Unable to open the database file."

#define DATABASEWRITEDBERROR -2009
//-2009 = "Writing to DB failed"

#define CACHEPAGESIZEERROR -2010
//-2010 = "The page size of the database and the cache do not match. Please change the page size from the cache panel and then try again."

#define DATABASENOTFOUNDERROR -2011
//-2011 = "Use Database: Database not found."

#define DATABASEREADDBERROR -2012
//-2012 = "Reading from DB failed"

#define DROPDBDATABASENOTFOUNDERROR -2013
//-2013 = "Drop Database: Database not found."

#define COMMITCACHEERROR -2014
//-2014 = "Commit Cache Error"

#define TABLENOTFOUNDERROR -2015
//-2015 = "Table Not Found Error"

#define COLUMNNOTFOUNDERROR -2016
//-2016 = "Column Not Found Error"

#define INDEXNOTFOUNDERROR -2017
//-2017 = "Index Not Found Error"

#define EXTENDFREESPACEERROR -2018
//-2018 = "Extend Free Space: Free Space not available. Additional storage could not be allocated."
