#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static const int month_days[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

void format_date(char *buf, int buf_size, uint64 timestamp) {
    uint64 seconds = timestamp / 1000000000;
    uint64 nanoseconds = timestamp % 1000000000;
    
    uint64 minutes = seconds / 60;
    uint64 hours = minutes / 60;
    uint64 days = hours / 24;
    
    uint64 year = 1970;
    uint64 month = 0;

    while (1) {
        int is_leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
        int days_in_year = is_leap ? 366 : 365;
        
        if (days >= days_in_year) {
            days -= days_in_year;
            year++;
        } else {
            break;
        }
    }
    
    int is_leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    for (month = 0; month < 12; month++) {
        int days_in_month = month_days[month];
        if (month == 1 && is_leap) days_in_month++;
        
        if (days < days_in_month) break;
        days -= days_in_month;
    }
    
    uint64 day = days + 1;
    month++;
    hours %= 24;
    minutes %= 60;
    seconds %= 60;
    
    char *p = buf;

    *p++ = '0' + (year / 1000);
    year %= 1000;
    *p++ = '0' + (year / 100);
    year %= 100;
    *p++ = '0' + (year / 10);
    year %= 10;
    *p++ = '0' + year;
    *p++ = '-';

    *p++ = '0' + (month / 10);
    *p++ = '0' + (month % 10);
    *p++ = '-';

    *p++ = '0' + (day / 10);
    *p++ = '0' + (day % 10);
    *p++ = ' ';

    *p++ = '0' + (hours / 10);
    *p++ = '0' + (hours % 10);
    *p++ = ':';
 
    *p++ = '0' + (minutes / 10);
    *p++ = '0' + (minutes % 10);
    *p++ = ':';
    
    *p++ = '0' + (seconds / 10);
    *p++ = '0' + (seconds % 10);
    *p++ = '.';
    
    *p++ = '0' + (nanoseconds / 100000000);
    nanoseconds %= 100000000;
    *p++ = '0' + (nanoseconds / 10000000);
    nanoseconds %= 10000000;
    *p++ = '0' + (nanoseconds / 1000000);
    nanoseconds %= 1000000;
    *p++ = '0' + (nanoseconds / 100000);
    nanoseconds %= 100000;
    *p++ = '0' + (nanoseconds / 10000);
    nanoseconds %= 10000;
    *p++ = '0' + (nanoseconds / 1000);
    nanoseconds %= 1000;
    *p++ = '0' + (nanoseconds / 100);
    nanoseconds %= 100;
    *p++ = '0' + (nanoseconds / 10);
    nanoseconds %= 10;
    *p++ = '0' + nanoseconds;
    *p++ = ' ';
    
    *p = '\0'; 
}

int main(int argc, char *argv[]) {
    uint64 time = rtc_time();
    char date_buf[64]; 
    
    format_date(date_buf, sizeof(date_buf), time);
    printf("Current date: %s\n", date_buf);
    
    exit(0);
}