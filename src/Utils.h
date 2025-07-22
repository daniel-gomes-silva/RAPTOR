/**
 * @file Utils.h
 * @brief Provides utility functions for the RAPTOR application.
 *
 * This header file declares utility functions for the RAPTOR application,
 * including functions for calculating distances, durations, and time conversions.
 *
 * @author Maria Rabelo and Daniel Gomes Silva
 * @date 22/07/2025
 */

#ifndef RAPTOR_UTILS_H
#define RAPTOR_UTILS_H

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <utility> // for std::pair
#include <vector>
#include <cmath>

#include "DateTime.h"

/**
 * @class Utils
 * @brief A utility class providing various helper functions.
 *
 * This class contains static utility methods to handle mathematical calculations, time conversions,
 * string manipulations, and date operations. These methods are used throughout the RAPTOR project
 * to simplify code and provide common functionality.
 */
class Utils {
public:
    /**
     * @brief Computes the Manhattan distance between two geographical points.
     *
     * This method calculates the Manhattan (or "taxicab") distance between two points given their
     * latitude and longitude in decimal degrees. This distance is useful for certain types of grid-based
     * calculations.
     *
     * @param[in] lat1 Latitude of the first point.
     * @param[in] lon1 Longitude of the first point.
     * @param[in] lat2 Latitude of the second point.
     * @param[in] lon2 Longitude of the second point.
     * @return The Manhattan distance between the two points.
     */
    static double manhattan(const double &lat1, const double &lon1, const double &lat2, const double &lon2);

    /**
     * @brief Calculates the duration between two geographical points in seconds.
     *
     * This method computes the duration based on the geographic distance between two sets of
     * latitude and longitude coordinates, expressed as strings.
     *
     * @param[in] string_lat1 Latitude of the first point as a string.
     * @param[in] string_lon1 Longitude of the first point as a string.
     * @param[in] string_lat2 Latitude of the second point as a string.
     * @param[in] string_lon2 Longitude of the second point as a string.
     * @return The duration in seconds.
     */
    static int getDuration(const std::string &string_lat1, const std::string &string_lon1,
                           const std::string &string_lat2, const std::string &string_lon2);

    /**
     * @brief Converts a time in seconds to a string format (HH:MM:SS).
     *
     * This method converts a given time in seconds into a formatted string representing the time
     * in the "HH:MM:SS" format.
     *
     * @param[in] seconds The time in seconds.
     * @return A string representation of the time in "HH:MM:SS" format.
     */
    static std::string secondsToTime(std::optional<int> seconds);

    /**
     * @brief Converts a time string to the equivalent number of seconds.
     *
     * This method converts a time string (e.g., "12:30:00") to the total number of seconds.
     *
     * @param[in] timeStr A time string in the "HH:MM:SS" format.
     * @return The total time in seconds.
     */
    static int timeToSeconds(const std::string &timeStr);

    /**
     * @brief Converts a Time object to the equivalent number of seconds.
     *
     * This method converts a Time object to the total number of seconds since midnight.
     *
     * @param[in] time A Time object representing a specific time.
     * @return The total time in seconds.
     */
    static int timeToSeconds(const Time &time);

    /**
     * @brief Splits a string into a vector of substrings based on a delimiter.
     *
     * This method splits a string into parts wherever a specified delimiter appears.
     *
     * @param[in] str The input string to be split.
     * @param[in] delimiter The delimiter character to split the string by.
     * @return A vector of substrings split from the input string.
     */
    static std::vector<std::string> split(const std::string &str, char delimiter);

    /**
     * @brief Retrieves the first word in a string.
     *
     * This method extracts and returns the first word from a given string, stopping at the first space.
     *
     * @param[in] str The input string.
     * @return The first word in the string.
     */
    static std::string getFirstWord(const std::string &str);

    /**
     * @brief Trims leading and trailing whitespace from a string.
     *
     * This method removes any leading or trailing whitespace from the given string.
     *
     * @param[in,out] line The line to be cleaned.
     */
    static void clean(std::string &input);

    /**
     * @brief Checks if a string represents a valid number.
     *
     * This method checks whether the input string can be interpreted as a valid numerical value.
     *
     * @param[in] str The input string to be checked.
     * @return True if the string is a valid number, false otherwise.
     */
    static bool isNumber(const std::string &str);

    /**
     * @brief Retrieves the number of days in a specific month of a specific year.
     *
     * This method returns the number of days in a given month, accounting for leap years if applicable.
     *
     * @param[in] year The year of interest.
     * @param[in] month The month of interest (1-12).
     * @return The number of days in the specified month of the specified year.
     */
    static int daysInMonth(int year, int month);

    /**
     * @brief Checks if a date is within a specified date range.
     *
     * This method checks whether a given date falls within the specified range of start and end dates.
     *
     * @param[in] date The date to be checked.
     * @param[in] start_date The start of the date range (in string format).
     * @param[in] end_date The end of the date range (in string format).
     * @return True if the date is within the range, false otherwise.
     */
    static bool dateWithinRange(const Date &date, const std::string &start_date, const std::string &end_date);

    /**
     * @brief Adds one day to a given date.
     *
     * This method increments the given date by one day.
     *
     * @param[in] date The date to which one day should be added.
     * @return The resulting date after adding one day.
     */
    static Date addOneDay(Date date);

    /**
     * @brief Converts a Day enum to a string representation.
     *
     * This method converts a Day enum (Current or Next) to its string representation.
     *
     * @param[in] day The Day enum to be converted.
     * @return The string representation of the specified day.
     */
    static std::string dayToString(Day day);


    /**
     * @brief Creates a unique key for a pair of stop IDs.
     *
     * This method generates a unique key for a pair of stop IDs, which can be used for efficient lookups.
     * The key is created by concatenating the two stop IDs in a consistent order (alphabetically).
     *
     * @param[in] stop_id1 The first stop ID.
     * @param[in] stop_id2 The second stop ID.
     * @return A string representing the unique key for the pair of stop IDs.
     */
    static std::string makeKey(const std::string &stop_id1, const std::string &stop_id2);
};

#endif //RAPTOR_UTILS_H
