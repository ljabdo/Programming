#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pcap/pcap.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "struct.h"
#include "checksum.h"
#include "funcs.h"

void printPacketInfo(int cur, u_int32_t length)
{
   printf("Packet number: %d Frame Len: %d\n\n", cur, length);
}

void printEthHeader(ethHeader * eth)
{
   struct ether_addr addr;
   char * temp;

   memcpy(addr.ether_addr_octet, eth->ethDest, MACSIZE);
   temp = ether_ntoa(&addr);

   printf("\tEthernet Header\n");
   printf("\t\tDest MAC: %s\n", temp);

   memcpy(addr.ether_addr_octet, eth->ethSrc, MACSIZE);
   temp = ether_ntoa(&addr);
   printf("\t\tSource MAC: %s\n", temp);

   temp = findEthType(eth);
   printf("\t\tType: %s\n\n", temp);
}

void printArp(arpHeader * arp)
{ 
   struct ether_addr addr;
   struct in_addr ipAddr;
   char * temp;

   printf("\n\tARP header\n");
   printf("\t\tOpcode: ");

   if (arp->op == htons(ARPREQUEST))
      printf("Request\n");
   else if (arp->op == htons(ARPREPLY))
      printf("Reply\n");

   memcpy(addr.ether_addr_octet, arp->sMAC, MACSIZE);
   temp = ether_ntoa(&addr);
   printf("\t\tSender MAC: %s\n", temp);

   memcpy(&(ipAddr.s_addr), arp->sIP, IPSIZE);
   temp = inet_ntoa(ipAddr);
   printf("\t\tSender IP: %s\n", temp);

   memcpy(addr.ether_addr_octet, arp->tMAC, MACSIZE);
   temp = ether_ntoa(&addr);
   printf("\t\tTarget MAC: %s\n", temp);

   memcpy(&(ipAddr.s_addr), arp->tIP, IPSIZE);
   temp = inet_ntoa(ipAddr);
   printf("\t\tTarget IP: %s\n\n", temp);
}

char * findProtocol(ipHeader *ip)
{
   if (ip->protocol == ICMP)
      return "ICMP"; 
   else if (ip->protocol == TCP)
      return "TCP";
   else if (ip->protocol == UDP)
      return "UDP";
   return "Unknown";
}

void printIP(ipHeader *ip, u_char * data)
{
   char * temp;
   struct in_addr ipAddr;

   printf("\tIP Header\n");
   printf("\t\tHeader Len: %hu (bytes)\n", ip->headerLen);
   printf("\t\tTOS: 0x%x\n", ip->TOS);
   printf("\t\tTTL: %hu\n", ip->TTL);
   printf("\t\tIP PDU Len: %hu (bytes)\n", ntohs(ip->PDUlen));
   printf("\t\tProtocol: %s\n", findProtocol(ip));
   printf("\t\tChecksum: ");

   if (in_cksum((short unsigned int *)data, ip->headerLen) == 0)
      printf("Correct (%#x)\n", ip->checkSum);
   else
      printf("Incorrect (%#x)\n", ip->checkSum);

   memcpy(&(ipAddr.s_addr), ip->sIP, IPSIZE);
   temp = inet_ntoa(ipAddr);
   printf("\t\tSender IP: %s\n", temp);

   memcpy(&(ipAddr.s_addr), ip->dIP, IPSIZE);
   temp = inet_ntoa(ipAddr);
   printf("\t\tDest IP: %s\n\n", temp);

}

int tcpPortPrint(uint16_t  port)
{
   if (port == HTTP)
      printf("HTTP\n");
   else if (port == TELNET)
      printf("TELNET\n");
   else if (port == FTP)
      printf("FTP\n");
   else if (port == POP3)
      printf("POP3\n");
   else if (port == SMTP)
      printf("SMTP\n");
   else
      return 1;
   return 0; 
}

void printTCPFlags(tcpHeader *tcp)
{
   printf("\t\tACK Flag: ");
   if(tcp->flags & 16)
      printf("Yes\n");
   else printf("No\n");

   printf("\t\tSYN Flag: ");
   if(tcp->flags & 2)
      printf("Yes\n");
   else printf("No\n");

   printf("\t\tRST Flag: ");
   if(tcp->flags & 4)
      printf("Yes\n");
   else printf("No\n");

   printf("\t\tFIN Flag: ");
   if(tcp->flags & 1)
      printf("Yes\n");
   else printf("No\n");
}

void TCPCheckSum(tcpHeader *tcp, u_char * data, ipHeader * ip, u_int32_t length)
{
   uint8_t psuedoH[PSUEDOHEADERSIZE];
   uint8_t reserve = 0;
   uint16_t totTcpLength = ntohs(ip->PDUlen) - ip->headerLen + PSUEDOHEADERSIZE;
   uint16_t tcpLength = ntohs(ip->PDUlen) - ip->headerLen;
   uint8_t finalHeader[totTcpLength];
   tcpLength = htons(tcpLength);

   memcpy(&psuedoH[0], ip->sIP, IPSIZE);
   memcpy(&psuedoH[IPSIZE], ip->dIP, IPSIZE);
   memcpy(&psuedoH[IPSIZE * 2], &reserve, 1);
   memcpy(&psuedoH[IPSIZE * 2 + 1], &ip->protocol, 1);
   memcpy(&psuedoH[IPSIZE * 2 + 2], &tcpLength, 2);

   memcpy(&finalHeader[0], &psuedoH, PSUEDOHEADERSIZE);
   memcpy(&finalHeader[PSUEDOHEADERSIZE], data, ntohs(tcpLength));

   if (in_cksum((short unsigned int *)finalHeader, totTcpLength) == 0)
      printf("Correct (%#x)\n", htons(tcp->checkSum));
   else
      printf("Incorrect (%#x)\n", htons(tcp->checkSum));
}

void printTCP(tcpHeader *tcp, u_char * data, ipHeader * ip, u_int32_t length)
{
   printf("\tTCP Header\n");
   printf("\t\tSource Port: ");

   if (tcpPortPrint(ntohs(tcp->sPort)))
      printf(": %hu\n", ntohs(tcp->sPort));

   printf("\t\tDest Port: ");
   if (tcpPortPrint(ntohs(tcp->dPort)))
      printf(": %hu\n", ntohs(tcp->dPort));

   printf("\t\tSequence Number: %u\n", ntohl(tcp->seqN));
   printf("\t\tACK Number: ");

   if (ntohl(tcp->ackN) == 0 || !(tcp->flags & 16))
      printf("<not valid>\n");
   else printf("%u\n", ntohl(tcp->ackN));

   printTCPFlags(tcp);

   printf("\t\tWindow Size: %u\n", ntohs(tcp->winSize));
   printf("\t\tChecksum: ");

   TCPCheckSum(tcp, data, ip, length);
}



