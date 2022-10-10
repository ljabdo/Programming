#ifndef STRUCT_H
#define STRUCT_H

#define ETHSIZE 14
#define MACSIZE 6
#define TYPESIZE 2
#define OPSIZE 2
#define IPSIZE 4

#define ARP 0x0806
#define IP 0x0800

#define ARPID 0
#define ARPREQUEST 0x0001
#define ARPREPLY 0x0002
#define ARPBYTESTART 20

#define IPID 1

#define IPHEADBYTELEN 1
#define TOSLEN 2
#define TTLLEN 8
#define IPPROTOCOLLEN 9
#define IPCHECKSUMLEN 10
#define IPSENDERIP 12
#define IPDESTIP 16

#define ICMP 1
#define TCP 6
#define UDP 17

#define ICMPREPLY 0
#define ICMPREQUEST 8

#define PORTSIZE 2

#define TCPDPORT 2
#define TCPSEQNUM 4
#define TCPACKNUM 8
#define TCPHEADERLEN 12
#define TCPFLAG 13
#define TCPWINSIZE 14
#define TCPCHECKSUM 16

#define HTTP 80
#define TELNET 23
#define FTP 21
#define POP3 110
#define SMTP 25

#define PSUEDOHEADERSIZE 12

typedef struct
{
   uint8_t ethFrame[ETHSIZE];
   uint8_t ethDest[MACSIZE];
   uint8_t ethSrc[MACSIZE];
   uint16_t type;
} __attribute__((packed)) ethHeader;

typedef struct
{
   uint16_t op;
   uint8_t sMAC[MACSIZE];
   uint8_t sIP[IPSIZE];
   uint8_t tMAC[MACSIZE];
   uint8_t tIP[IPSIZE];
} __attribute__((packed)) arpHeader;

typedef struct
{
   uint8_t headerLen;
   uint8_t TTL;
   uint8_t TOS;
   uint16_t PDUlen;
   uint8_t protocol;
   uint16_t checkSum;
   uint8_t sIP[IPSIZE];
   uint8_t dIP[IPSIZE];
} __attribute__((packed)) ipHeader;

typedef struct
{
   uint16_t sPort;
   uint16_t dPort;
   uint32_t seqN;
   uint32_t ackN;
   uint8_t headerLen;
   uint8_t flags;
   uint16_t winSize;
   uint16_t checkSum;
} __attribute__((packed)) tcpHeader;

#endif
