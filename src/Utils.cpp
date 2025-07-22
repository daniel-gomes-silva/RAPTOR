/**
 * @file Utils.cpp
 * @brief Provides utility functions for the RAPTOR application.
 *
 * This file contains utility functions for the RAPTOR application,
 * including functions for calculating distances, durations, and time conversions.
 *
 * @author Maria Rabelo and Daniel Gomes Silva
 * @date 22/07/2025
 */

#include "Utils.h"

double Utils::manhattan(const double &lat1, const double &lon1, const double &lat2, const double &lon2) {
    return std::abs(lat1 - lat2) + std::abs(lon1 - lon2);
}

int Utils::getDuration(const std::string &string_lat1, const std::string &string_lon1,
                       const std::string &string_lat2, const std::string &string_lon2) {
    double lat1, lon1, lat2, lon2;

    try {
        lat1 = std::stod(string_lat1);
        lon1 = std::stod(string_lon1);
        lat2 = std::stod(string_lat2);
        lon2 = std::stod(string_lon2);
    } catch (const std::invalid_argument &e) {
        throw std::runtime_error("Invalid latitude or longitude format.");
    }

    double average_speed = 5.0; // km/h
    double scaling_factor = 111.0; // Approximately 111 km per degree
    double distance = Utils::manhattan(lat1, lon1, lat2, lon2) * scaling_factor;
    return static_cast<int>(std::round((distance / average_speed) * 60 * 60)); // Seconds
}

std::string Utils::secondsToTime(std::optional<int> seconds) {
    if (!seconds.has_value()) return "INF";

    int seconds_value = seconds.value();

    int hours = seconds_value / 3600 % 24;
    int minutes = (seconds_value % 3600) / 60;
    int secs = seconds_value % 60;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours << ":"
            << std::setw(2) << std::setfill('0') << minutes << ":"
            << std::setw(2) << std::setfill('0') << secs;
    return oss.str();
}

int Utils::timeToSeconds(const std::string &timeStr) {
    int hours, minutes, seconds;
    char colon;

    std::istringstream iss(timeStr);
    iss >> hours >> colon >> minutes >> colon >> seconds;

    return hours * 3600 + minutes * 60 + seconds;
}

int Utils::timeToSeconds(const Time &time) {
    return time.hours * 3600 + time.minutes * 60 + time.seconds;
}

std::vector<std::string> Utils::split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    if (str.back() == delimiter) {
        tokens.emplace_back(""); // Add empty token if the last character is the delimiter
    }

    return tokens;
}


std::string Utils::getFirstWord(const std::string &str) {
    return str.substr(0, str.find(' '));
}

void Utils::clean(std::string &input) {
    size_t first = input.find_first_not_of(" \t\n\r");
    size_t last = input.find_last_not_of(" \t\n\r");
    input = (first == std::string::npos) ? "" : input.substr(first, (last - first + 1));
}


bool Utils::isNumber(const std::string &str) {
    return !str.empty() && std::ranges::all_of(str, [](char c) { return std::isdigit(c); });
}

int Utils::daysInMonth(int year, int month) {
    static const int daysInMonths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        return 29; // February in a leap year
    }
    return daysInMonths[month - 1];
}

bool Utils::dateWithinRange(const Date &date, const std::string &start_date, const std::string &end_date) {
    // Convert strings YYYYMMDD to Date objects
    Date start = {
        std::stoi(start_date.substr(0, 4)), std::stoi(start_date.substr(4, 2)),
        std::stoi(start_date.substr(6, 2))
    };
    Date end = {std::stoi(end_date.substr(0, 4)), std::stoi(end_date.substr(4, 2)), std::stoi(end_date.substr(6, 2))};

    // Check if the date is earlier than the start date
    if ((date.year < start.year)
        || (date.year == start.year && date.month < start.month)
        || (date.year == start.year && date.month == start.month && date.day < start.day))
        return false;

    // Check if the date is later than the end date
    if ((date.year > end.year)
        || (date.year == end.year && date.month > end.month)
        || (date.year == end.year && date.month == end.month && date.day > end.day))
        return false;

    return true;
}

Date Utils::addOneDay(Date date) {
    std::tm time_info = {};
    time_info.tm_year = date.year - 1900;
    time_info.tm_mon = date.month - 1;
    time_info.tm_mday = date.day + 1; // Add one day

    std::mktime(&time_info); // Normalize the date
    Date new_date = {time_info.tm_year + 1900, time_info.tm_mon + 1, time_info.tm_mday};

    return new_date;
}

std::string Utils::dayToString(Day day) {
    return (day == Day::CurrentDay) ? "current" : "next";
}

std::string Utils::makeKey(const std::string &stop_id1, const std::string &stop_id2) {
    return (stop_id1 < stop_id2)
               ? stop_id1 + ":" + stop_id2
               : stop_id2 + ":" + stop_id1;
}
