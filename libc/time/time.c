#include <time.h>
#include <sys/syscall.h>

const char* monts[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char* weekdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

int century_register = 0x00; // Set by ACPI table parsing code if possible

unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;
unsigned char month;
unsigned int year;

int get_update_in_progress_flag() {
    outb(CMOS_ADDRESS, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

unsigned char get_rtc_register(int reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

void read_rtc() {
    unsigned char century;
    unsigned char last_second;
    unsigned char last_minute;
    unsigned char last_hour;
    unsigned char last_day;
    unsigned char last_month;
    unsigned char last_year;
    unsigned char last_century;
    unsigned char registerB;

    while (get_update_in_progress_flag()); // Make sure an update isn't in progress
    second = get_rtc_register(0x00);
    minute = get_rtc_register(0x02);
    hour = get_rtc_register(0x04);
    day = get_rtc_register(0x07);
    month = get_rtc_register(0x08);
    year = get_rtc_register(0x09);
    if (century_register != 0) {
        century = get_rtc_register(century_register);
    }

    do {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;
        last_century = century;

        while (get_update_in_progress_flag()); // Make sure an update isn't in progress
        second = get_rtc_register(0x00);
        minute = get_rtc_register(0x02);
        hour = get_rtc_register(0x04);
        day = get_rtc_register(0x07);
        month = get_rtc_register(0x08);
        year = get_rtc_register(0x09);
        if (century_register != 0) {
            century = get_rtc_register(century_register);
        }
    } while ((last_second != second) || (last_minute != minute) || (last_hour != hour) ||
             (last_day != day) || (last_month != month) || (last_year != year) ||
             (last_century != century));

    registerB = get_rtc_register(0x0B);

    // Convert BCD to binary values if necessary
    if (!(registerB & 0x04)) {
        second = (second & 0x0F) + ((second / 16) * 10);
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
        if (century_register != 0) {
            century = (century & 0x0F) + ((century / 16) * 10);
        }
    }

    // Convert 12-hour clock to 24-hour clock if necessary
    if (!(registerB & 0x02) && (hour & 0x80)) {
        hour = ((hour & 0x7F) + 12) % 24;
    }

    // Calculate the full (4-digit) year
    if (century_register != 0) {
        year += century * 100;
    } else {
        year += (CURRENT_YEAR / 100) * 100;
        if (year < CURRENT_YEAR) year += 100;
    }
}

int is_leap_year(unsigned int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month(unsigned char month, unsigned int year) {
    int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month == 2 && is_leap_year(year)) {
        return 29;
    }
    return days[month - 1];
}

// Convert date and time to UNIX time in seconds
unsigned long long unix_time() {
    unsigned long long days = 0;
    unsigned int i;

    // Calculate days for complete years
    for (i = 1970; i < year; ++i) {
        days += is_leap_year(i) ? 366 : 365;
    }

    // Calculate days for complete months of the current year
    for (i = 1; i < month; ++i) {
        days += days_in_month(i, year);
    }

    // Add days of the current month
    days += day - 1;

    // Convert days to seconds
    unsigned long long seconds = days * 86400;

    // Add seconds for the current day
    seconds += hour * 3600;
    seconds += minute * 60;
    seconds += second;

    // Return in seconds (UNIX time)
    return seconds;
}

const char* day_of_week(unsigned int day, unsigned int month, unsigned int year) {
    int k = day;
    int m = month;
    int Y = year;

    // Adjust months and year for January and February
    if (m < 3) {
        m += 12;
        Y--;
    }

    // Zeller's Congruence formula
    int h = (k + (13 * (m + 1)) / 5 + Y + (Y / 4) - (Y / 100) + (Y / 400)) % 7;

    // Return the corresponding day name
    return weekdays[h - 1];
}

uint64_t time() {
#if defined(__is_libk)
    read_rtc();
    // Return in seconds (UNIX time)
    return unix_time();
#else
    return syscall(sys_time);
#endif
}

void date() {
#if defined(__is_libk)
    read_rtc();
    
    const char* weekday = day_of_week(day, month, year);
    const char* month_name = monts[month - 1];
    printf("%s %d %s %d %d:%d:%d %s\n", weekday, day, month_name, year, hour, minute, second, TIMEZONE);
#else
    syscall(sys_date);
#endif
}
