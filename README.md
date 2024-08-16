
# HTTP File Server in C++

This project is a simple multithreaded HTTP server built using C++. The server is capable of handling basic HTTP requests, such as serving files, echoing messages, and responding to user-agent queries.


## Features

- GET Request Handling: Serve files from a specified directory.
- POST Request Handling: Save files sent via POST requests to the server.
- /echo/: Echoes the message sent after /echo/ in the URL.
- User-Agent Logging: Responds with the user-agent information of the client.

## Folder Structure

```bash
├── CMakeLists.txt
├── README.md
├── build/
├── src/
├── vcpkg-configuration.json
├── vcpkg.json
└── your_program.sh
```

- CMakeLists.txt: Contains the CMake configuration for building the project.
- README.md: This file, which describes the project.
- build/: Directory where the compiled binaries will be placed.
- src/: Contains the source code of the HTTP server.
- vcpkg-configuration.json: Configuration file for vcpkg dependencies.
- vcpkg.json: Manifest file for managing dependencies with vcpkg.
- your_program.sh: A shell script for running or setting up the program.
## Dependencies
The project uses the following libraries:

- pthread: For creating and managing threads.
- arpa/inet.h, netdb.h, sys/socket.h, sys/types.h, unistd.h: For networking.
To install dependencies via vcpkg, run the following command:

```bash
  vcpkg install [dependency-list]
```
Make sure to use the vcpkg configuration provided in vcpkg-configuration.json.

## Building the Project

To build the project, follow these steps:

Clone the repository:
```bash
  git clone https://github.com/your-repo/http-server.git
  cd http-server  
```

Build the project:

```bash
  mkdir build
  cd build
  cmake ..
  make
```

The binaries will be generated in the build/ directory.

## Running the Server

After building the project, you can run the server with the following command:


```bash
./http-server --directory <path_to_serve_files_from>
```

For example:

```bash
./http-server --directory /path/to/files
```
This will start the server on port 4221. The server will listen for incoming connections and serve files from the specified directory.
## Running the Server

After building the project, you can run the server with the following command:


```bash
./http-server --directory <path_to_serve_files_from>
```

For example:

```bash
./http-server --directory /path/to/files
```
This will start the server on port 4221. The server will listen for incoming connections and serve files from the specified directory.
## Usage Examples

### 1. GET Request

To retrieve a file, you can send a GET request:
```bash
curl http://localhost:4221/files/<filename>
```
If the file exists, the server will respond with a 200 OK status and the file content.

### 2. POST Request

To upload a file using a POST request:
```bash
curl -X POST -d "file content" http://localhost:4221/files/<filename>
```
The server will save the file and respond with a 201 Created status.

### 3. Echo
To echo a message:
```bash
curl http://localhost:4221/echo/hello
```
The server will respond with the message hello.

## Logging

The server logs various activities, including incoming messages, file retrievals, and POST requests. Logs are printed directly to the console.

## Contributions

Contributions are welcome! Please fork the repository and submit a pull request.

## License
This project is licensed under the MIT License - see the LICENSE file for details.
[MIT](https://choosealicense.com/licenses/mit/)

