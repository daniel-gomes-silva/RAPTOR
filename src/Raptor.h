/**
 * @file Raptor.h
 * @brief Defines the Raptor class for finding Pareto-optimal journeys in a transit network.
 *
 * This header file declares the Raptor class, which implements the
 * Round-Based Public Transit Routing algorithm.
 *
 * The main method involve finding journeys.
 *
 * The class also contains several private methods for initializing the algorithm,
 * traversing routes, and reconstructing journeys.
 *
 * @author Maria Rabelo and Daniel Gomes Silva
 * @date 22/07/2025
 */
#ifndef RAPTOR_RAPTOR_H
#define RAPTOR_RAPTOR_H

#include <iostream>
#include <vector>
#include <iomanip>  // for setw
#include "Parser.h"
#include "Utils.h"
#include <hiredis/hiredis.h>

/**
 * @class Raptor
 * @brief Implements the RAPTOR algorithm for finding Pareto-optimal journeys.
 *
 * The Raptor class provides methods to set a query, find Pareto-optimal journeys,
 * and print journey steps. It uses various data structures to store information
 * about agencies, calendars, stops, routes, trips, and stop times.
 */
class Raptor {
public:
    /**
     * @brief Default constructor for the Raptor class.
     *
     * Initializes the Raptor object with empty data structures.
     */
    Raptor() = default;

    /**
     * @brief Parameterized constructor for Raptor.
     *
     * Initializes the Raptor object with provided agency, calendar, stop, route, trip, and stop time data.
     *
     * @param[in] agencies_ A map of agency IDs to Agency objects.
     * @param[in] calendars_ A map of calendar IDs to Calendar objects.
     * @param[in] stops A map of stop IDs to Stop objects.
     * @param[in] routes A map of pairs of route IDs and direction IDs to Route objects.
     * @param[in] trips A map of trip IDs to Trip objects.
     * @param[in] stop_times A map of pairs of trip IDs and stop IDs to StopTime objects.
     */
    Raptor(const std::unordered_map<std::string, Agency> &agencies_,
           const std::unordered_map<std::string, Calendar> &calendars_,
           const std::unordered_map<std::string, Stop> &stops,
           const std::unordered_map<std::pair<std::string, std::string>, Route, pair_hash> &routes,
           const std::unordered_map<std::string, Trip> &trips,
           const std::unordered_map<std::pair<std::string, std::string>, StopTime, pair_hash> &stop_times);

    /**
     * @brief Sets the query for the Raptor algorithm.
     *
     * @param[in] query The query containing the parameters for journey search.
     */
    void setQuery(const Query &query);

    /**
     * @brief Finds all Pareto-optimal journeys.
     *
     * This function uses the RAPTOR algorithm to compute all optimal journeys based on the provided query.
     *
     * @return A vector of Journey objects representing the Pareto-optimal journeys.
     */
    std::vector<Journey> findJourneys();

    /**
    * @brief Displays the steps of a journey.
    *
    * Prints each step of the given journey to the console.
    *
    * @param[in] journey The Journey object to be displayed.
    */
    static void showJourney(const Journey &journey);

    /**
     * @brief Gets the stops in the system.
     *
     * @return A reference to the map of stop IDs to Stop objects.
     */
    const std::unordered_map<std::string, Stop> &getStops() const;

    /**
     * @brief Validates if the given journey is valid.
     *
     * Checks whether the given journey meets the required criteria.
     *
     * @param[in] journey The Journey object to be validated.
     * @return True if the journey is valid, false otherwise.
     */
    bool isValidJourney(Journey journey) const;

private:
    std::unordered_map<std::string, Agency> agencies_; ///< Map of agency IDs to Agency objects.
    std::unordered_map<std::string, Calendar> calendars_; ///< Map of service IDs to Calendar objects.
    std::unordered_map<std::string, Stop> stops_; ///< Map of stop IDs to Stop objects.
    std::unordered_map<std::pair<std::string, std::string>, Route, pair_hash> routes_;
    ///< Map of route keys to Route objects.
    std::unordered_map<std::string, Trip> trips_; ///< Map of trip IDs to Trip objects.
    std::unordered_map<std::pair<std::string, std::string>, StopTime, pair_hash> stop_times_;
    ///< Map of stop time keys to StopTime objects.

    Query query_; ///< The current query for the RAPTOR algorithm.
    std::unordered_map<std::string, std::vector<StopInfo> > arrivals_;
    ///< Map of stop IDs to vectors of StopInfo for each k.
    std::unordered_set<std::string> prev_marked_stops; ///< Set of previously marked stops.
    std::unordered_set<std::string> marked_stops; ///< Set of currently marked stops.
    int k{}; ///< The current round of the algorithm.

    /**
     * @brief Initializes the footpaths between stops.
     */
    void initializeFootpaths();

    /**
     * @brief Initializes the footpaths between stops using OSRM (Open Source Routing Machine) durations.
     *
     * This method retrieves walking times between stops by querying a Redis database
     * that contains precomputed OSRM routing durations.
     * The cached durations provide efficient access to accurate pedestrian travel times
     * between stops, which are used during the RAPTOR algorithm to enable transfers.
     *
     */
    void initializeFootpathsOSRM();

    /**
     * @brief Initializes the algorithm by setting required parameters.
     */
    void initializeAlgorithm();

    /**
     * @brief Sets the minimum arrival time for a given stop.
     *
     * @param[in] stop_id The ID of the stop.
     * @param[in] stop_info The stop info containing the arrival time, parent trip ID, and parent stop ID.
     */
    void setMinArrivalTime(const std::string &stop_id, StopInfo stop_info);

    /**
     * @brief Fills the active trips for a given day.
     *
     * @param[in] day The day for which trips are being filled.
     */
    void fillActiveTrips(Day day);

    /**
     * @brief Sets the upper bound for the search, based on previous round.
     */
    void setUpperBound();

    /**
     * @brief Accumulates routes serving each stop.
     *
     * @return A set of routes that serve stops.
     */
    std::unordered_set<std::pair<std::pair<std::string, std::string>, std::string>, nested_pair_hash>
    accumulateRoutesServingStops();

    /**
     * @brief Traverses the routes serving each stop.
     *
     * @param[in] routes_stops_set The set of routes and stops to be traversed.
     */
    void traverseRoutes(
        std::unordered_set<std::pair<std::pair<std::string, std::string>, std::string>, nested_pair_hash>
        routes_stops_set);

    /**
     * @brief Finds the earliest trip for a given stop and route.
     *
     * @param[in] pi_stop_id The ID of the stop.
     * @param[in] route_key The key consisting of route and direction.
     * @return An optional pair of trip ID and day if found.
     */
    std::optional<std::pair<std::string, Day> >
    findEarliestTrip(const std::string &pi_stop_id, const std::pair<std::string, std::string> &route_key);

    /**
     * @brief Checks if a trip is valid based on the route and stop time.
     *
     * @param[in] route_key The key consisting of route and direction.
     * @param[in] stop_time The stop time for the trip.
     * @param[in] day The day to check the trip against.
     * @return True if the trip is valid, false otherwise.
     */
    bool isValidTrip(const std::pair<std::string, std::string> &route_key, const StopTime &stop_time, const Day &day);

    /**
     * @brief Checks if the service is active based on the calendar and date.
     *
     * @param[in] calendar The calendar object containing service dates.
     * @param[in] date The date to check.
     * @return True if the service is active on the given date, false otherwise.
     */
    static bool isServiceActive(const Calendar &calendar, const Date &date);

    /**
     * @brief Traverses a specific trip.
     *
     * @param[in,out] et_id The trip ID.
     * @param[in,out] et_day The day of travel.
     * @param[in,out] pi_stop_id The stop ID for the trip.
     */
    void traverseTrip(std::string &et_id, Day &et_day, std::string &pi_stop_id);

    /**
     * @brief Compares two arrival times to determine which is earlier.
     *
     * @param[in] secondsA The first arrival time in seconds.
     * @param[in] secondsB The second arrival time in seconds.
     * @return True if the first arrival time is earlier, false otherwise.
     */
    static bool earlier(int secondsA, std::optional<int> secondsB);

    /**
     * @brief Checks if a step improves the arrival time for a destination.
     *
     * @param[in] arrival The arrival time.
     * @param[in] dest_id The destination stop ID.
     * @return True if the arrival time improves, false otherwise.
     */
    bool improvesArrivalTime(int arrival, const std::string &dest_id);

    /**
     * @brief Marks a stop with the arrival time, parent trip, and parent stop.
     *
     * @param[in] stop_id The ID of the stop.
     * @param[in] arrival The arrival time at the stop.
     * @param[in] parent_trip_id The ID of the parent trip.
     * @param[in] parent_stop_id The ID of the parent stop.
     */
    void markStop(const std::string &stop_id, int arrival,
                  const std::optional<std::string> &parent_trip_id, const std::optional<std::string> &parent_stop_id);

    /**
     * @brief Handles footpath logic during traversal.
     */
    void handleFootpaths();

    /**
     * @brief Checks if the given stop info represents a footpath.
     *
     * @param[in] stop_info The stop info to be checked.
     * @return True if the stop is a footpath, false otherwise.
     */
    static bool isFootpath(const StopInfo &stop_info);

    /**
     * @brief Reconstructs the journey based on the current round.
     *
     * @return A Journey object representing the reconstructed journey.
     */
    Journey reconstructJourney();

    /**
     * Checks if a given journey is dominated by any other journey in the list.
     *
     * @param journeys A list of all journeys to compare against.
     * @param journey The journey to check.
     * @return True if the journey is dominated, otherwise false.
     */
    static bool isDominatedByAny(const std::vector<Journey> &journeys, const Journey &journey);

    /**
     * @brief Keeps the Pareto-optimal journeys from a list of journeys.
     *
     * @param[in] journeys The list of journeys to be filtered.
     * @return A list of Pareto-optimal journeys.
     */
    static void keepParetoOptimal(std::vector<Journey> &journeys);

    /**
     * @brief Compares two journeys to check if one dominates the other.
     *
     * @param[in] journey1 The first journey to be compared.
     * @param[in] journey2 The second journey to be compared.
     * @return True if the first journey dominates the second, false otherwise.
     */
    static bool dominates(const Journey &journey1, const Journey &journey2);
};

#endif //RAPTOR_RAPTOR_H
