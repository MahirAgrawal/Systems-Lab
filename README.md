Uses Sockets Library and Multithreading in C

Compile using: 
- g++ Server.cpp -o Server
- g++ Client.cpp -o Client

Run Server using ./Server <port_number> and Server will start listening for connections
Run Client using ./Client <IP-Address> <port_number> and Client will connect to Server. For IP-Address you can use loopback address (127.0.0.1) if server is running on same machine.
You can run multiple clients in parallel which server handles using threads
