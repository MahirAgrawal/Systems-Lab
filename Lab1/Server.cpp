#include<iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include<unistd.h>
#include<thread>
#include"essentials.h"

#define MAX_BACKLOG_QUEUE_LENGTH 3

using namespace std;

int globalUserCnt = 1;

pair<int, sockaddr_in> startServer(int port){
    //AF_INET is for saying its using IPV4
    //SOCK_STREAM is to say TCP stream
    //third argument is protocol but we mention 0 always since systems picks
    //appropriate protocol based on 2nd arg. IF sock_stream then it picks tcp
    //if sock_dgram then udp

    //returns file descriptor as network socket in unix systems is just file
    //where both client and server can read write
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
   
   //preparing address of socket to bind to for listening for active connections
   //socket() just creates socket but address on which that socket listens
   //is specified using sockaddr_in
   sockaddr_in addr;
   addr.sin_family = AF_INET;   //ipv4
   addr.sin_port = htons(port); //htons makes sure port is correctly interpreted irrespective of type of endian cpu uses 
   addr.sin_addr.s_addr = htonl(INADDR_ANY); //same as why htons is used
   
   //:: used because "namespace" scope has another bind function and i
   //don't want c++ to consider that bind function and consider bind function in global scope 
   if(::bind(serverSocket, (sockaddr *) &addr, sizeof(addr)) != 0){
        cout<<"Binding failed..."<<endl;
        return make_pair(-1, addr);
   }

   //listen tells socket to start listening on the address it is bind to
   if(listen(serverSocket, MAX_BACKLOG_QUEUE_LENGTH) != 0){
        cout<<"Failed Listening..."<<endl;
        return make_pair(-1, addr);
   }
   
   cout<<"Server started at port: "<<port<<endl;
   return make_pair(serverSocket, addr);
}

void handleUser(int userSocket, int userID){
    try{
        cout<<"New User#"<<userID<<" Enters The Chat"<<endl;
        char buffer[maxMessageLength] = {0};
        bool closed = 0;
        while(!closed){

            read(userSocket, buffer, maxMessageLength);
            
            printf("Client#%d writes (Encoded): %s\n", userID, buffer);

            pair<int, string> mssg = analyseMessage(buffer, sizeof(buffer));
            
            //clean buffer
            bzero(buffer, sizeof(buffer));
            
            if(mssg.first == -1){
                cerr<<"Invalid Message Sent By Client! Closing Connection!!"<<endl;
                closed = true;
            }
            else if(mssg.first == 1){
                printf("Client#%d writes (Decoded): %s\n", userID, mssg.second.c_str());
                createMessage(2, ack, buffer, maxMessageLength);

                //writing acknowledgement to client
                if(write(userSocket, buffer, strlen(buffer)) == -1)
                    closed = true;
                
                //clean buffer
                bzero(buffer, sizeof(buffer));
            }
            else{
                printf("Client#%d has closed the connection\n", userID);
                closed = true;
            }
        }
        //gracefully closing this particular client's connection
        close(userSocket);
    }catch(exception& e){
        cout<<"Error Occured in Handling Client: "<<e.what()<<endl;
        close(userSocket);
    }
}

void manageNewConnection(int userSocket){
    thread(handleUser, userSocket, globalUserCnt).detach();
    globalUserCnt++;
}

int main(int argc, char *args[]){
    if(argc != 2){
        cout<<"Number of arguments must be one only!!"<<endl;
        exit(1);
    }

    int port = fetchPortValue(args[1]);

    if(port == -1){
        cout<<"Invalid Port Value"<<endl;
        exit(1);
    }

    auto pr = startServer(port);

    if(pr.first == -1){
        cout<<"Server cannot be started!!"<<endl;
        exit(1);
    }

    int serverSocket = pr.first; 
    sockaddr_in addr = pr.second;
    int addrLen = sizeof(addr);

    //listening and accepting connections in loop
    while(1){

        //does 3 way handshake under the hood and handles connection using
        //new file descriptor new_socket
        int new_socket = accept(serverSocket, (struct sockaddr*)&addr, (socklen_t*) &addrLen);

        if(new_socket < 0){
            cout<<"Failed Connection Attempt!!"<<endl;
            continue;
        }

        //handles the connected user using new thread so that listening
        //thread doesn't get blocked
        manageNewConnection(new_socket);
    }
    
    //closing server socket
    close(serverSocket);

    return 0;
}