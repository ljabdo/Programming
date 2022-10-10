#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

#include "window.h"
#include "pdu.h"
#include "safeUtil.h"

#define MAXPDULEN 1407

int getRR();

int main(int argc, char *argv[])
{
    int rr;
    uint8_t pdu[MAXPDULEN];
    int seqNum = 0;
    int pduLen;
    createWindowTable(4);
    //printf("rr = %d", rr);

    while(1)
    {
        if (checkStatus()){
            printWindowData();
            pduLen = 20 + rand() % 80;
            createPDU(pdu, seqNum, 3,(uint8_t *) "hello", pduLen);
            addPdu(pdu, seqNum, 3, pduLen);
            seqNum++;
            }
            
        else{
            printf("\n");
            printWindowTable();
            printWindowData();
            printf("\n");
            rr = getRR();
            printf("\n");
            processRR(rr);
            printWindowTable();
            printf("\n");
        }
    }
}

int getRR()
{
    int rr;    
	
	// Important you don't input more characters than you have space 
	printf("Enter number to RR: ");
    if (!scanf("%d", &rr) > 0){
        printf("Scan error\n");
        exit(EXIT_FAILURE);
    }
	
	return rr;
}