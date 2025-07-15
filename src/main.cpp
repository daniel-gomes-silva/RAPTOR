/**
 * @file main.cpp
 * @brief Entry point for the RAPTOR application.
 *
 * This file initializes the application, parses input directories, and starts
 * the main event loop for processing user queries.
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
    std::vector<std::string> inputDirectories = {
        "../datasets/Porto/metro/GTFS/",
        "../datasets/Porto/stcp/GTFS/"
    };

    // Initialize and run the application
    Application application(inputDirectories);
    application.run();

    crow::SimpleApp app;

    CROW_ROUTE(app, "/query").methods("POST"_method)
    ([&application](const crow::request &req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string source = body["source"].s();
        std::string target = body["target"].s();
        int year = body["year"].i();
        int month = body["month"].i();
        int day = body["day"].i();
        int hours = body["hours"].i();
        int minutes = body["minutes"].i();

        /*
        std::cout << "...:::QUERY RECEIVED:::..." << std::endl;
        std::cout << "Source: " << source << std::endl;
        std::cout << "Target: " << target << std::endl;
        std::cout << "Year: " << year << ", Month: " << month << ", Day: " << day << std::endl;
        std::cout << "Hours: " << hours << ", Minutes: " << minutes << std::endl;
        */

        application.handleQueryAPI(source, target, year, month, day, hours, minutes);

        // No response sent to client
        return crow::response();
    });

    //set the port, set the app to run on multiple threads, and run the app
    app.port(18080).multithreaded().run();

    return 0;
}
