#include "traceroute.h"

u_int16_t compute_icmp_checksum (const void *buff, int length) {
    u_int32_t sum;
    const u_int16_t* ptr = (const u_int16_t*) buff;
    assert (length % 2 == 0);
    for (sum = 0; length > 0; length -= 2)
    {
        sum += *ptr++;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    return (u_int16_t)(~(sum + (sum >> 16)));
}

int send(int sockfd, int process_id, int ttl, struct sockaddr_in address, int i) {
    struct icmphdr header;
    header.type = ICMP_ECHO;
    header.code = 0;
    header.un.echo.id = process_id;
    header.un.echo.sequence = (ttl - 1)*3 + i;
    header.checksum = 0;
    header.checksum = compute_icmp_checksum ((u_int16_t*)&header, sizeof(header));
    
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        throw runtime_error("setsockopt error");
    }
    if (sendto(sockfd, &header, sizeof(header), 0, (struct sockaddr*)&address, sizeof(address)) < 0) {
        throw runtime_error("sendto error");
    }
    return 1;
}

void send3(int sockfd, int ttl, int process_id, struct sockaddr_in address) {
    for(int i = 0; i < 3; i++) {
        send(sockfd, process_id, ttl, address, i);
    }
}