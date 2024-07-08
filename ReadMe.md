## Build How-To
Run make from the root of the `mylaps` folder

Platform supported
 - Linux

Dependencies
 - boost::asio - boost libs should be installed to build this project
 - nlohmann::json - included in the project

## Usage

To start the server simply run it:
`> ./bin/ml_server`

To start the client:
`> ./bin/ml_client <host> <port> <data_path>`

Example:

`../bin/ml_client localhost 25000 ../karttimes.csv`

(Please note that for this very simple implementation port number is hardcoded for server, so 25000 should be used to connect to server always)

It is technically possible to cennect to server using netcat utility:

`abrus@MSI-Andrey:~$ netcat localhost 25000`

`{"method":"ranking"}`

`{"method":"ranking","result":[]}`

## Implementation and data description

Clients can send multiple commands withing same message, but every command should be line separated ('\n'). 

Both server and client parts of this project communicate with each other over socket connection. Server implements asynchronous reading, can operate on multiple connected clients simultaneously. Server implements API using JSON-structured commands:
 - register time event for specific driver - no response for this command

`{"method":"lap_event","driver_id":"1","time":"12:00:00"}`

 - get total stats of the racing, sorted by best lap driver

`{"method":"ranking"}`

Response example:

`{"method":"ranking","result":[{"driver_id":"5","total_time":271,"average_lap":67,"best_lap":56,"total_laps":4,"best_lap_diff":0},{"driver_id":"2","total_time":259,"average_lap":64,"best_lap":59,"total_laps":4,"best_lap_diff":3},{"driver_id":"4","total_time":283,"average_lap":70,"best_lap":62,"total_laps":4,"best_lap_diff":6},{"driver_id":"3","total_time":285,"average_lap":71,"best_lap":62,"total_laps":4,"best_lap_diff":6},{"driver_id":"1","total_time":268,"average_lap":67,"best_lap":62,"total_laps":4,"best_lap_diff":6}]}`

 - get stats of specific specific driver (no winner dif in this case)
`{"method":"ranking", "driver_id":"3"}`

Response example:

`{"method":"ranking","result":{"driver_id":"3","total_time":285,"average_lap":71,"best_lap":62,"total_laps":4,"best_lap_diff":0}}`

### JSON data description:
Basic stats returned from server are basically self explanatory:

{
	
    "driver_id":"3",  // driver id

	"total_time":285, // total time of all laps (in seconds)

	"average_lap":71, // average lap time (in seconds)

	"best_lap":62,    // best lap time (in seconds)

	"total_laps":4,   // total amount of laps

	"best_lap_diff":0 // diff in time from the winner best lap (in seconds)
    
}

Total stat command returns the same data but within JSON array, sorted by best_lap, so the first one is the winner.

Client is implemented as a very simple cmd utility, which first connects to the server, sends data from provided file and enters the user-awaiting prompt mode. Entering specified in the menu commands it is possible to add more lap events or get the stats.
