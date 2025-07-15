/**
 * @file Application.cpp
 * @brief Application class implementation
 *
 * This file contains the implementation of the Application class, which manages
 * the initialization and execution of the RAPTOR application.
 *
 * @autor Maria
 * @date 11/11/2024
 */
#include "Application.h"

Application::Application(std::vector<std::string> inputDirectories)
    : inputDirectories(std::move(inputDirectories)) {
}

void Application::run() {
    initializeRaptor();

    /*std::string command;
    showCommands();

    while (true) {
      std::cout << std::endl << "Type a command: ";
      std::getline(std::cin, command);

      Utils::clean(command);

      if (command == "query") {
        handleQuery();
      } else if (command == "help") {
        showCommands();
      } else if (command == "quit") {
        std::cout << "Quitting program..." << std::endl;
        break;
      } else {
        std::cout << "Invalid command. :/" << std::endl;
        showCommands();
      }
    }*/
}

void Application::initializeRaptor() {
    std::unordered_map<std::string, Agency> agencies;
    std::unordered_map<std::string, Calendar> calendars;
    std::unordered_map<std::string, Trip> trips;
    std::unordered_map<std::pair<std::string, std::string>, Route, pair_hash> routes;
    std::unordered_map<std::string, Stop> stops;
    std::unordered_map<std::pair<std::string, std::string>, StopTime, pair_hash> stop_times;

    for (const auto &dir: inputDirectories) {
        Parser parser(dir);

        auto dirAgencies = parser.getAgencies();
        agencies.insert(dirAgencies.begin(), dirAgencies.end());

        auto dirCalendars = parser.getCalendars();
        calendars.insert(dirCalendars.begin(), dirCalendars.end());

        auto dirTrips = parser.getTrips();
        trips.insert(dirTrips.begin(), dirTrips.end());

        auto dirRoutes = parser.getRoutes();
        routes.insert(dirRoutes.begin(), dirRoutes.end());

        auto dirStops = parser.getStops();
        stops.insert(dirStops.begin(), dirStops.end());

        auto dirStopTimes = parser.getStopTimes();
        stop_times.insert(dirStopTimes.begin(), dirStopTimes.end());
    }

    raptor_ = Raptor(agencies, calendars, stops, routes, trips, stop_times);
}

void Application::showCommands() {
    std::cout << std::endl << "Available commands:" << std::endl;

    std::cout << std::left << std::setw(30) << " 1. query " << " Runs RAPTOR algorithm." << std::endl;
    std::cout << std::left << std::setw(30) << " 2. help " << " Shows available commands. " << std::endl;

    std::cout << " 3. quit " << std::endl;
}

void Application::handleQuery() {
    Query query = getQuery();
    raptor_->setQuery(query);

    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<Journey> journeys = raptor_->findJourneys();
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "Took " << duration << " ms (" << std::round(static_cast<double>(duration) / 1000.0) <<
            " seconds) to look for journeys."
            << std::endl;

    if (journeys.empty()) std::cout << "No journey found :/" << std::endl;
    else {
        std::cout << "Found " << journeys.size() << " journey(s)! =) " << std::endl;
        for (int i = 0; i < journeys.size(); i++) {
            const Journey &journey = journeys[i];
            int journey_duration = journey.duration;
            std::cout << std::endl << "Journey " << i + 1 << " (" << Utils::secondsToTime(journey_duration) << "): "
                    << std::endl << std::endl;
            Raptor::showJourney(journey);
        }
    }
}

void Application::handleQueryAPI(const std::string &source, const std::string &target,
                                 const int year, const int month, const int day, const int hours, const int minutes) {
    Query query = {source, target, getDate(year, month, day), {hours, minutes, 0}};
    raptor_->setQuery(query);

    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<Journey> journeys = raptor_->findJourneys();
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "Took " << duration << " ms (" << std::round(static_cast<double>(duration) / 1000.0) <<
            " seconds) to look for journeys."
            << std::endl;

    if (journeys.empty()) std::cout << "No journey found :/" << std::endl;
    else {
        std::cout << "Found " << journeys.size() << " journey(s)! =) " << std::endl;
        for (int i = 0; i < journeys.size(); i++) {
            const Journey &journey = journeys[i];
            int journey_duration = journey.duration;
            std::cout << std::endl << "Journey " << i + 1 << " (" << Utils::secondsToTime(journey_duration) << "): "
                    << std::endl << std::endl;
            Raptor::showJourney(journey);
        }
    }
}

Query Application::getQuery() {
    std::string source = getSource();
    std::string target = getTarget();
    Date date = getDate();
    Time departure_time = getDepartureTime();

    return {source, target, date, departure_time};
}

std::string Application::getSource() {
    std::string source;
    while (true) {
        std::cout << "Source stop id: ";
        std::getline(std::cin, source);
        Utils::clean(source);

        if (raptor_->getStops().find(source) != raptor_->getStops().end())
            break;
        else
            std::cout << "Invalid source stop id. Please try again. Example: 5753 for Metro or SAL2 for STCP." <<
                    std::endl;
    }

    return source;
}

std::string Application::getTarget() {
    std::string target;
    while (true) {
        std::cout << "Target stop id: ";
        std::getline(std::cin, target);
        Utils::clean(target);

        if (raptor_->getStops().find(target) != raptor_->getStops().end())
            break;
        else
            std::cout << "Invalid target stop id. Please try again. Example: 5753 for Metro or SAL2 for STCP." <<
                    std::endl;
    }

    return target;
}

Date Application::getDate() {
    int year = getYear();
    int month = getMonth();
    int day = getDay(year, month);

    std::tm time_info = {};
    time_info.tm_year = year - 1900;
    time_info.tm_mon = month - 1;
    time_info.tm_mday = day;
    std::mktime(&time_info);

    return {year, month, day, time_info.tm_wday};
}

Date Application::getDate(const int year, const int month, const int day) {
    std::tm time_info = {};
    time_info.tm_year = year - 1900;
    time_info.tm_mon = month - 1;
    time_info.tm_mday = day;
    std::mktime(&time_info);

    return {year, month, day, time_info.tm_wday};
}

int Application::getYear() {
    std::string input;
    int year;
    while (true) {
        std::cout << "Year (e.g., 2024): ";
        std::getline(std::cin, input);
        Utils::clean(input);

        if (Utils::isNumber(input)) {
            year = std::stoi(input);
            if (year > 1900) break;
        }
        std::cout << "Invalid year. Please enter a valid year in number format, greater than 1900." << std::endl;
    }
    return year;
}

int Application::getMonth() {
    std::string input;
    int month;
    while (true) {
        std::cout << "Month (1-12): ";
        std::getline(std::cin, input);
        Utils::clean(input);
        if (Utils::isNumber(input)) {
            month = std::stoi(input);
            if (month >= 1 && month <= 12) break;
        }
        std::cout << "Invalid month. Please enter a valid month between 1 and 12." << std::endl;
    }
    return month;
}

int Application::getDay(int year, int month) {
    std::string input;
    int day;
    while (true) {
        std::cout << "Day:  ";
        std::getline(std::cin, input);
        Utils::clean(input);
        if (Utils::isNumber(input)) {
            day = std::stoi(input);
            if (day >= 1 && day <= Utils::daysInMonth(year, month)) break;
        }
        std::cout << "Invalid day. Please enter a valid day for this month." << std::endl;
    }
    return day;
}

Time Application::getDepartureTime() {
    int hours = getHours();
    int minutes = getMinutes();

    return {hours, minutes, 0};
}

int Application::getHours() {
    std::string input;
    int hours;
    while (true) {
        std::cout << "Hours (0-23): ";
        std::getline(std::cin, input);
        Utils::clean(input);

        if (Utils::isNumber(input)) {
            hours = std::stoi(input);
            if (hours >= 0 && hours <= 23) break;
        }
        std::cout << "Invalid hours. Please enter a valid hour between 0 and 23." << std::endl;
    }
    return hours;
}

int Application::getMinutes() {
    std::string input;
    int minutes;
    while (true) {
        std::cout << "Minutes (0-59): ";
        std::getline(std::cin, input);
        Utils::clean(input);

        if (Utils::isNumber(input)) {
            minutes = std::stoi(input);
            if (minutes >= 0 && minutes <= 59) break;
        }
        std::cout << "Invalid minutes. Please enter valid minutes between 0 and 59." << std::endl;
    }
    return minutes;
}
