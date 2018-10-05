# portscanner.c
Portscanner.c can scan for running services on a range of ports on a single host. It uses either TCP or UDP.

To compile portscanner.c, run the following:<br/>
gcc portscanner.c -o portscanner

To use portscanner, run the following:<br/>
./portscanner (target_IP) (beginning port) (ending port) (protocol)
  
Example command to scan 127.0.0.1 at ports 1 through 1023 using TCP:<br/>
./portscanner 127.0.0.1 1 1023 TCP<br/>

Note: you may have to run as root.
