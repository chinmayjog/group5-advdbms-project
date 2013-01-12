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

#include<iostream>
#include<limits>
#include<cmath>
#include<cstdlib>
#include<cstring>

using namespace std;

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
			/*memcpy(&dateString[0],&date,sizeof(short));
			memcpy(&dateString[2],&month,sizeof(short));
			memcpy(&dateString[4],&year,sizeof(int));*/

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
			/*memcpy(&timeString[0],&hours,sizeof(short));
			memcpy(&timeString[2],&minutes,sizeof(short));
			memcpy(&timeString[4],&seconds,sizeof(short));*/

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

		DateTime(short date,short month,int year,short hours,short minutes,short seconds)
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
			/*memcpy(&dateTimeString[0],&date,sizeof(short));
			memcpy(&dateTimeString[2],&month,sizeof(short));
			memcpy(&dateTimeString[4],&year,sizeof(int));
			memcpy(&dateTimeString[8],&hours,sizeof(short));
			memcpy(&dateTimeString[10],&minutes,sizeof(short));
			memcpy(&dateTimeString[12],&seconds,sizeof(short));*/

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

short retDataTypeID(char *dataType);
int dataCompare(char * data1,char * data2,short dataType);
short retDataTypeSize(short dataTypeID,short length);
int stringConversion(string String,char *charString);
string charArrayConversion(char *charString,int length);
string getDefaultValue(short dataTypeID,int charLength);
void fillDefaultValue(char *defValue,int valueLength);
int getDefaultValue(string defaultValue,char *defValue,int valueLength,short dataType);
int dataCompare(char * data1,string data2,short dataType);
