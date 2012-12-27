// DataTypes.cpp
// This file contains details on conversion of data.....

/*
Data Types
BIGINT long
UBIG unsigned long
INT int
UINT unsigned int
SMALLINT short
USMALL unsigned short
FLOAT float
DOUBLE double
CHAR fixed-length char
VARCHAR variable-length char (length+char)
DATE
TIME
DATETIME

// Not implemented as of now....
DECFLOAT
DATETIME+/-TIMEZONE
*/

// This file will have all functions needed for comparison.....
// For double, I think we should use difference between nos being less than some constant
// Or square error, being less than some constant... Check Numerical Recipes or some other Internet reference
// on how to compare floats, longs and doubles...... May be even large ints......

/*
Data Types            Associated Short IDs
INT                   1
FLOAT                 2
CHAR                  3
DOUBLE                4
VARCHAR               5
SMALLINT              6
BIGINT                7
UINT                  8
USMALL                9
UBIG                  10
DATE                  11
TIME                  12
DATETIME              13

DECFLOAT              14
DATETIME+/-TZ         15
*/

class Date
{
	private: int year;
		 short month;
		 short date;

	public: Date()
		{
			year = 1970;
			month = 1;
			date = 1;
			// UTC
		}

		Date(short date,short month,int year)
		{
			this->date = date;
			this->month = month;
			this->year = year;
		}

		Date(char *buffer)
		{
			memcpy(&date,&buffer[0],sizeof(short));
			memcpy(&month,&buffer[2],sizeof(short));
			memcpy(&year,&buffer[4],sizeof(int));
		}

		int getYear()
		{
			return year;
		}

		void setYear(int year)
		{
			this->year = year;
		}

		short getMonth()
		{
			return month;
		}

		void setMonth(short month)
		{
			this->month = month;
		}

		short getDate()
		{
			return date;
		}

		void setDate(short date)
		{
			this->date = date;
		}

		string toString()
		{
			char * dateString = new char [8];
			memcpy(&dateString[0],&date,sizeof(short));
			memcpy(&dateString[2],&month,sizeof(short));
			memcpy(&dateString[4],&year,sizeof(int));
			//string retDateString = string(dateString); // Check this tomo once
			string retDateString;
			for(int i=0;i<8;i++)
			{
				retDateString = retDateString+dateString[i];
				if(i == 1 || i == 3)
					retDateString = retDateString+":";
			}
			return retDateString;
		}

		int compare(Date d2)
		{
			if(this->year < d2.year)
				return -1;
			if(this->year > d2.year)
				return 1;
			// Years are equal.... Check for month.....
			if(this->month < d2.month)
				return -1;
			if(this->month > d2.month)
				return 1;
			// Months are equal..... Check for date.....
			if(this->date < d2.date)
				return -1;
			if(this->date > d2.date)
				return 1;
			// The dates are equal.....
			return 0;
		}

		int validateDate()
		{
			// Checks whether the date is valid or not and returns 0 if it is correct and -1 if not
			// Check for leap year
			// Check for other weird years....
			// Check Internet for the code....
		}

		void fillBuffer(char *buffer)
		{
			memcpy(&buffer[0],&date,sizeof(short));
			memcpy(&buffer[2],&month,sizeof(short));
			memcpy(&buffer[4],&year,sizeof(int));
		}
};

class Time
{
	private: short hours; // 00 to 23
		 short minutes; // 00 to 59
		 short seconds; // 00 to 59

	public: Time()
		{
			hours = 00;
			minutes = 00;
			seconds = 00;
			// 12:00:00 AM or 00:00:00
		}

		Time(short hours,short minutes,short seconds)
		{
			this->hours = hours;
			this->minutes = minutes;
			this->seconds = seconds;
		}

		Time(char *buffer)
		{
			memcpy(&hours,&buffer[0],sizeof(short));
			memcpy(&minutes,&buffer[2],sizeof(short));
			memcpy(&seconds,&buffer[4],sizeof(short));
		}

		short getHours()
		{
			return hours;
		}

		void setHours(short hours)
		{
			this->hours = hours;
		}

		short getMinutes()
		{
			return minutes;
		}

		void setMinutes(short minutes)
		{
			this->minutes = minutes;
		}

		short getSeconds()
		{
			return seconds;
		}

		void setSeconds(short seconds)
		{
			this->seconds = seconds;
		}

		string toString()
		{
			char * timeString = new char [6];
			memcpy(&timeString[0],&hours,sizeof(short));
			memcpy(&timeString[2],&minutes,sizeof(short));
			memcpy(&timeString[4],&seconds,sizeof(short));
			// string retTimeString = string(timeString); // Check this tomo once
			string retTimeString;
			for(int i=0;i<6;i++)
			{
				retTimeString = retTimeString+timeString[i];
				if(i == 1 || i == 3)
					retTimeString = retTimeString+":";
			}
			return retTimeString;
		}

		int compare(Time t2)
		{
			if(this->hours < t2.hours)
				return -1;
			if(this->hours > t2.hours)
				return 1;
			// Hours are equal.... Check for minutes.....
			if(this->minutes < t2.minutes)
				return -1;
			if(this->minutes > t2.minutes)
				return 1;
			// Minutes are equal..... Check for seconds.....
			if(this->seconds < t2.seconds)
				return -1;
			if(this->seconds > t2.seconds)
				return 1;
			// The times are equal.....
			return 0;
		}

		int validateTime()
		{
			bool validHours=0,validMinutes=0,validSeconds=0;
			if(hours >= 0 && hours < 24)
				validHours = 1;
			if(minutes >= 0 && minutes < 60)
				validMinutes = 1;
			if(seconds >= 0 && seconds < 60)
				validSeconds = 1;

			if(validHours == 1 && validMinutes == 1 && validSeconds == 1)
				return 1;
			else
				return -1;
		}

		void fillBuffer(char *buffer)
		{
			memcpy(&buffer[0],&hours,sizeof(short));
			memcpy(&buffer[2],&minutes,sizeof(short));
			memcpy(&buffer[4],&seconds,sizeof(short));
		}
};

short retDataTypeID(char *dataType)
{
	string dataTypes[15];
	dataTypes[0] = "INT$$$$$";
	dataTypes[1] = "FLOAT$$$";
	dataTypes[2] = "CHAR$$$$";
	dataTypes[3] = "DOUBLE$$";
	dataTypes[4] = "VARCHAR$";
	dataTypes[5] = "SMALLINT";
	dataTypes[6] = "BIGINT$$";
	dataTypes[7] = "UINT$$$$";
	dataTypes[8] = "USMALL$$";
	dataTypes[9] = "UBIG$$$$";
	dataTypes[10] = "DATE$$$$";
	dataTypes[11] = "TIME$$$$";
	dataTypes[12] = "DATETIME";
	dataTypes[13] = "DECFLOAT";
	dataTypes[14] = "DTIMETZ$";

	for(i=0;i<15;i++)
	{
		if(strncmp(dataType,dataTypes[i].c_str(),8)==0)
			return i+1;
		else
			return -1;
	}
}

int dataCompare(char * data1,char * data2,short dataType)
{
	if(dataType == 1)
	{
		int d1,d2;
		memcpy(&d1,data1,sizeof(int));
		memcpy(&d2,data2,sizeof(int));
		if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;
	}
	else if(dataType == 2)
	{
		float d1,d2;
		memcpy(&d1,data1,sizeof(float));
		memcpy(&d2,data2,sizeof(float));
		// This will change as we have to check how we can compare floats and doubles from Press or 
		// other reliable Internet sources.....
		if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;
	}
	else if(dataType == 3)
	{
		return strcmp(data1,data2);
	}
	else if(dataType == 4)
	{
		double d1,d2;
		memcpy(&d1,data1,sizeof(double));
		memcpy(&d2,data2,sizeof(double));
		// This will change as we have to check how we can compare floats and doubles from Press or 
		// other reliable Internet sources.....
		if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;
	}
	else if(dataType == 5)
	{
		return strcmp(data1,data2);
	}
	else if(dataType == 6)
	{
		short d1,d2;
		memcpy(&d1,data1,sizeof(short));
		memcpy(&d2,data2,sizeof(short));
		if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;
	}
	else if(dataType == 7)
	{
		long d1,d2;
		memcpy(&d1,data1,sizeof(long));
		memcpy(&d2,data2,sizeof(long));
		if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;
	}
	else if(dataType == 8)
	{
		unsigned int d1,d2;
		memcpy(&d1,data1,sizeof(unsigned int));
		memcpy(&d2,data2,sizeof(unsigned int));
		if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;
	}
	else if(dataType == 9)
	{
		unsigned short d1,d2;
		memcpy(&d1,data1,sizeof(unsigned short));
		memcpy(&d2,data2,sizeof(unsigned short));
		if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;
	}
	else if(dataType == 10)
	{
		unsigned long d1,d2;
		memcpy(&d1,data1,sizeof(unsigned long));
		memcpy(&d2,data2,sizeof(unsigned long));
		if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;
	}
	else if(dataType == 11)
	{
		Date d1(char *data1),d2(char *data2);
		return d1.compare(d2);
	}
	else if(dataType == 12)
	{
		Time d1(char *data1),d2(char *data2);
		return d1.compare(d2);
	}
	else if(dataType == 13)
	{
		// Not implemented yet.....
		return -20;
	}
	else if(dataType == 14)
	{
		// Not implemented yet.....
		return -20;
	}
	else if(dataType == 15)
	{
		// Not implemented yet.....
		return -20;
	}
}
