#include <stdio.h> // Standard input/output library
#include <arpa/inet.h> // For the in_addr structure and the inet_pton function
#include <sys/socket.h> // For the socket function
#include <sys/types.h>
#include <netinet/tcp.h>
#include <unistd.h> // For the close function
#include <string.h> // For the memset function
#include <stdlib.h>
#include <time.h>
#include "RUDP.h"

#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 1024
#define FILE_SIZE 2097152*2 // We can support a file of any size. Just change FILE_SIZE accordingly. For testing purposes, we will set FILE_SIZE as a constant at 2MB.

#define SENDER_ADDR "127.0.0.1"
#define SENDER_PORT 2000
//NOTE about ACKs:
//The TCP Protocol underneath ensures reliability with ACKs and FINs.

//SOURCES:
//Code from Moodle - Sockets in C
//Beej's Guide 

//UTIL FUNCTIONS


// Provided by Beej's Guide
// s is the socket you want to send the data to, buf is the buffer
// containing the data, and len is a pointer to an int containing the number of bytes in the buffer.

//STRUCTS USED IN THIS FILE





int main(int argc, char *argv[]) {
    
    //Initialize variables to their desired names.
    if(argc != 5){
        printf("exit failure\n");
        exit(EXIT_FAILURE);
    }


    //get file
    FILE *fptr;
    fptr = fopen("file.txt", "r");
    if(fptr== NULL){
        printf("ERROR READING FILE");
    }


    //create a UDP SOCKET
    char* ip = argv[2];
    int port = atoi(argv[4]);


    // structs used in this file
    typedef struct{
    uint16_t seq ;
    uint16_t checksum;
    uint16_t length;
    unsigned int ACK: 1;
    unsigned int FIN: 1;
} RUDPHeader;

typedef struct{
    int sockFD;
    struct sockaddr_in sender_addr;
} RUDPSocket;

    
    
    
    
    // create socket and header information
    RUDPSocket* senderSocket = create_rudp_socket(ip, port);
    RUDPHeader* header = create_rudp_header();

    if(senderSocket == NULL){
        printf("ERROR CREATING SOCKET");
    }


    if(bind(senderSocket->sockFD, (struct sockaddr *)&senderSocket->sender_addr, sizeof(senderSocket->sender_addr) < 0)){
        printf("ERROR BINDING");
    }


    //CONNECTION SUCCESFUL AND PSUDO HANDSHAKE COMPLETE. Now sending information
    
    while(TRUE){
    char data[MAX_BUFFER_SIZE];

         //get ack loop
    while (TRUE){
    char* buffer;
    buffer= rudp_recv(senderSocket, header, data, sizeof(header));
    if(buffer == NULL){
        perror("ERROR RECIEVING FROM RECIEVER");
        sleep(2);
    }

    header = get_header(buffer);
    if(header->ACK != 1){
        perror("didn't get ack");
        sleep(2);
        continue;
    }
    break;
    }
    
    //GET FILE AND MOVE ADJUST POINTER
    memcpy(&data, fptr, sizeof(MAX_BUFFER_SIZE));
    fseek(fptr, MAX_BUFFER_SIZE, SEEK_CUR);
        if (rudp_send(senderSocket, header, data, sizeof(data) != NULL)){
            printf("SENT NEXT PACKAGE, WAIT FOR ACK");
        }
        char user_input;
        printf("Resend the file? [y/n] \n");
        scanf(" %c", &user_input);


        char* message;
        //Accepted inputs
        switch(user_input){
            case 'y': 
                message = "RETRY";
                break;
            case 'Y':  
                message = "RETRY";
                break;
            case 'n':
                message = "EXIT";
                break;
            case 'N':
                message = "EXIT";
                break;
            default:
                printf("Resend the file? [y/n] \n");
        }


        if(user_input == 'n' || user_input == 'N'){
            break;  //Break out of the program loop
        }
        
        // Not breaking the loop will cause the program to resend the file.
    }
    printf("Connection Closed.");
    rudp_close(senderSocket);
    exit(EXIT_SUCCESS);
}