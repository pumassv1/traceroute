#include "traceroute.h"

void traceroute(char **argv) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        throw runtime_error("socket error");
    }

    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient)); 
    recipient.sin_family = AF_INET; 

    if (inet_pton(AF_INET, argv[1], &recipient.sin_addr) == 0) {
       throw runtime_error("IP error");
    }

    int pid = getpid();

    cout << "traceroute to " << argv[1] << ", 30 hops max" << endl;
    for (int ttl = 1; ttl <= 30; ttl++) {
        struct timeval tvalBefore;
        gettimeofday (&tvalBefore, NULL);

        send3(sockfd, ttl, pid, recipient);

        if (receive(sockfd, ttl, pid, tvalBefore)) { break; }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        throw runtime_error("usage: sudo ./traceroute <ip adress>");
    }

    if (getuid()) {
        throw runtime_error("You must be root to run this code!");
    }

    traceroute(argv);

    return 0;
}