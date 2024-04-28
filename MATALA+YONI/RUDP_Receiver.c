
#include <stdio.h> // Standard input/output library
#include <arpa/inet.h> // For the in_addr structure and the inet_pton function
#include <sys/socket.h> // For the socket function
#include <netinet/tcp.h>
#include <unistd.h> // For the close function
#include <string.h> // For the memset function
#include <stdlib.h>
#include <sys/time.h>
#include "RUDP.h"
#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 1024
#define IP "89.139.40.229"
// #define FILE_SIZE 2000000 // 2MB

//RUDP Server
//The RUDP Protocol underneath ensures reliability with ACKs and FINs.

//SOURCES:
//Code from Moodle - Sockets in C
//Beej's Guide 
//GeeksforGeeks
//StackOverflow

int main(int argc, char *argv[]) {



//STRUCTS USED TO IMPLEMENT RUDP
    typedef struct{
    uint16_t length;
    uint16_t checksum;
    uint16_t seq;
    unsigned int ACK: 1;
    unsigned int FIN: 1;
} RUDPHeader;

typedef struct{
    int sockFD;
    struct sockaddr_in sender_addr;
} RUDPSocket;

 
    //Initialize variables to their desired names.
    //(Wanted to create an initialize function, but it was just too ugly and unreadable)
    if(argc != 3){
        printf("exit failure\n");
        exit(EXIT_FAILURE);
    }

    //set FILE
    FILE* fptr2;
    fopen("newfile.txt", "w");


    int port = atoi(argv[2]);


    // Create a variable for the UDP Socket, and then create a UDP Socket. If creation failed, exit.
    // We will be creating an IPV4 Socket.

char data = {0};
RUDPSocket* recievedSocket = create_rudp_socket(IP, port);


RUDPHeader* header = create_rudp_header();

size_t size = sizeof(header);
    
if(send_ACK(recievedSocket, header, data, size) == NULL){
    perror("ERROR SENDING ACK");
    return -1;
}


    
    printf("Sender connected, beginning to receive file...\n");

    int lifetime = 0;
    double total_time_ms = 0;
    double time_elapsed_ms;
    double time_elapsed_seconds;
    struct timeval time_start, time_end;
    double bandwidth;
    double total_bandwidth;
    double size_in_mb;
    char* statistics = malloc(128);
    char* file_stats;
    size_t new_length;
    //Program loop
    while(TRUE){

        // Create a buffer to store incoming data.
        char buffer[MAX_BUFFER_SIZE] = {0};

    
        *buffer = rudp_recv(recievedSocket, header, data, sizeof(data));


        // If the message receiving failed, print an error message and return 1.
        if (buffer == NULL){
            printf("ERROR: Failed recieving file size.\n");
            close(recievedSocket);
            exit(EXIT_FAILURE);
        }
        // save to file, and adjust pointer
        memcpy(&fptr2, buffer, sizeof(buffer));
        fseek(fptr2, sizeof(buffer),SEEK_CUR);


        //Update statistics
        gettimeofday(&time_end, NULL);
        lifetime++;
        time_elapsed_seconds = (time_end.tv_sec - time_start.tv_sec) + (time_end.tv_usec - time_start.tv_usec) / 1000000.0;
        time_elapsed_ms = (time_end.tv_sec - time_start.tv_sec)*1000 +(time_end.tv_usec - time_start.tv_usec)/1000;
        size_in_mb = (sizeof(buffer)/1024)/1024;
        bandwidth = size_in_mb/time_elapsed_seconds; 

        total_bandwidth += bandwidth;
        total_time_ms += (time_elapsed_ms);

        file_stats = (char *)malloc((128 + 1)); // +1 for the null terminator

        sprintf(file_stats, "- Run #%d Data: Time = %f ms; Speed = %f MB/s\n",lifetime,time_elapsed_ms,bandwidth);
        new_length = strlen(file_stats) + strlen(statistics);
        statistics = (char*)realloc(statistics, new_length);  // Reallocate memory for the new string
        if (statistics == NULL) {
            printf("Error: Memory allocation failed.\n");
            rudp_close(recievedSocket);
            exit(EXIT_FAILURE);
        }

        strcat(statistics, file_stats);  // Appending the two strings.
        RUDPHeader* header = get_header(buffer);
        if(header->FIN == 1){
            printf("DONE RECIEVING FILE)");
            break;
        }
            }   


    printf("----------------------------------\n");
    printf("-         * Statistics *         -\n");
    printf("%s\n", statistics);
    printf("- Average time: %f ms\n", total_time_ms/lifetime);
    printf("- Average bandwidth: %f MB/s\n", total_bandwidth/lifetime);
    printf("----------------------------------\n");
    printf("Receiver end.\n");
    rudp_close(recievedSocket);
    exit(EXIT_SUCCESS);
}