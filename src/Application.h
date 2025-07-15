/**
 * @file Application.h
 * @brief Defines the Application class, which manages the initialization and execution of the RAPTOR application.
 *
 * This header provides declarations for the main application class, including methods
 * for running the application, handling user queries, and interacting with the RAPTOR algorithm.
 *
 * @autor Maria
 * @date 11/11/2024
 */
#ifndef RAPTOR_APPLICATION_H
#define RAPTOR_APPLICATION_H

#include "Raptor.h"
#include <iostream>
#include <iomanip>

/**
 * @class Application
 * @brief The main application class for managing the RAPTOR transit algorithm.
 *
 * This class provides methods to initialize data structures, handle user input,
 * and execute the RAPTOR algorithm for transit planning.
 */
class Application {
public:
    /**
     * @brief Constructs an Application instance with the given input directories.
     * @param inputDirectories A vector of directories containing transit data files.
     */
    explicit Application(std::vector<std::string> inputDirectories);

    /**
     * @brief Starts the application, providing a command-line interface for users.
     */
    void run();

    void handleQueryAPI(const std::string &source, const std::string &target,
                        int year, int month, int day, int hours, int minutes);

private:
    std::vector<std::string> inputDirectories; ///< Directories containing transit data files.
    std::optional<Raptor> raptor_; ///< Optional instance of the RAPTOR algorithm.

    /**
     * @brief Initializes the RAPTOR data structures by parsing input files.
     */
    void initializeRaptor();

    /**
     * @brief Displays the list of available commands to the user.
     */
    static void showCommands();

    /**
     * @brief Handles a user query by executing the RAPTOR algorithm and displaying results.
     */
    void handleQuery();

    /**
     * @brief Retrieves a query from the user, including source, target, date, and time.
     * @return A Query object representing the user's transit request.
     */
    Query getQuery();

    /**
     * @brief Prompts the user to enter the source stop ID.
     * @return A valid source stop ID.
     */
    std::string getSource();

    /**
     * @brief Prompts the user to enter the target stop ID.
     * @return A valid target stop ID.
     */
    std::string getTarget();

    /**
     * @brief Prompts the user to enter the journey date.
     * @return A Date object representing the entered date.
     */
    static Date getDate();
    static Date getDate(int year, int month, int day);

    /**
     * @brief Prompts the user to enter the year.
     * @return The entered year as an integer.
     */
    static int getYear();

    /**
     * @brief Prompts the user to enter the month.
     * @return The entered month as an integer.
     */
    static int getMonth();

    /**
     * @brief Prompts the user to enter the day.
     * @param year The year of the journey, used for validation.
     * @param month The month of the journey, used for validation.
     * @return The entered day as an integer.
     */
    static int getDay(int year, int month);

    /**
     * @brief Prompts the user to enter the departure time.
     * @return A Time object representing the departure time.
     */
    static Time getDepartureTime();

    /**
     * @brief Prompts the user to enter the hour component of the departure time.
     * @return The entered hour as an integer.
     */
    static int getHours();

    /**
     * @brief Prompts the user to enter the minutes component of the departure time.
     * @return The entered minutes as an integer.
     */
    static int getMinutes();
};

#endif //RAPTOR_APPLICATION_H
