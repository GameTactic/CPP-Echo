# GameTactic Echo Microservice

This is used as echoing pings and data in the room.

## Usage

### Docker

To start server, run.
` docker run -p 80:80 gametactic/cpp-echo`

To run the server on a non default port
` docker run -p 80:8080 gametactic/cpp-echo --port 8080`

Docker image is very minimal and currently it's size is under 5MB!

### From Sources


Requirements: 

 - libboost
 - cmake
 - gcc
 - websocketspp

To build server itself, run:
```
git clone https://github.com/GameTactic/CPP-Echo.git echo
cd echo
git submodule update --init --recursive
cmake .
make -j $(nproc --all) && make install
```

After this server can be ran by:
```
bin/server
```

Server will automatically bind into port 80.

### Command Line Options

* --port <portNo>    Change the default port the server is listening on (default 80)
* --help             Display the help text.

### Server Usage

To use this server you should open websocket connection to port 80.
After this have been done, you need to specify room to listen.
This can be done by sending following data to server:

```
join:xxxx
```

In response you should get:

```json
{"success":"Room xxxx selected."}
```

If you try send message without first specifying room, you will get:
```json
{"error":"No room selected."}
```

Any futher messages sent after room selection will be just echo to everybody.

## Contributing

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request.

## Credits

 - [Niko Granö](https://github.com/niko9911)
 - [Loki Astari](https://github.com/Loki-Astari)

## License

GPLv3. Please see LICENSE file for further information.


