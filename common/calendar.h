/************************************************************
 * calendar.h
 * Last modified by billjeff, June/28/2008
  
 * CL-PIM   
 * http://code.google.com/p/clpim/

 * This file contains the definition of class Calendar, which
 * provides time service to other plugins.

 * Revision History:
 * June/28/2008 - Finish the code of version 1.
 * July/7/2008 - Add SetTimeZone function.
 * July/10/2008 - Add TimeAdjust() function, add string Time()
 *   add string TimeWithoutTimeZone(), 
 ************************************************************/



#ifndef _CALENDAR_H_
#define _CALENDAR_H_

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>

using namespace std ;


namespace CLPIM
{
    class Calendar
    {
    private:
        int year, month, day ;
        // 0: Sunday, 1: Monday, 2: Tuesday, ...
        int week ;
        string weekStr[7] ;
        int hour, minute, second ;
        // d: d = PreTimeZone-NewTimeZone.
        int timeZone, d ;
        //bool timeZoneChanged ;
    protected:
        bool IsLeapYear( int year )
        {
            if ( (year%4 == 0 && year%100 != 0) || (year%400 == 0))
                return true ;
            return false ;
        }

        // Adjust week variable according to w. 
        void AdjustWeek( int w )
        {
            week += w ;
            if ( week < 0 )  week += 7 ;
            if ( week >= 7 ) week %= 7 ;
            return ;
        }

        void TimeAdjust()
        {
            // TimeZone is not changed.
            if ( d == 0 )  return ;

            bool leapYear = IsLeapYear(year) ;      
            hour += d ;
            if ( hour >= 24 )
            {
                if ( month == 2 )
                {
                    if ( (leapYear && day == 29) || (!leapYear && day == 28) )
                    {
                        day = 1 ;
                        month = 3 ;	    
                    }
                    else
                        day++ ;
                    hour -= 24 ;
                }
                else
                {
                    if ( month <= 7 )
                    {
                        if ( month%2 == 1 ) // 31 days
                        {
                            if ( day == 31 )
                            {
                                day = 1 ;
                                ++month ;
                            }
                            else
                                ++day ;
                        }
                        else // 30 days
                        {
                            if ( day == 30 )
                            {
                                day = 1 ;
                                ++month ;
                            }
                            else
                                ++day ;
                        }
                        hour -= 24 ;
                    } // month <= 7
                    else // month > 7
                    {
                        if ( month%2 == 0 ) // 31 days
                        {
                            if ( day == 31 )
                            {
                                day = 1 ;
                                ++month ;
                            }
                            else
                                ++day ;
                        } // 31 days
                        else // 30 days
                        {
                            if ( day == 30 )
                            {
                                ++month ;
                                day = 1 ;
                            }
                            else
                                ++day ;
                        } // 30 days
                        hour -= 24 ;
                        if ( month == 13 )
                        {
                            ++year ;
                            month = 1 ;
                        }
                    } // month > 7
                }
                ++week ;
                week %= 7 ;
            }
            else if ( hour < 0 )
            {
                hour += 24 ;
                --day ;
                if ( day == 0 )
                {
                    --month ;
                    if ( month == 0 )
                    {
                        --year ;
                        month = 12 ;
                        if ( year <= 0 )
                            cout << "Ooooops, year is less than zero. Orz" << endl ;
                    }
                    if ( month == 2 )
                    {
                        if ( leapYear )  day = 29 ;
                        else             day = 28 ;
                    } // month == 3 
                    else if ( month <= 7 )
                    {
                        if ( month%2 == 1 )  day = 31 ;
                        else                 day = 30 ;
                    } // month <= 7
                    else
                    {
                        if ( month%2 == 1 )  day = 30 ;
                        else                 day = 31 ;
                    } // month > 7
	  
                } // day == 0
                --week ;
                week = (week < 0 ? week+7 : week) ;
            } // hour < 0

            d = 0 ;
        }
    public:
        void UpdateTime()
        {
            time_t t ;
            time(&t) ;

            tm *p = gmtime(&t) ;
            year = 1900+p->tm_year ;
            month = 1+p->tm_mon ;
            day = p->tm_mday ;
            week = p->tm_wday ;

            hour = p->tm_hour ;
            if ( timeZone != 0 )
            {
                d = timeZone ;
                TimeAdjust() ;
            }

            minute = p->tm_min ;
            second = p->tm_sec ;
        }

        Calendar()
        {
            timeZone = 0 ;
            d = 0 ;
            //timeZoneChanged = false ;

            UpdateTime() ;
            weekStr[0] = "Sun" ;
            weekStr[1] = "Mon" ;
            weekStr[2] = "Tue" ;
            weekStr[3] = "Wed" ;
            weekStr[4] = "Thu" ;
            weekStr[5] = "Fri" ;
            weekStr[6] = "Sta" ;
        }

        void SetTimeZone( int z )
        {
            if ( z < -12 || z > 12 )
            {
                cerr << "Time Zone value error! Set its value to 0 instead." << endl ;
                timeZone = 0 ;
                return ;
            }

            //timeZoneChanged = true ;
            d = z-timeZone ;
            if ( d == 24 || d == -24 )  return ;
            timeZone = z ;
            TimeAdjust() ;
        }

        void SetYear( int y ) 
        {
            int s = min( y, year ), e = max( y, year ) ;
            int sum = 0 ;
            for ( int i = s ; i < e ; ++i ){
                if ((month >= 2 && IsLeapYear(i+1))
                    || (month < 2 && IsLeapYear(i)))  
                    sum += 366 ;
                else
                    sum += 365 ;
            }
      
            sum %= 7 ;
            if ( y > year ){
                week += sum ;
                week %= 7 ;
            }
            else {
                week -= sum ;
                if ( week < 0 )  week += 7 ;
            }      
            year = y ;
        }

        void SetMonth( int m )
        {
            if ( m <= 0 || m > 12 ){
                cout << "Oooops, month cannot be: " << m 
                     << ". Set it to 1(Jan) instead." << endl ;
                month = 1 ;
                return ;
            }
            // update week.
            {
                int s = min( m, month ), e = max( m, month ) ;
                int sum = 0 ;
                for ( int i = s ; i < e ; ++i ){
                    if ( i == 2 ){ // febrary
                        if ( IsLeapYear(year) )
                            sum += 29 ;
                        else
                            sum += 28 ;
                    }
                    else if ( i <= 7 ){
                        if ( i%2 == 0 )  sum += 30 ;
                        else             sum += 31 ;
                    }
                    else {
                        if ( i%2 == 0 )  sum += 31 ;
                        else             sum += 30 ;
                    }
                }
                sum %= 7 ;
                if ( m > month ) {
                    week += sum ;
                    week %= 7 ;
                }
                else {
                    week -= sum ;
                    if ( week < 0 )  week += 7 ;
                }
            }
            month = m ;
        }

        void SetDay( int d ) 
        {
            int min, max ;
            min = 0 ;
            if ( month == 2 ){
                if ( IsLeapYear(year) )  max = 29 ;
                else                     max = 28 ;
            }
            else if ( month < 8 ){
                if ( month % 2 == 1 )    max = 31 ;
                else                     max = 30 ;
            }
            else {
                if ( month % 2 == 0 )    max = 31 ;
                else                     max = 30 ;
            }

            if ( d <= min || d > max ){
                cout << "Oooops day cannot be: " << d 
                     << ". Set it to 1 instead." << endl ;
                week = week-(day-d) ;
                if ( week < 0 ) week += 7 ;
                day = 1 ;
                return ;
            }
            week += (d-day)%7 ;
            if ( week < 0 ) week += 7 ;
            day = d ;
        }

        void SetWeek( int w ) 
        {
            if ( w < 0 || w >= 7 ){
                cout << "Oooops, week cannot be: " << w 
                     << ". Set it to 1(Monday) instead." << endl ;
                week = 1 ;
                cout << "Notes: 0 for Sunday, 1 for Monday, and so on." << endl ;
                return ;
            }
            week = w ;
        }

        void SetHour( int h ) 
        {
            if ( h < 0 || h > 24 ){
                cout << "Oooops hour cannot be: " << h
                     << ". Set it to 0 instead." << endl ;
                hour = 0 ;
                return ;
            }
            hour = h ;
        }

        void SetMinute( int m ) 
        {
            if ( m < 0 || m > 60 ){
                cout << "Oooops minute cannot be: " << m
                     << ". Set it to 0 instead." << endl ;
                minute = 0 ;
                return ;
            }
            minute = m ;
        }

        void SetSecond( int s ) 
        {
            if ( s < 0 || s > 60 ){
                cout << "Oooops second cannot be: " << s
                     << ". Set it to 0 instead." << endl ;
                second = 0 ;
                return ;
            }
            second = s ;
        }

        // For the parameter of Get*** like functions, if it is true, then
        // call UpdateTime() before return the value or print the reuslt.
        int GetYear(bool f) 
        {
            if ( f ) UpdateTime() ;
            return year ;
        }

        int GetMonth(bool f) 
        {
            if ( f ) UpdateTime() ;
            return month ;
        }

        int GetDay(bool f) 
        {
            if ( f ) UpdateTime() ;
            return day ;
        }

        string GetWeek(bool f) 
        {
            if ( f ) UpdateTime() ;
            if ( week >= 0 && week < 7 )
                return weekStr[week] ;
            cerr << "Week error in Calendar! Please check the value of week! Return Mon "
                 << "instead!"  << endl ;
            return weekStr[0] ;
        }

        int GetHour(bool f)
        {
            if ( f ) UpdateTime() ;
            return hour ;
        }

        int GetMinute(bool f)
        {
            if ( f ) UpdateTime() ;
            return minute ;
        }

        int GetSecond(bool f)
        {
            if ( f ) UpdateTime() ;
            return second ;
        }

        // f: if f is true, then call updatetime() first.
        void Print(bool f)
        {
            if ( f )  UpdateTime() ;
            cout << year << "-" << day << "-" << month << ", " ;
            cout << weekStr[week] << ", " << hour << ":" << minute << ":" << second  ;
            cout << ", GMT " << (timeZone >= 0 ? "+" : "") << timeZone ;
        }

        void PrintWithoutTimeZone(bool f)
        {
            if ( f ) UpdateTime() ;
            cout << year << "-" << day << "-" << month << ", " ;
            cout << hour << ":" << minute << ":" << second ;
        }

        // return time - string without timezone
        string TimeWithoutTimeZone(bool f)
        {
            if ( f ) UpdateTime() ;
            ostringstream oss ; 
            oss << year << "-" << day << "-" << month << " " ;
            oss << hour << ":" << minute << ":" << second ;
            return oss.str() ;
        }

        string Time(bool f)
        {
            if ( f )  UpdateTime() ;
            ostringstream oss ;
            oss << year << "-" << day << "-" << month << ", " ;
            oss << weekStr[week] << ", " << hour << ":" << minute << ":" << second  ;
            oss << ", GMT " << (timeZone >= 0 ? "+" : "") << timeZone ;
            return oss.str() ;
        }

    } ;
}

#endif // _CALENDAR_H_
