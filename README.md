# portscanner.c
Portscanner.c can scan for a range of ports on a single host. It uses either TCP or UDP.

To compile portscanner.c, run the following:
gcc portscanner.c -o portscanner -pthread -g -O1

To use portscanner, run the following:
./portscanner <target_IP> <beginning port> <ending port> <protocol>
  
Example command to scan 127.0.0.1 at ports 1 through 1023 using TCP:
./portscanner 127.0.0.1 1 1023 TCP
