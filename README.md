# RAPTOR - Round-Based Public Transit Routing Algorithm
This project implements the Round-Based Public Transit Routing (RAPTOR) algorithm, 
enabling efficient public transport route searches based on GTFS data.

## What's New

### Transition from CLI to REST API

The application's entry point `main.cpp` has been significantly refactored to modernize its interface. 
The original interactive command-line interface (CLI) has been replaced with a persistent **RESTful API server** 
built using the [Crow framework](https://crowcpp.org/master/). This change shifts the application from a standalone, 
interactive tool to a backend service that can be easily integrated with web frontends, mobile apps, or other services.

**Key Changes:**
- **Hardcoded Datasets**: For demonstration purposes, the application is now hardcoded to load GTFS data for Porto's 
metro and STCP bus networks from the `../datasets/` directory
  - Download the latest GTFS files:
    - [Metro do Porto GTFS](https://opendata.porto.digital/dataset/horarios-paragens-e-rotas-em-formato-gtfs)
    - [STCP GTFS](https://opendata.porto.digital/dataset/horarios-paragens-e-rotas-em-formato-gtfs-stcp)
- **Web Server**: It initializes a Crow web server that listens on `port 18080`
- **API Endpoint**: It provides a single endpoint, `POST /query`, to handle all routing requests

**How to Use the API**
1. **Run the application**. The server will start and listen on port `18080`
2. **Send a POST request** to `http://localhost:18080/query` with a JSON body specifying the `source` and `target` 
stop IDs. You can also provide an optional date and time. If the date/time is omitted, the server uses the current 
system time

**Request Body JSON Fields**:

- `source` (string, **required**): The ID of the starting stop (e.g., "FEUP2")
- `target` (string, **required**): The ID of the destination stop (e.g., "FCUP1")
- `year` (int, optional): The departure year
- `month` (int, optional): The departure month (1-12)
- `day` (int, optional): The departure day
- `hours` (int, optional): The departure hour (0-23)
- `minutes` (int, optional): The departure minute (0-59)

**Examples using `curl`**:

```bash
curl -X POST http://localhost:18080/query \
     -H "Content-Type: application/json" \
     -d '{"source": "FEUP2", "target": "FCUP1"}'
```

```bash
curl -X POST http://localhost:18080/query \
     -H "Content-Type: application/json" \
     -d '{"source": "FEUP2", "target": "FCUP1", "year": 2025, "month": 7, "day": 23, "hours": 9, "minutes": 30}'
```

**API Response Format**

The `POST /query` endpoint returns a JSON object containing the query details and a list of possible journeys to get 
from the origin to the destination. The structure of the main JSON response object is as follows:

- **Top-Level Object**

| Key | Type | Description |
|----------|----------|----------|
| `elapsed_time_ms` | Number | The time, in milliseconds, that the server took to process the request |
| `journeys_length` | Number | The total number of journey options found |
| `journeys` | Array | An array of objects, where each object represents a complete journey option |

- **Journey Object** (within the `journeys` array) 
Each object within the journeys array represents a complete journey and has the following structure:

| Key | Type | Description |
|----------|----------|----------|
| `duration` | Number | The total duration of the journey in seconds |
| `steps` | Array | An array of objects, where each object represents a step or segment of the journey (e.g., walking, taking a bus) |

- **Step Object** (within the `steps` array)
Each step of the journey contains detailed information about that segment:

| Key | Type   | Description |
|----------|--------|----------|
| `step_id` | Number | A sequential identifier for the step within the journey (1, 2, 3, ...) |
| `day` | String | Indicates if this step begins on the current day or next day |
| `departure_secs` | Number | The departure time for this step, in seconds since midnight|
| `src_stop_id` | String | The ID of the origin stop for this step |
| `src_stop_name` | String | The name of the origin stop |
| `step_duration_secs` | Number | The duration of this step only, in seconds |
| `dest_stop_id` | String | The ID of the destination stop for this step |
| `dest_stop_name` | String | The name of the destination stop |
| `arrival_secs` | Number | The arrival time for this step, in seconds since midnight (adjusted for trips that go past midnight) |
| `trip_id` | String | The ID of the public transport trip. This will be "footpath" if the step is a walking segment |
| `agency_name` | String | The name of the transit agency (e.g., "Metro"). This will be an empty string for walking segments |

**Example JSON Request and Response**

**Tip:** To make the API response more readable, you can pipe the `curl` output through `jq`

```bash
curl -X POST http://localhost:18080/query \
  -H "Content-Type: application/json" \
  -d '{
    "source": "5777",
    "target": "5776",
    "year": 2025,
    "month": 7,
    "day": 23,
    "hours": 22,
    "minutes": 30
  }' | jq
```

```json
{
  "elapsed_time_ms": 58,
  "journeys": [
    {
      "duration": 120,
      "steps": [
        {
          "agency_name": "Metro",
          "arrival_secs": 81420,
          "day": "current",
          "departure_secs": 81300,
          "dest_stop_id": "5776",
          "dest_stop_name": "Pólo",
          "src_stop_id": "5777",
          "src_stop_name": "Salgueiros",
          "step_duration_secs": 120,
          "step_id": 1,
          "trip_id": "DU406"
        }
      ]
    }
  ],
  "journeys_length": 1
}
```

### Improved Walking Path Calculation with OSRM and Redis

A fundamental improvement was made to the algorithm's accuracy: 
the calculation of walking durations (`footpaths`) between stops

- **Previous Functionality: Estimation with Manhattan Distance**

The original version of the code (`Raptor::initializeFootpaths`) calculated the duration of walking transfers using a 
mathematical estimation. The method was based on the `Manhattan Distance` between the geographical coordinates 
(latitude and longitude) of two stops, applying a `fixed average walking speed` to derive the `duration`.  
While fast, this method is inaccurate as it ignores the actual street topology, obstacles, elevations, 
and the real-world layout of pedestrian paths.

- **Current Functionality: Real-World Data from OSRM via Redis**

The new implementation (`Raptor::initializeFootpathsOSRM`) uses much more accurate route data, pre-calculated with 
the `Open Source Routing Machine (OSRM)`. OSRM calculates the optimal walking `duration using the real street network`.  
To optimize performance at application startup, these `durations were pre-processed and stored in a Redis` in-memory 
database. Now, during initialization, the application connects to Redis to fetch these realistic values.

**Key Advantages:**  
- **Accuracy**: Walking durations reflect `real-world paths`, resulting in much more reliable route suggestions and 
arrival times for the user  
- **Performance**: By using Redis, the application avoids the computational cost of calculating these durations at 
startup. The initialization of `footpaths` is now an extremely fast data-retrieval operation. `Startup takes 
less than 4 minutes`, compared to around 27 minutes (sequential version) and 10 minutes (parallel version) when 
calculating durations on the fly  
- **Flexibility**: The system is `decoupled`. The walking data can be updated or generated by other tools 
(like Valhalla or Google Maps) and populated into Redis without any changes to the main application

<br>

### Setting up the Project Environment in a UNIX-like terminal
1. **Clone the Repository**
    ```bash 
    git clone --recurse-submodules git@github.com:mariaarabelo/RAPTOR.git
    ```

The ``--recurse-submodules flag`` ensures that the submodules (Google Test) are initialized properly. 
    
If you already cloned the repository without this flag, you can initialize submodules later using:
    
```bash 
git submodule update --init --recursive
```
    
2. **Install Dependencies**
 - **g++ 10.0 (for C++20)**

```bash 
sudo apt update
sudo apt install g++ -y
g++ --version
```
 - **CMake 3.22 or later**

```bash 
sudo apt update
sudo apt install -y software-properties-common
sudo apt-add-repository -y ppa:deadsnakes/ppa
sudo apt install cmake -y
cmake --version
```

3. **Build the Program**

```bash
mkdir build
cd build
cmake ..
make
```

### Running the Program

You can run the program by executing one of the following commands in the terminal:

```bash
./RAPTOR ../datasets/Porto/metro/GTFS/ ../datasets/Porto/stcp/GTFS/
./RAPTOR ../datasets/Porto/metro/GTFS/
./RAPTOR ../datasets/Porto/stcp/GTFS/
```

You can specify the path to the GTFS directories directly in the command line.

If no path is provided, the program will prompt you to enter the directory path.

### Running the Tests
You can run the tests by using the following command:

```bash
./tests/TESTS
```

From the tests folder, to automatically run all tests, you can run the command:

```bash 
ctest
```
### Generate Doxygen documentation
To generate the Doxygen documentation, you can run the following command:

```bash
doxygen Doxyfile
```

### Project Structure
- **src/**: Contains the main RAPTOR algorithm implementation and supporting code.
- **datasets/**: Directory for storing GTFS data.
- **tests/**: Directory for test files and Google Test submodule.
- **docs/**: Contains the Doxygen configuration file for generating documentation.

This repository is developed as part of my internship at OPT (Optimizações e Planeamento de Transporte). 

### References
- Delling, Daniel, Thomas Pajor, Renato F. Werneck, “Round-based Public Transit Routing.” Microsoft Research (2012). https://www.microsoft.com/en-us/research/wp-content/uploads/2012/01/raptor_alenex.pdf
- GTFS Schedule Documentation (2024) https://gtfs.org/documentation/schedule/reference/
- Raptor, another journey planning algorithm (2018) https://ljn.io/posts/raptor-journey-planning-algorithm
