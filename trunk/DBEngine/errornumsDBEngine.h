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

#define SYSTABLEENTRYCREATEERROR -2019
//-2019 = "Create Table: SysTable Entry create failure"

#define SYSCOLUMNENTRYCREATEERROR -2020
//-2020 = "Create Table: SysColumn Entry create failure"

#define SYSINDEXENTRYCREATEERROR -2021
//-2021 = "Create Table: SysIndex Entry create failure"

#define DROPDBERROR -2022
//-2022 = "Drop Database Error: Error in dropping the database"

#define INSERTTABLETBLNOTFOUNDERROR -2023
//-2023 = "Insert Table: Table not found"

#define INSERTTABLECOLNOTFOUNDERROR -2024
//-2024 = "Insert Table: Column not found"

#define INSERTTABLEDATANOTINSERTERROR -2025
//-2025 = "Insert Table: Unable to insert the data"

#define INSERTTABLEDATANULLINSERTERROR -2026
//-2026 = "Insert Table: NULL value insertion not possible"

#define INSERTTABLEDATABOUNDARYEXCEEDSERROR -2027
//-2027 = "Insert Table: Boundary value exceeded error"

#define SYSCOLUMNSAUTOINCREMENTERROR -2028
//-2028 = "Insert Table: Auto Increment incompatible with other data types"

#define EXTENDFREESPACEGETPAGECOUNTERROR -2029
//-2029 = "Extend free space: Get page count error"

#define INSERTDIRECTORYPAGEENTRY -2030
//-2030 = "Insert Entry: Create new Directory Page Entry Error"

#define CREATEDIRECTORYPAGEENTRY -2031
//-2031 = "Create Directory Entry: Create new Directory Page Failed"

#define DELETETABLETBLNOTFOUNDERROR -2032
//-2032 = "Delete Table: Table not found"

#define QUERYUNSUPPORTEDERROR -2033
//-2033 = "Query unsupported or is wrong"

#define QUERYERROR -2034
//-2034 = "Query is wrong. Multiple values has been specified"

#define QUERYLENGTHERROR -2035
//-2034 = "Query is wrong. Length of the specified string exceeds boundaries"
