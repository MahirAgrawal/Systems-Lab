Uses Sockets Library and Multithreading in C

Compile using: 
- g++ Server.cpp -o Server
- g++ Client.cpp -o Client

#### Run Server using ./Server <port_number> and Server will start listening for connections
#### Run Client using ./Client <IP-Address> <port_number> and Client will connect to Server. For IP-Address you can use loopback address (127.0.0.1) if server is running on same machine.
#### You can run multiple clients in parallel which server handles using threads

### Resources Used
[Medium Article on Building TCP Server](https://medium.com/@shivambhadani_/understanding-tcp-and-building-our-own-tcp-server-in-c-language-8de9d9de78ef)
[GFG Article on Socket Implementation in C](https://www.geeksforgeeks.org/c/tcp-server-client-implementation-in-c/)
[Base64](https://en.wikipedia.org/wiki/Base64)
[Decode base64 GFG Reference Article](https://www.geeksforgeeks.org/dsa/decode-encoded-base-64-string-ascii-string/)
[Encode base64 GFG Reference Article](https://www.geeksforgeeks.org/dsa/encode-ascii-string-base-64-format/)