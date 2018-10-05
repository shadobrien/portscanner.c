#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define TCP_STR "TCP"
#define UDP_STR "UDP"

//initialize UDP socket
int init_sock_udp() {
	return socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

//initialize TCP socket
int init_sock_tcp() {
	return socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

//initialize ICMP socket
int init_sock_icmp() {
	return socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
}

//used raw ICMP socket to receive response from UDP probe
int get_icmp(int sock) {
	//set timeout values
	fd_set reset;
	struct timeval time;
	//set buffer
	char msg[100];
	
	time.tv_sec = 1;
	time.tv_usec = 0;
	
	//loop to wait for response
	while(1) {
		FD_ZERO(&reset);
		FD_SET(sock, &reset);
		
		//if timeout try again
		if(select(sock+1, &reset, NULL, NULL, &time) > 0) {
			recvfrom(sock, &msg, sizeof(msg), 0x0, NULL, NULL);
		}
		
		//finally received response
		else if(!FD_ISSET(sock, &reset)) {
			return 1;
		}
		
		else {
			//get ICMP packet back failed
			return 0;
		}
	}
}

//scanning function, takes in target ip, port, and protocol
void scan(char *in_ip, int in_port, char *in_protocol) {
	//grab fully qualified domain name to eventually grab formatted ip
	struct hostent *he;
	he = gethostbyname(in_ip);
	
	//setup parameters of sockaddr_in struct, fill in values with ip, port, and family
	struct sockaddr_in target_ip;
	bzero(&target_ip, sizeof(target_ip));
	target_ip.sin_family = AF_INET;
	target_ip.sin_port = htons(in_port);
	target_ip.sin_addr = *((struct in_addr*)he->h_addr);
	
	//initialize socket for sending (TCP only needs one) and receiving (UDP requires both)
	int out_sock;
	int in_sock;
	
	//go TCP route
	if(strcmp(in_protocol, TCP_STR) == 0) {
		//init TCP socket
		out_sock = init_sock_tcp();
		//if connection is successful, print out open
		if(connect(out_sock, (struct sockaddr*)&target_ip, sizeof(struct sockaddr)) == 0) {
			printf("\t%s/%d OPEN\n", in_protocol, in_port);
		}
	}
	
	//go UDP route
	else if(strcmp(in_protocol, UDP_STR) == 0) {
		//UDP requires sending a message through a UDP packet and listening for response on ICMP packet
		char msg[100];
		
		//init UDP socket to send out
		out_sock = init_sock_udp();
		//init TCMP socket to listen on
		in_sock = init_sock_icmp();
		
		//if sending UDP packet is successful
		if(sendto(out_sock, msg, sizeof(msg), 0, (struct sockaddr*)&target_ip, sizeof(target_ip)) >= 0) {
			//then listen for response
			if(get_icmp(in_sock) == 1) {
				//print out open
				printf("\t%s/%d OPEN\n", in_protocol, in_port);
			}
		}
	}
	
	//invalid protocol parameter
	else {
		printf("Please enter either TCP or UDP\n");
	}
	
	//clean up
	if(out_sock)
		close(out_sock);
	
	if(in_sock)
		close(in_sock);
	
}

int main(int argc, char **argv) {
	//take in user params
	if(argc != 5) {
		printf("portscanner.c usage:\n");
		printf("Please enter target in the following form:\n");
		printf("./portscanner <target IP> <beginning port> <ending port> <protocol>\n");
		printf("Example: ./portscanner 192.168.50.50 50 100 UDP\n");
		exit(-1);
	}
	
	else {
		//set params as variables
		char *target_host_ip = argv[1];
		int beg_host_port = atoi(argv[2]);
		int end_host_port = atoi(argv[3]);
		char *protocol = argv[4];
		
		//port counter
		int current_port;
		
		printf("** Scanning %s at %s ports %d - %d **\n", target_host_ip, protocol, beg_host_port, end_host_port);
		printf("** ... ... ... **\n");
		
		//scanning loop
		for(current_port = beg_host_port; current_port <= end_host_port; current_port++) {
			scan(target_host_ip, current_port, protocol);
		}
		
		printf("** Finished **\n");
	}
	
	return 0;
}
