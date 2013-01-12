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

#include"DataTypes.h"
#include<climits>
#include<cfloat>

/*
#include<iostream>
#include<limits>
#include<cmath>

class Date
{
	private: int year; // 1000 to 9999
		 short month; // 1 to 12
		 short date; // 1 to 28/29/30/31

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

		void fillDate(char * buffer)
		{
			int tempYear = year;
			short tempMonth = month,tempDate = date;

			char firstDigit,secondDigit,thirdDigit,fourthDigit;

			firstDigit = '0'+(tempDate/10);
			tempDate = tempDate % 10;
			secondDigit = '0'+tempDate;
			memcpy(&buffer[0],&firstDigit,sizeof(char));
			memcpy(&buffer[1],&secondDigit,sizeof(char));

			firstDigit = '0'+(tempMonth/10);
			tempMonth = tempMonth % 10;
			secondDigit = '0'+tempMonth;
			memcpy(&buffer[2],&firstDigit,sizeof(char));
			memcpy(&buffer[3],&secondDigit,sizeof(char));

			firstDigit = '0'+(tempYear/1000);
			tempYear = tempYear % 1000;
			secondDigit = '0'+(tempYear/100);
			tempYear = tempYear % 100;
			thirdDigit = '0'+(tempYear/10);
			tempYear = tempYear % 10;
			fourthDigit = '0'+tempYear;
			memcpy(&buffer[4],&firstDigit,sizeof(char));
			memcpy(&buffer[5],&secondDigit,sizeof(char));
			memcpy(&buffer[6],&thirdDigit,sizeof(char));
			memcpy(&buffer[7],&fourthDigit,sizeof(char));
		}

		string toString()
		{
			char * dateString = new char [8];
			//memcpy(&dateString[0],&date,sizeof(short));
			//memcpy(&dateString[2],&month,sizeof(short));
			//memcpy(&dateString[4],&year,sizeof(int));

			fillDate(dateString);

			//string retDateString = string(dateString); // Check this tomo once
			string retDateString;
			for(int i=0;i<8;i++)
			{
				retDateString = retDateString+dateString[i];
				if(i == 1 || i == 3)
					retDateString = retDateString+":";
			}
			delete dateString;
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
			// Checks whether the date is valid or not and returns 1 if it is correct and -1 if not
			// Check for leap year
			// Check for other weird years....
			// Check Internet for the code....
			bool validYear = 0,validMonth = 0,validDate = 0;
			short noOfDays;

			if(year >= 1000 && year < 10000)
				validYear = 1;
			if(month > 0 && month < 13)
				validMonth = 1;
			if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
				noOfDays = 31;
			else if(month == 4 || month == 6 || month == 9 || month == 11)
				noOfDays = 30;
			else
			{
				if((year % 400 == 0) || (year % 100 != 0 && year % 4 == 0))
					noOfDays = 29;
				else
					noOfDays = 28;
			}
			if(date > 0 && date <= noOfDays)
				validDate = 1;

			if(validYear == 1 && validMonth == 1 && validDate == 1)
				return 1;
			else
				return -1;
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

		void fillTime(char * buffer)
		{
			short tempHours = hours,tempMinutes = minutes,tempSeconds = seconds;

			char firstDigit,secondDigit;

			firstDigit = '0'+(tempHours/10);
			tempHours = tempHours % 10;
			secondDigit = '0'+tempHours;
			memcpy(&buffer[0],&firstDigit,sizeof(char));
			memcpy(&buffer[1],&secondDigit,sizeof(char));

			firstDigit = '0'+(tempMinutes/10);
			tempMinutes = tempMinutes % 10;
			secondDigit = '0'+tempMinutes;
			memcpy(&buffer[2],&firstDigit,sizeof(char));
			memcpy(&buffer[3],&secondDigit,sizeof(char));

			firstDigit = '0'+(tempSeconds/10);
			tempSeconds = tempSeconds % 10;
			secondDigit = '0'+tempSeconds;
			memcpy(&buffer[4],&firstDigit,sizeof(char));
			memcpy(&buffer[5],&secondDigit,sizeof(char));
		}

		string toString()
		{
			char * timeString = new char [6];
			//memcpy(&timeString[0],&hours,sizeof(short));
			//memcpy(&timeString[2],&minutes,sizeof(short));
			//memcpy(&timeString[4],&seconds,sizeof(short));

			fillTime(timeString);

			// string retTimeString = string(timeString); // Check this tomo once
			string retTimeString;
			for(int i=0;i<6;i++)
			{
				retTimeString = retTimeString+timeString[i];
				if(i == 1 || i == 3)
					retTimeString = retTimeString+":";
			}
			delete timeString;
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

class DateTime
{
	private: int year; // 1000 to 9999
		 short month; // 1 to 12
		 short date; // 1 to 28/29/30/31
		 short hours; // 00 to 23
		 short minutes; // 00 to 59
		 short seconds; // 00 to 59

	public: DateTime()
		{
			year = 1970;
			month = 1;
			date = 1;
			// UTC
			hours = 00;
			minutes = 00;
			seconds = 00;
			// 12:00:00 AM or 00:00:00
		}

		DateTime(int year,short month,short date,short hours,short minutes,short seconds)
		{
			this->year = year;
			this->month = month;
			this->date = date;
			this->hours = hours;
			this->minutes = minutes;
			this->seconds = seconds;
		}

		DateTime(char *buffer)
		{
			memcpy(&date,&buffer[0],sizeof(short));
			memcpy(&month,&buffer[2],sizeof(short));
			memcpy(&year,&buffer[4],sizeof(int));
			memcpy(&hours,&buffer[8],sizeof(short));
			memcpy(&minutes,&buffer[10],sizeof(short));
			memcpy(&seconds,&buffer[12],sizeof(short));
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

		void fillDateTime(char * buffer)
		{
			int tempYear = year;
			short tempMonth = month,tempDate = date;
			short tempHours = hours,tempMinutes = minutes,tempSeconds = seconds;

			char firstDigit,secondDigit,thirdDigit,fourthDigit;

			firstDigit = '0'+(tempDate/10);
			tempDate = tempDate % 10;
			secondDigit = '0'+tempDate;
			memcpy(&buffer[0],&firstDigit,sizeof(char));
			memcpy(&buffer[1],&secondDigit,sizeof(char));

			firstDigit = '0'+(tempMonth/10);
			tempMonth = tempMonth % 10;
			secondDigit = '0'+tempMonth;
			memcpy(&buffer[2],&firstDigit,sizeof(char));
			memcpy(&buffer[3],&secondDigit,sizeof(char));

			firstDigit = '0'+(tempYear/1000);
			tempYear = tempYear % 1000;
			secondDigit = '0'+(tempYear/100);
			tempYear = tempYear % 100;
			thirdDigit = '0'+(tempYear/10);
			tempYear = tempYear % 10;
			fourthDigit = '0'+tempYear;
			memcpy(&buffer[4],&firstDigit,sizeof(char));
			memcpy(&buffer[5],&secondDigit,sizeof(char));
			memcpy(&buffer[6],&thirdDigit,sizeof(char));
			memcpy(&buffer[7],&fourthDigit,sizeof(char));

			firstDigit = '0'+(tempHours/10);
			tempHours = tempHours % 10;
			secondDigit = '0'+tempHours;
			memcpy(&buffer[8],&firstDigit,sizeof(char));
			memcpy(&buffer[9],&secondDigit,sizeof(char));

			firstDigit = '0'+(tempMinutes/10);
			tempMinutes = tempMinutes % 10;
			secondDigit = '0'+tempMinutes;
			memcpy(&buffer[10],&firstDigit,sizeof(char));
			memcpy(&buffer[11],&secondDigit,sizeof(char));

			firstDigit = '0'+(tempSeconds/10);
			tempSeconds = tempSeconds % 10;
			secondDigit = '0'+tempSeconds;
			memcpy(&buffer[12],&firstDigit,sizeof(char));
			memcpy(&buffer[13],&secondDigit,sizeof(char));
		}

		string toString()
		{
			char * dateTimeString = new char [14];
			//memcpy(&dateTimeString[0],&date,sizeof(short));
			//memcpy(&dateTimeString[2],&month,sizeof(short));
			//memcpy(&dateTimeString[4],&year,sizeof(int));
			//memcpy(&dateTimeString[8],&hours,sizeof(short));
			//memcpy(&dateTimeString[10],&minutes,sizeof(short));
			//memcpy(&dateTimeString[12],&seconds,sizeof(short));

			fillDateTime(dateTimeString);

			// string retTimeString = string(timeString); // Check this tomo once
			string retDateTimeString;
			for(int i=0;i<14;i++)
			{
				retDateTimeString = retDateTimeString+dateTimeString[i];
				if(i == 1 || i == 3 || i == 7 || i == 9 || i == 11)
					retDateTimeString = retDateTimeString+":";
			}
			delete dateTimeString;
			return retDateTimeString;
		}

		int compare(DateTime dt2)
		{
			if(this->year < dt2.year)
				return -1;
			if(this->year > dt2.year)
				return 1;
			// Years are equal.... Check for month.....
			if(this->month < dt2.month)
				return -1;
			if(this->month > dt2.month)
				return 1;
			// Months are equal..... Check for date.....
			if(this->date < dt2.date)
				return -1;
			if(this->date > dt2.date)
				return 1;
			if(this->hours < dt2.hours)
				return -1;
			if(this->hours > dt2.hours)
				return 1;
			// Hours are equal.... Check for minutes.....
			if(this->minutes < dt2.minutes)
				return -1;
			if(this->minutes > dt2.minutes)
				return 1;
			// Minutes are equal..... Check for seconds.....
			if(this->seconds < dt2.seconds)
				return -1;
			if(this->seconds > dt2.seconds)
				return 1;
			// The dates and times are equal.....
			return 0;
		}

		int validateDateTime()
		{
			bool validHours=0,validMinutes=0,validSeconds=0;
			bool validYear = 0,validMonth = 0,validDate = 0;
			short noOfDays;

			if(year >= 1000 && year < 10000)
				validYear = 1;
			if(month > 0 && month < 13)
				validMonth = 1;
			if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
				noOfDays = 31;
			else if(month == 4 || month == 6 || month == 9 || month == 11)
				noOfDays = 30;
			else
			{
				if((year % 400 == 0) || (year % 100 != 0 && year % 4 == 0))
					noOfDays = 29;
				else
					noOfDays = 28;
			}
			if(date > 0 && date <= noOfDays)
				validDate = 1;

			
			if(hours >= 0 && hours < 24)
				validHours = 1;
			if(minutes >= 0 && minutes < 60)
				validMinutes = 1;
			if(seconds >= 0 && seconds < 60)
				validSeconds = 1;

			if(validYear == 1 && validMonth == 1 && validDate == 1 && validHours == 1 && validMinutes == 1 && validSeconds == 1)
				return 1;
			else
				return -1;
		}

		void fillBuffer(char *buffer)
		{
			memcpy(&buffer[0],&date,sizeof(short));
			memcpy(&buffer[2],&month,sizeof(short));
			memcpy(&buffer[4],&year,sizeof(int));
			memcpy(&buffer[8],&hours,sizeof(short));
			memcpy(&buffer[10],&minutes,sizeof(short));
			memcpy(&buffer[12],&seconds,sizeof(short));
		}
};
*/

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

	int dCount = 0;

	for(dCount=0;dCount<15;dCount++)
	{
		if(strncmp(dataType,dataTypes[dCount].c_str(),8)==0)
			return dCount+1;
	}
	return -1;
}

short retDataTypeSize(short dataTypeID,short length)
{
	switch(dataTypeID)
	{
		case 1: return sizeof(int);
			break;
		case 2: return sizeof(float);
			break;
		case 3: return sizeof(char)*length;
			break;
		case 4: return sizeof(double);
			break;
		case 5: return sizeof(char)*length;
			break;
		case 6: return sizeof(short);
			break;
		case 7: return sizeof(long);
			break;
		case 8: return sizeof(unsigned int);
			break;
		case 9: return sizeof(unsigned short);
			break;
		case 10: return sizeof(unsigned long);
			 break;
		case 11: return 8*sizeof(char);
			 break;
		case 12: return 6*sizeof(char);
			 break;
		case 13: return 14*sizeof(char);
			 break;
		default: return -1;
			 break;
	}
}

int stringConversion(string String,char *charString)
{
	int length = String.length();

	for(int i = 0;i<length;i++)
	{
		charString[i] = String[i];
	}
	return length;
}

string charArrayConversion(char *charString,int length)
{
	int i;
	string retString;
	for(i=0;i<length;i++)
	{
		retString = retString+charString[i];
	}
	return retString;
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
		/*if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;*/

		// This is adapted from Knuth's book as per discussion in Stack Overflow - http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison
		float epsilon = numeric_limits<float>::epsilon();
		if((fabs(d1 - d2) <= ((fabs(d1) < fabs(d2) ? fabs(d2) : fabs(d1)) * epsilon)) == 1)
			return 0;
		else if((d1 - d2) > ((fabs(d1) < fabs(d2) ? fabs(d2) : fabs(d1)) * epsilon))
			return 1;
		else
			return -1;
	}
	else if(dataType == 3)
	{
		int shortestLength;
		string d1 = data1,d2 = data2;
		if(d1.length()<d2.length())
			shortestLength = d1.length();
		else
			shortestLength = d2.length();
		return strncmp(data1,data2,shortestLength);
	}
	else if(dataType == 4)
	{
		double d1,d2;
		memcpy(&d1,data1,sizeof(double));
		memcpy(&d2,data2,sizeof(double));
		// This will change as we have to check how we can compare floats and doubles from Press or 
		// other reliable Internet sources.....
		/*if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;*/

		// This is adapted from Knuth's book as per discussion in Stack Overflow - http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison
		double epsilon = numeric_limits<double>::epsilon();
		if((fabs(d1 - d2) <= ((fabs(d1) < fabs(d2) ? fabs(d2) : fabs(d1)) * epsilon)) == 1)
			return 0;
		else if((d1 - d2) > ((fabs(d1) < fabs(d2) ? fabs(d2) : fabs(d1)) * epsilon))
			return 1;
		else
			return -1;
	}
	else if(dataType == 5)
	{
		//return strcmp(data1,data2);
		int shortestLength;
		string d1 = data1,d2 = data2;
		if(d1.length()<d2.length())
			shortestLength = d1.length();
		else
			shortestLength = d2.length();
		return strncmp(data1,data2,shortestLength);
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
		Date d1(data1),d2(data2);
		return d1.compare(d2);
	}
	else if(dataType == 12)
	{
		Time d1(data1),d2(data2);
		return d1.compare(d2);
	}
	else if(dataType == 13)
	{
		DateTime d1(data1),d2(data2);
		return d1.compare(d2);
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

int dataCompare(char * data1,string data2,short dataType)
{
	if(dataType == 1)
	{
		int d1,d2;
		memcpy(&d1,data1,sizeof(int));
		//memcpy(&d2,data2,sizeof(int));
		d2 = atoi(data2.c_str());
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
		//memcpy(&d2,data2,sizeof(float));
		d2 = atof(data2.c_str());
		// This will change as we have to check how we can compare floats and doubles from Press or 
		// other reliable Internet sources.....
		/*if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;*/

		// This is adapted from Knuth's book as per discussion in Stack Overflow - http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison
		float epsilon = numeric_limits<float>::epsilon();
		if((fabs(d1 - d2) <= ((fabs(d1) < fabs(d2) ? fabs(d2) : fabs(d1)) * epsilon)) == 1)
			return 0;
		else if((d1 - d2) > ((fabs(d1) < fabs(d2) ? fabs(d2) : fabs(d1)) * epsilon))
			return 1;
		else
			return -1;
	}
	else if(dataType == 3)
	{
		int shortestLength;
		short bigger;
		string d1 = data1,d2 = data2;
		if(d1.length()<d2.length())
		{
			shortestLength = d1.length();
			bigger = 2;
		}
		else
		{
			shortestLength = d2.length();
			bigger = 1;
		}
		for(int i = 0;i<shortestLength;i++)
		{	
			if(d1[i] == d2[i])
			{
				if(d1.length() == d2.length())
					return 0;
				continue;
			}
			if(d1[i] > d2[i])
				return 1;
			else
				return -1;
		}
		//return strncmp(data1,data2,shortestLength);
	}
	else if(dataType == 4)
	{
		double d1,d2;
		memcpy(&d1,data1,sizeof(double));
		//memcpy(&d2,data2,sizeof(double));
		d2 = atof(data2.c_str());
		// This will change as we have to check how we can compare floats and doubles from Press or 
		// other reliable Internet sources.....
		/*if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;*/

		// This is adapted from Knuth's book as per discussion in Stack Overflow - http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison
		double epsilon = numeric_limits<double>::epsilon();
		if((fabs(d1 - d2) <= ((fabs(d1) < fabs(d2) ? fabs(d2) : fabs(d1)) * epsilon)) == 1)
			return 0;
		else if((d1 - d2) > ((fabs(d1) < fabs(d2) ? fabs(d2) : fabs(d1)) * epsilon))
			return 1;
		else
			return -1;
	}
	else if(dataType == 5)
	{
		//return strcmp(data1,data2);
		int shortestLength;
		short bigger;
		string d1 = data1,d2 = data2;
		if(d1.length()<d2.length())
		{
			shortestLength = d1.length();
			bigger = 2;
		}
		else
		{
			shortestLength = d2.length();
			bigger = 1;
		}
		for(int i = 0;i<shortestLength;i++)
		{	
			if(d1[i] == d2[i])
			{
				if(d1.length() == d2.length())
					return 0;
				continue;
			}
			if(d1[i] > d2[i])
				return 1;
			else
				return -1;
		}
		//return strncmp(data1,data2,shortestLength);
	}
	else if(dataType == 6)
	{
		short d1,d2;
		memcpy(&d1,data1,sizeof(short));
		//memcpy(&d2,data2,sizeof(short));
		d2 = atoi(data2.c_str());
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
		//memcpy(&d2,data2,sizeof(long));
		d2 = atol(data2.c_str());
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
		//memcpy(&d2,data2,sizeof(unsigned int));
		d2 = atoi(data2.c_str());
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
		//memcpy(&d2,data2,sizeof(unsigned short));
		d2 = atoi(data2.c_str());
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
		//memcpy(&d2,data2,sizeof(unsigned long));
		d2 = atol(data2.c_str());
		if(d1 == d2)
			return 0;
		else if(d1 - d2 < 0)
			return -1;
		else
			return 1;
	}
	else if(dataType == 11)
	{
		Date d1(data1);
		char * dateString = new char [8];
		d1.fillDate(dateString);
		for(int i=0;i<8;i++)
		{
			if(dateString[i] == data2[i])
			{
				if(i == 7)
					return 0;
				continue;
			}
			if(dateString[i] < data2[i])
				return -1;
			else
				return 1;
		}
		//return d1.compare(d2);
	}
	else if(dataType == 12)
	{
		Time d1(data1);
		char * timeString = new char [6];
		d1.fillTime(timeString);
		for(int i=0;i<6;i++)
		{
			if(timeString[i] == data2[i])
			{
				if(i == 5)
					return 0;
				continue;
			}
			if(timeString[i] < data2[i])
				return -1;
			else
				return 1;
		}
		//return d1.compare(d2);
	}
	else if(dataType == 13)
	{
		DateTime d1(data1);
		char * dateTimeString = new char [14];
		d1.fillDateTime(dateTimeString);
		for(int i=0;i<14;i++)
		{
			if(dateTimeString[i] == data2[i])
			{
				if(i == 13)
					return 0;
				continue;
			}
			if(dateTimeString[i] < data2[i])
				return -1;
			else
				return 1;
		}
		//return d1.compare(d2);
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

int dataCheck(char * data1,int length,short dataType)
{
	if(dataType == 1)
	{
		int d1;
		memcpy(&d1,data1,sizeof(int));
		if(d1 > INT_MIN && d1 < INT_MAX)
			return 1;
		else
			return -1;
	}
	else if(dataType == 2)
	{
		float d1;
		memcpy(&d1,data1,sizeof(float));
		if(d1 > FLT_MIN && d1 < FLT_MAX)
			return 1;
		else
			return -1;
	}
	else if(dataType == 3)
	{
		for(int i=0;i<length;i++)
		{
			if(data1[i] < CHAR_MIN || data1[i] > CHAR_MAX)
				return -1;
		}
		return 1;
	}
	else if(dataType == 4)
	{
		double d1;
		memcpy(&d1,data1,sizeof(double));
		if(d1 > DBL_MIN && d1 < DBL_MAX)
			return 1;
		else
			return -1;
	}
	else if(dataType == 5)
	{
		for(int i=0;i<length;i++)
		{
			if(data1[i] < CHAR_MIN || data1[i] > CHAR_MAX)
				return -1;
		}
		return 1;
	}
	else if(dataType == 6)
	{
		short d1;
		memcpy(&d1,data1,sizeof(short));
		if(d1 > SHRT_MIN && d1 < SHRT_MAX)
			return 1;
		else
			return -1;
	}
	else if(dataType == 7)
	{
		long d1;
		memcpy(&d1,data1,sizeof(long));
		if(d1 > LONG_MIN && d1 < LONG_MAX)
			return 1;
		else
			return -1;
	}
	else if(dataType == 8)
	{
		unsigned int d1;
		memcpy(&d1,data1,sizeof(unsigned int));
		if(d1 > -1 && d1 < UINT_MAX)
			return 1;
		else
			return -1;
	}
	else if(dataType == 9)
	{
		unsigned short d1;
		memcpy(&d1,data1,sizeof(unsigned short));
		if(d1 > -1 && d1 < USHRT_MAX)
			return 1;
		else
			return -1;
	}
	else if(dataType == 10)
	{
		unsigned long d1;
		memcpy(&d1,data1,sizeof(unsigned long));
		if(d1 > -1 && d1 < ULONG_MAX)
			return 1;
		else
			return -1;
	}
	else if(dataType == 11)
	{
		Date d1(data1);
		return d1.validateDate();
	}
	else if(dataType == 12)
	{
		Time d1(data1);
		return d1.validateTime();
	}
	else if(dataType == 13)
	{
		DateTime d1(data1);
		return d1.validateDateTime();
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

string getDefaultValue(short dataTypeID,int charLength)
{
	int defInt = 0;
	float defFloat = 0.0;
	char defChar = '\0';
	double defDouble = 0.0;
	short defShort = 0;
	long defLong = 0L;
	unsigned int defUInt = 0;
	unsigned short defUShort = 0;
	unsigned long defULong = 0L;
	Date defDate;
	Time defTime;
	DateTime defDateTime;
	string retString;
	char retChar[256];
	int i;
	for(i=0;i<256;i++)
		retChar[i] = '$';
	char dateBuffer[8];
	char timeBuffer[6];
	char dateTimeBuffer[14];
	switch(dataTypeID)
	{
		case 1: memcpy(retChar,&defInt,sizeof(int));
			break;
		case 2: memcpy(retChar,&defFloat,sizeof(float));
			break;
		case 3: for(i=0;i<charLength;i++)
				retChar[i] = defChar;
			break;
		case 4: memcpy(retChar,&defDouble,sizeof(int));
			break;
		case 5: for(i=0;i<charLength;i++)
				retChar[i] = defChar;
			break;
		case 6: memcpy(retChar,&defShort,sizeof(short));
			break;
		case 7: memcpy(retChar,&defLong,sizeof(long));
			break;
		case 8: memcpy(retChar,&defUInt,sizeof(unsigned int));
			break;
		case 9: memcpy(retChar,&defUShort,sizeof(unsigned short));
			break;
		case 10: memcpy(retChar,&defULong,sizeof(unsigned long));
			 break;
		case 11: defDate.fillBuffer(dateBuffer);
			 for(i=0;i<8;i++)
				retChar[i] = dateBuffer[i];
			 break;
		case 12: defTime.fillBuffer(timeBuffer);
			 for(i=0;i<6;i++)
				retChar[i] = timeBuffer[i];
			 break;
		case 13: defDateTime.fillBuffer(dateTimeBuffer);
			 for(i=0;i<14;i++)
				retChar[i] = dateTimeBuffer[i];
			 break;
		case 14: // Not implemented
			 break;
		case 15: // Not implemented
			 break;
	}

	for(i=0;i<256;i++)
		retString = retString+retChar[i];
	return retString;
}

void fillDefaultValue(char *defValue,int valueLength)
{
	//string retString;
	char retChar[256];
	int i;
	for(i=0;i<256;i++)
		retChar[i] = '$';
	memcpy(retChar,defValue,valueLength);
	//return retString;
}

int getDefaultValue(string defaultValue,char *defValue,int valueLength,short dataType)
{
	int retValueLength = 0,i;
	for(i=0;i<valueLength;i++)
	{
		if(defaultValue[i] == '$')
			break;
		defValue[i] = defaultValue[i];
		retValueLength++;
	}
	return retValueLength;
}
