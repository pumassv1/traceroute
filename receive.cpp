#include "traceroute.h"

packet_info get_packet(int sockfd,  struct timeval tvalBefore) {
    struct timeval tvalAfter;
    gettimeofday (&tvalAfter, NULL);
    packet_info packet = {NULL, -1, -1, -1, -1, false};

    struct sockaddr_in 	sender;
    socklen_t sender_len = sizeof(sender);
    u_int8_t buffer[IP_MAXPACKET];
    if (recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len) < 0) {
        return packet;
    }

    char sender_ip_str[20]; 
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

    struct iphdr *ip_header = (struct iphdr *)buffer;
    u_int8_t* icmp_packet = buffer + 4 * ip_header->ihl;
    struct icmphdr *icmp_header = (struct icmphdr*) icmp_packet;

    if (ICMP_TIME_EXCEEDED == icmp_header->type) {
        u_int8_t *icmp_packet = buffer + 4 * ip_header->ihl + 8;
        icmp_packet = icmp_packet + 4 * ((struct ip *)(icmp_packet))->ip_hl;
        struct icmp *icmp_te = (struct icmp *)icmp_packet;

        packet = {sender_ip_str,
                  icmp_te->icmp_id,
                  1 + (icmp_te->icmp_seq)/3,
                  (icmp_te->icmp_seq),
                  (tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000 + tvalAfter.tv_usec - tvalBefore.tv_usec,
                  false};
        return packet;
    }
    else if (ICMP_ECHOREPLY == icmp_header->type) {
        packet = {sender_ip_str,
                  icmp_header->un.echo.id,
                  1 + (icmp_header->un.echo.sequence)/3,
                  icmp_header->un.echo.sequence,
                  (tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000 + tvalAfter.tv_usec - tvalBefore.tv_usec,
                  true};
        return packet;
    }
    else { return packet; }
}

int receive(int sockfd, int ttl, int process_id, struct timeval tvalBefore) {
    packet_info packet_n = {NULL, -1, -1, -1, -1, false};
    packet_info packets[3]; 
    packets[0]=packet_n; packets[1]=packet_n; packets[2]=packet_n;

    packet_info packet;
    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET (sockfd, &descriptors);
    struct timeval tv; tv.tv_sec = 1; tv.tv_usec = 0;

    int ready;
    int packets_received = 0;
    while (packets_received < 3) {

        ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);
        if (ready == 0) { break; } // timeout
        else if (ready < 0) { break; } // error
        else {
            packet = get_packet(sockfd, tvalBefore);
            
            if ((packet.id == process_id) && (packet.ttl == ttl) && ((packet.seq < 3*ttl) && (packet.seq >= 3*ttl-3))) {
                packets[packets_received] = packet;
                packets_received++;
            }
        }
    }

    if (packets_received == 0) {
        cout << ttl << ". * * *" << endl; 
        return 0;
    }
    else if (packets_received == 3) {
        int time = (packets[0].time + packets[1].time + packets[2].time)/3000;
        cout << ttl << ". " << packets[0].ip;
        if ((packets[1].ip !=  packets[0].ip) && (packets[1].ip !=  packets[2].ip))
            cout << "  " << packets[1].ip;
        if ((packets[2].ip !=  packets[0].ip) && (packets[2].ip !=  packets[1].ip)) 
            cout << "  " << packets[2].ip;
    
        cout << "  " << time << "ms" <<endl; 

        if (packets[0].dest) {
            return 1;
        } else {
            return 0;
        }
    }
    else if (packets_received < 3) {
        cout << ttl << ". " << packets[0].ip;
        if (( packets_received == 2 ) && (packets[1].ip !=  packets[0].ip))
            { cout << "  " << packets[1].ip; }
        cout << "  ??" <<endl;

        if (packets[0].dest) {
            return 1;
        } else {
            return 0;
        }
    }
    else {
        throw runtime_error("receive error");
    }
    
}