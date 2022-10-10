#ifndef FUNCS_H
#define FUNCS_H

void checkArgs(int argc, char*argv[]);

void printEth(char* file);

pcap_t* openPacket(char* file);

struct pcap_pkthdr * checkNextPacket(pcap_t *pcap, struct pcap_pkthdr* header, u_int8_t *data);

void printPacketInfo(int cur, bpf_u_int32 length);

void fillEthHeader(u_char * data, ethHeader* eth);

void printEthHeader(ethHeader * eth);

char * findEthType(ethHeader * eth);

void headerArpIP(uint16_t type, u_char * data, u_int32_t length);

void processARP(u_char * data);

void processIP(u_char * data, u_int32_t length);

void fillIP(ipHeader *ip, u_char * data);

void runICMP(u_char * data);

void runTCP(u_char * data, ipHeader * ip, u_int32_t length);

void runUDP(u_char * data);

void runProtocol(u_char * data, uint8_t pro, ipHeader * ip, u_int32_t length);

void printArp(arpHeader * arp);

char * findProtocol(ipHeader *ip);

void printIP(ipHeader *ip, u_char * data);

int tcpPortPrint(uint16_t  port);

void printTCPFlags(tcpHeader *tcp);

void TCPCheckSum(tcpHeader *tcp, u_char * data, ipHeader * ip, u_int32_t length);

void printTCP(tcpHeader *tcp, u_char * data, ipHeader * ip, u_int32_t length);

#endif
