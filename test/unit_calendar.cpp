#ifdef UNITTEST
#include "calendar.h"

int main( void )
{
  CLPIM::Calendar c ;
  c.SetTimeZone(8) ;

  cout << "--Test constructor--" << endl ;
  cout << "Year: " << c.GetYear() << endl  ;
  cout << "Month: " << c.GetMonth() << endl  ;
  cout << "Day: " << c.GetDay() << endl  ;
  cout << "Week: " << c.GetWeek() << endl  ;
  cout << "Hour: " << c.GetHour() << endl  ;
  cout << "Minute: " << c.GetMinute() << endl  ;
  cout << "Second: " << c.GetSecond() << endl  ;

  cout << "--Test UpdateTime Function in Calendar--" << endl ;
  c.UpdateTime() ;
  cout << "Year: " << c.GetYear() << endl  ;
  cout << "Month: " << c.GetMonth() << endl  ;
  cout << "Day: " << c.GetDay() << endl  ;
  cout << "Week: " << c.GetWeek() << endl  ;
  cout << "Hour: " << c.GetHour() << endl  ;
  cout << "Minute: " << c.GetMinute() << endl  ;
  cout << "Second: " << c.GetSecond() << endl  ;

  /*
  cout << "--Test Set Functions in Calendar--" << endl ;
  c.SetYear(1999) ;
  cout << "Year: " << c.GetYear() << endl  ;
  c.SetMonth(11) ;
  cout << "Month: " << c.GetMonth() << endl  ;
  c.SetDay(23) ;
  cout << "Day: " << c.GetDay() << endl  ;
  c.SetWeek(4) ;
  cout << "Week: " << c.GetWeek() << endl  ;
  cout << "Hour: " << c.GetHour() << endl  ;
  cout << "Minute: " << c.GetMinute() << endl  ;
  c.SetSecond(56) ;
  cout << "Second: " << c.GetSecond() << endl  ;

  c.Print() ;
  */
  {
    int zone, year, month, day, hour ;
    while ( cin >> zone >> year >> month >> day >> hour )
    {
      if ( year == -1 )  break ;
      c.SetYear(year) ;
      c.SetMonth( month ) ;
      c.SetDay(day) ;
      c.SetHour(hour) ;
      c.SetTimeZone(zone) ;
      
      c.Print() ;
    }

  }

  return 0 ;
}
#endif // UNITTEST
