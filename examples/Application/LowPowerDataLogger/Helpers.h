#pragma one

#include <Arduino.h>
#include <mbed_mktime.h>

#include "DebugMode.h"

// Convert compile time to system time
time_t compileDateTimeToSystemTime(const String date, const String time, bool local_time = true, int tz = 0)
{
    char s_month[5];
    int year;

    tm t;
    time_t seconds;

    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sscanf(date.c_str(), "%s %d %d", s_month, &t.tm_mday, &year);
    sscanf(time.c_str(), "%2d %*c %2d %*c %2d", &t.tm_hour, &t.tm_min, &t.tm_sec);

    // Find where is s_month in month_names. Deduce month value.
    t.tm_mon = (strstr(month_names, s_month) - month_names) / 3;
    t.tm_year = year - 1900;
    _rtc_maketime(&t, &seconds, RTC_FULL_LEAP_YEAR_SUPPORT);

    if (!local_time)
    {
        if (tz > 200)
        {
            tz = 0x100 - tz; // Handle negative values
            seconds += (3600UL) * tz;
        }
        else
        {
            seconds -= (3600UL) * tz;
        }
    }

    return seconds;
}

String getLocaltime()
{
    char buffer[32];
    tm t;
    _rtc_localtime(time(NULL), &t, RTC_FULL_LEAP_YEAR_SUPPORT);
    strftime(buffer, 32, "%Y-%m-%d %k:%M:%S", &t);
    return String(buffer);
}

String getLocaltime(const time_t &build_time)
{
    char buffer[32];
    tm t;
    _rtc_localtime(build_time, &t, RTC_FULL_LEAP_YEAR_SUPPORT);
    strftime(buffer, 32, "%Y-%m-%d %k:%M:%S", &t);
    return String(buffer);
}

/**
 * Set system and TimeAlarm clock from compile datetime or RTC
 */
void setSystemClock(String compileDate, String compileTime)
{
    // Retrieve clock time from compile date...
    auto buildTime = compileDateTimeToSystemTime(compileDate, compileTime, true, 2);
    // ... ore use the one from integrated RTC.
    auto rtcTime = time(NULL);

    // Remeber to connect at least the CR2032 battery
    // to keep the RTC running.
    auto actualTime = rtcTime > buildTime ? rtcTime : buildTime;

    // Set both system time and the alarms one
    set_time(actualTime);

    if constexpr (debugMode) Serial.print("Compile Date and Time: ");
    if constexpr (debugMode) Serial.println(getLocaltime(buildTime));
    if constexpr (debugMode) Serial.print("RTC Date and Time:     ");
    if constexpr (debugMode) Serial.println(getLocaltime(rtcTime));
    if constexpr (debugMode) Serial.print("System Clock:          ");
    if constexpr (debugMode) Serial.println(getLocaltime());
}