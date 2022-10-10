#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pcap/pcap.h>
#include "checksum.h"
#include <netinet/ether.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "struct.h"
#include "funcs.h"

int main(int argc, char *argv[])
{
   checkArgs(argc, argv);
   printEth(argv[1]);
   return 0;
}

void checkArgs(int argc, char*argv[])
{
   if (argc != 2){
      fprintf(stderr, "Usage: trace [filename.pcap]\n");
      exit(EXIT_FAILURE);
   }
}

char * findEthType(ethHeader * eth)
{
   if(eth->type == htons(ARP)){
      eth->type = ARPID;
      return "ARP";
   }

   if(eth->type == htons(IP)){
      eth->type = IPID;
      return "IP";
   }
   return "None";
}


void fillEthHeader(u_char * data, ethHeader* eth)
{
   memcpy(eth->ethDest, data, MACSIZE);
   memcpy(eth->ethSrc, data + MACSIZE, MACSIZE);
   memcpy(&(eth->type), data + (2 * MACSIZE), TYPESIZE);
}

void printEth(char* file)
{
   pcap_t *pcap;
   struct pcap_pkthdr* header;
   ethHeader eth;
   u_char *data;
   int cur = 1;

   pcap = openPacket(file);

   while(pcap_next_ex(pcap, &header, (const u_char **)&data) == 1){
      printPacketInfo(cur, header->caplen);
      fillEthHeader(data, &eth);
      printEthHeader(&eth);
      headerArpIP(eth.type, data, header->caplen);
      cur++;
   }
}

void processARP(u_char * data)
{
   arpHeader arp;
   memcpy(&(arp.op), data, OPSIZE);
   memcpy(arp.sMAC, data + OPSIZE, MACSIZE);
   memcpy(arp.sIP, data + OPSIZE + MACSIZE, IPSIZE);
   memcpy(arp.tMAC, data + OPSIZE + MACSIZE + IPSIZE, MACSIZE);
   memcpy(arp.tIP, data + OPSIZE + 2 * MACSIZE + IPSIZE, IPSIZE);
   printArp(&arp);
}

void fillIP(ipHeader *ip, u_char * data)
{
   uint8_t tempLen;
   memcpy(&tempLen, data, 1);
   ip->headerLen = (tempLen & 0x0F) * 4;

   memcpy(&ip->TOS, data + IPHEADBYTELEN, 1);

   memcpy(&ip->PDUlen, data + TOSLEN, 2);

   memcpy(&ip->TTL, data + TTLLEN, 1);

   memcpy(&ip->protocol, data + IPPROTOCOLLEN, 1);

   memcpy(&ip->checkSum, data + IPCHECKSUMLEN, 2);

   memcpy(&ip->sIP, data + IPSENDERIP, 4);

   memcpy(&ip->dIP, data + IPDESTIP, 4);
}

void runICMP(u_char * data)
{
   uint8_t type;
   char * temp;
   memcpy(&type, data, 2);

   printf("\tICMP Header\n");
   printf("\t\tType: ");
   if (type != ICMPREPLY && type != ICMPREQUEST){
      printf("%hu\n", type);
      return;
   }

   else if (type == ICMPREPLY)
      temp = "Reply";

   else if (type == ICMPREQUEST)
      temp = "Request";

   printf("%s\n", temp);
}

void runTCP(u_char * data, ipHeader * ip, u_int32_t length)
{
   tcpHeader tcp;
   uint8_t tempLen;

   memcpy(&(tcp.sPort), data, PORTSIZE);

   memcpy(&(tcp.dPort), data + TCPDPORT, PORTSIZE);

   memcpy(&(tcp.seqN), data + TCPSEQNUM, 4);

   memcpy(&(tcp.ackN), data + TCPACKNUM, 4);

   memcpy(&tempLen, data + TCPHEADERLEN, 1);

   memcpy(&(tcp.flags), data + TCPFLAG, 1);

   memcpy(&(tcp.winSize), data + TCPWINSIZE, 2);

   memcpy(&(tcp.checkSum), data + TCPCHECKSUM, 2);

   printTCP(&tcp, data, ip, length);
}

void runUDP(u_char * data)
{
   uint16_t sourcePort;
   uint16_t destPort;
   memcpy(&sourcePort, data, 2);
   memcpy(&destPort, data + PORTSIZE, 2);
   printf("\tUDP Header\n");
   printf("\t\tSource Port: : %hu\n", ntohs(sourcePort));
   printf("\t\tDest Port: : %hu\n\n", ntohs(destPort));
}


void runProtocol(u_char * data, uint8_t protocol, ipHeader * ip, u_int32_t length)
{
   if (protocol == ICMP)
      runICMP(data);
   else if (protocol == TCP)
      runTCP(data, ip, length);
   if (protocol == UDP)
      runUDP(data);
}

void processIP(u_char * data, u_int32_t length)
{
   ipHeader ip;
   fillIP(&ip, data);
   printIP(&ip, data);
   data = data + ip.headerLen;
   runProtocol(data, ip.protocol, &ip, length);
}

void headerArpIP(uint16_t type, u_char * data, u_int32_t length)
{
   if (type == ARPID)
      processARP(data + ARPBYTESTART);
   else if (type == IPID)
      processIP(data + ETHSIZE, length); 
}

struct pcap_pkthdr * checkNextPacket(pcap_t *pcap, struct pcap_pkthdr* header, u_char *data)
{
   if (pcap_next_ex(pcap, &header,(const u_char **) &data) != 1){
      fprintf(stderr, "Next packet could not be read");
      exit(EXIT_FAILURE);
   }
   printf("packet found\n");
   return header;
}

pcap_t* openPacket(char* file)
{
   pcap_t *pcap;
   char errbuf[PCAP_ERRBUF_SIZE];

   if ((pcap = pcap_open_offline(file, errbuf)) == NULL){
      fprintf(stderr, "pcap buffer error\n");
      exit(EXIT_FAILURE);
   }
   return pcap;
}


