#ifndef GUARD_H
#define GUARD_H

#include <iostream>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h> 
#include <arpa/inet.h> 
#include <sys/time.h> 
#include <cassert>
#include <stdint.h> 
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h> 

using namespace std;

pid_t getpid(void);

typedef struct packet_info {
    char *ip;
    int id;
    int ttl;
    int seq;
    long int time;
    bool dest;
} packet_info;

// Send
u_int16_t compute_icmp_checksum(const void *buff, int length);
int send(int sockfd, int ttl, int process_id, struct sockaddr_in address);
void send3(int sockfd, int ttl, int process_id, struct sockaddr_in address);

// Receive
packet_info get_packet(int sockfd, struct timeval);
int receive(int sockfd, int ttl, int process_id, struct timeval start_time);

#endif
