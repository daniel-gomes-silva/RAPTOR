/**
 * @file main.cpp
 * @brief Entry point for the RAPTOR application.
 *
 * This file initializes the application, parses input directories, and starts
 * the main event loop for processing user queries (this code is commented out).
 *
 * UPDATE: This file initializes the RAPTOR application with hardcoded GTFS datasets from Porto (metro and STCP)
 * and sets up a REST API server for transit routing queries.
 *
 */

#include <iostream>
#include "Application.h"
#include "crow_all.h"

/**
 * @brief Main function for the RAPTOR application.
 *
 * This function parses command-line arguments or prompts the user for GTFS input directories,
 * initializes the application, and starts the interactive event loop.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return Exit status of the application.
 */
/*int main(int argc, char *argv[]) {
  std::vector<std::string> inputDirectories;

  // Parse command-line arguments for input directories
  if (argc >= 2) {
    for (int i = 1; i < argc; ++i)
      inputDirectories.emplace_back(argv[i]);

  } else {
    // Prompt user for input directories
    std::string input;
    std::cout << "Enter GTFS Input Directories (one per line). Type 'done' to finish, or enter a blank line: " << std::endl;

    while (true) {
      std::getline(std::cin, input);
      if (input == "done" || input.empty()) break;
      inputDirectories.push_back(input);
    }
  }

  // Initialize and run the application
  Application application(inputDirectories);
  application.run();

  return 0;
}*/

int main() {
    const std::vector<std::string> inputDirectories = {
        "../datasets/Porto/metro/GTFS/",
        "../datasets/Porto/stcp/GTFS/"
    };

    // Initialize RAPTOR algorithm
    Application application(inputDirectories);
    application.initializeRaptor();

    crow::SimpleApp app;

    CROW_ROUTE(app, "/query").methods("POST"_method)
    ([&application](const crow::request &req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(crow::status::BAD_REQUEST, "Invalid JSON");

        if (!body.has("source") || !body.has("target")) {
            return crow::response(crow::status::BAD_REQUEST, "Missing required fields");
        }
        std::string source = body["source"].s();
        std::transform(source.begin(), source.end(), source.begin(), ::toupper);
        std::string target = body["target"].s();
        std::transform(target.begin(), target.end(), target.begin(), ::toupper);
        if (!application.isValidStop(source) || !application.isValidStop(target)) {
            return crow::response(crow::status::BAD_REQUEST, "Invalid source or target stop ID");
        }

        // Get current time and set default values if not provided
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm *now_tm = std::localtime(&now_c);

        int year = body.has("year") ? body["year"].i() : (now_tm->tm_year + 1900);
        int month = body.has("month") ? body["month"].i() : (now_tm->tm_mon + 1);
        int day = body.has("day") ? body["day"].i() : now_tm->tm_mday;
        int hours = body.has("hours") ? body["hours"].i() : now_tm->tm_hour;
        int minutes = body.has("minutes") ? body["minutes"].i() : now_tm->tm_min;

        // TODO: improve validation (30th February, etc.)
        if (year < 2000 || month < 1 || month > 12 || day < 1 || day > 31 ||
            hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
            return crow::response(crow::status::BAD_REQUEST, "Invalid date/time values");
        }

        auto result = application.handleQueryAPI(source, target, year, month, day, hours, minutes);

        return crow::response(crow::status::OK, result);
    });

    //set the port, set the app to run on multiple threads, and run the app
    app.port(18080).multithreaded().run();

    return 0;
}
