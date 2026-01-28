#include<iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include<unistd.h>
#include <arpa/inet.h>
#include"essentials.h"
using namespace std;

//it connects to server and then returns socket through which client interacts
//with server
int connectToServer(int port, char *ipAddress){

    sockaddr_in addr;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ipAddress);

    //build socket working with ipv4 and tcp protocol
    auto clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    //try to connect to the server of which address is provided by addr
    if(connect(clientSocket, (sockaddr *) &addr, sizeof(addr)) < 0){
        return -1;
    }

    return clientSocket;
}

int printMenuAndGetSelection(){
    int option;
    cout<<"Enter option number: "<<endl;
    cout<<"1. Send Message To Server"<<endl;
    cout<<"2. Exit"<<endl;
    cin>>option;
    return (option == 1 || option == 2) ? option : 2;
}

//responsible for interacting with server
//also responsible for closing socket if connection terminated
void startChatting(int clientSocket){
    cout<<"Connected To Server..."<<endl;

    char buff[maxMessageLength];//maxMessageLength is fixed across both server and client using common library essentials.h
    string message;
    int option = 1;
    
    while(option == 1){
        option = printMenuAndGetSelection();
        switch (option)
        {
            case 1:
                cout<<"Enter your text to send: ";

                //ignore because from previous "cin", there may be '\n' left
                //in input buffer since cin reads until space or newline
                //if that newline is left in buffer then it can be
                //read by getline and so it will not take user's current input
                //because of the previous input's '\n' and hence new input will be ignored.
                //To avoid it and to empty the buffer, we use ignore
                cin.ignore();
                getline(cin, message);
                if(!createMessage(1, message, buff, maxMessageLength)){
                    cout<<"Max limit of Message exceeded. Try sending shorter message!!!"<<endl;
                }
                else{
                    //message would be copied into buff by createMessage if all is good
                    //now we write that to server
                    if(write(clientSocket, buff, strlen(buff)) == -1){
                        cout<<"Error Sending to the Server!!"<<endl;
                        option = 2;
                    }
                    else{
                        //clean buffer
                        bzero(buff, sizeof(buff));
                        
                        //flow comes here means successfully written to server
                        read(clientSocket, buff, sizeof(buff));
                        
                        //server is carefully written hence it will not send
                        //invalid mssg type and so client side no check is required
                        pair<int, string> mssg = analyseMessage(buff, sizeof(buff));
                        printf("From server: %s\n\n", mssg.second.c_str());

                        //clean buffer
                        bzero(buff, sizeof(buff));
                    }
                }
                break;
            default:
                cout<<"Disconnecting from Server..."<<endl;
                //user will exit
                message = "BYE";
                createMessage(3, message, buff, maxMessageLength);
                write(clientSocket, buff, strlen(buff));
                break;
        }
    }

    //cleaning up socket...
    close(clientSocket);
}

int main(int argc, char *args[]){
    if(argc != 3){
        cout<<"Number of arguments must be two only!!"<<endl;
        exit(1);
    }

    int port = fetchPortValue(args[2]);

    if(port == -1){
        cout<<"Invalid Port Value"<<endl;
        exit(1);
    }

    auto clientSocket = connectToServer(port, args[1]);
    
    if(clientSocket == -1){
        cout<<"Could not connect to Server!!";
        exit(1);
    }

    startChatting(clientSocket);
    
    return 0;
}