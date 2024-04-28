#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "RUDP.h"



#define MAX_BUFFER_SIZE 1024
#define CHECKSUM_LEN 16

typedef struct{
    uint16_t seq;
    uint16_t checksum;
    uint16_t length;
    unsigned int ACK: 1;
    unsigned int FIN: 1;
} RUDPHeader;

typedef struct{
    int sockFD;
    struct sockaddr_in sender_addr;
} RUDPSocket;

// creating an RUDP socket
RUDPSocket* create_rudp_socket(char* ip, int port){
    RUDPSocket *rudp_socket = (RUDPSocket*)malloc(sizeof(RUDPSocket));
    if (rudp_socket == NULL){
        printf("ERROR CREATING SOCKET");
        rudp_close(rudp_socket);
        return NULL;
    }
    
    if((rudp_socket->sockFD = socket(AF_INET, SOCK_DGRAM, 0) <0)){
        printf("NOT ABLE TO CREATE SOCKET");
        rudp_close(rudp_socket);
        return NULL;
    }


    // creating the actuall address
    memset(&rudp_socket->sender_addr, 0, sizeof(rudp_socket->sender_addr));
    rudp_socket->sender_addr.sin_family = AF_INET;
    rudp_socket->sender_addr.sin_port = htons(port);
    
    if(inet_pton(AF_INET, ip, &rudp_socket->sender_addr.sin_addr) <=0){
        printf("IP ADDRESS ERROR");
        //////////////////////////////////close here!///

        rudp_close(rudp_socket);
        return NULL;
    }
    
    return rudp_socket;

}


RUDPHeader* createACK(RUDPHeader *header){
    header->ACK = 1;
    return header;
}

int create_handshake(RUDPHeader *header, RUDPSocket *socket){
    header->ACK = 1;
    char data = NULL;
    if (rudp_send(socket, header, data, sizeof(header) < 0 )){
        printf("NO HANDSHAKE");
        return -1;
    }
    return 1;
}

RUDPHeader* create_rudp_header(){
    RUDPHeader* header = (RUDPHeader*)malloc(sizeof(RUDPHeader));
    if (header == NULL){
        printf("ERROR CREATING HEADER");
        return NULL;
    }
    header->length = MAX_BUFFER_SIZE;
    header->ACK = 0;
    header->FIN = 0;
    return header;
}


int handshake(char* ipSender, int portSender){
    RUDPSocket *socket = create_rudp_socket(ipSender, portSender); 
    RUDPHeader *header = create_rudp_header();
    if(socket != NULL && header != NULL){
        /// CHECK WHAT TO DO
    }
}


// sends data to peer. wait for an acknowledgment packet -- if not : sends again 
char* rudp_send(RUDPSocket *socket, RUDPHeader *header, char* data, size_t data_size){
    char buffer[MAX_BUFFER_SIZE];
    if(sizeof(data) > sizeof(header)){
         header->seq += 1;
    }
    //checksum calculation function here
    header->checksum = checksum_sender(MAX_BUFFER_SIZE);


    memcpy(buffer, data, data_size);
    memcpy(buffer+ data_size, header, sizeof(header));
    
    ssize_t sent_bytes;
    socklen_t addr_len = sizeof(socket->sender_addr);


    if( sent_bytes = sendto(socket->sockFD, buffer, MAX_BUFFER_SIZE, 0, (struct sockadrr *)&socket->sender_addr, &addr_len) <0){
        printf("Wasn't able to send");
        return NULL;
    }
    return buffer;
}

//recieves data from peer
char* rudp_recv(RUDPSocket *socket, RUDPHeader *header, char* data, size_t data_size){
    uint16_t old_seq = header->checksum;
    char buffer[MAX_BUFFER_SIZE] = {0};
    socklen_t addr_len = sizeof(socket->sender_addr);
    ssize_t recieved_bytes;
    if(recieved_bytes = recvfrom(socket->sockFD, buffer, MAX_BUFFER_SIZE, 0, (struct sockadrr *)&socket->sender_addr, &addr_len) < 0){
        printf("ERROR RECIEVING");
        return NULL;
    }
    
    RUDPHeader* header = get_header(buffer);
    // checks if all is well
    if(header->checksum == checksum_sender(buffer)  && (header->seq = old_seq + 1)){
        return buffer;
    } // GOT ALL PACKTS SUCCESSFULY
    else{
        return NULL;
    } // lost packets
}


u_int16_t checksum_sender(char buffer[MAX_BUFFER_SIZE]){
    int checksum, sum = 0,  i;
    int mone = 0;
    mone += 1;
    for(i=0; i<= MAX_BUFFER_SIZE; i++){
        if(buffer[i] == 10){
            mone ++;
        }
    }
    checksum =~mone;
    return checksum;
}


char* send_ACK(RUDPSocket *socket, RUDPHeader *header, char* data, size_t data_size){
    header->seq = 0;
    header->ACK = 1;
    char buffer[MAX_BUFFER_SIZE];
    *buffer = rudp_send(socket, header, data, data_size);
    return buffer;
}


char* send_FIN(RUDPSocket *socket, RUDPHeader *header, char* data, size_t data_size){
    header->seq += 1;
    header->ACK = 1;
    header->FIN = 1;
    char buffer[MAX_BUFFER_SIZE];
    *buffer = rudp_send(socket, header, data, data_size);
    return buffer;
}


RUDPHeader* get_header(buffer){
    RUDPHeader* header = malloc(sizeof(RUDPHeader));
    memcpy(&header, buffer, sizeof(header));
    header->seq= ntohs(header);
    header->checksum = ntohs(header);
    header->length = ntohs(header);
    return (RUDPHeader*)header;
}

//closes a connection
void rudp_close(RUDPSocket* socket){
    if(socket != NULL){
        close(socket->sockFD);
        free(socket);
    }
}

int recieved_ack(RUDPHeader* header){
    if (header->ACK == 1){
        return 1;
    }
    return 0;
}

int correct_seq(RUDPHeader* header, uint16_t correct){
    if( header->seq == correct){
        return 1;
    }
    return 0;
}