
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<cstring>
#include<queue>
#include<vector>
#include<stdbool.h>

using namespace std;
/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

struct HostA
{
    int seq_A;
    int ack_A;
    int next_seq_A;
    
    int next_buffer; 
    
    
} A;


struct HostB
{
    int seq_B;
    int ack_B;
    
} B;

struct pkt livepacket;

struct msg message;

int size_of_window, base_number;  
static const float RTT = 16.0; 

vector <msg> buffer;

// bool IsCorrupted(struct pkt packet);
// int build_checksum(pkt packet);
// void gbn_send();
// void messagetopacket(msg message, int seqnum, int acknum);
// void makeack(int acknum);



bool ispacket_not_corrupt(struct pkt packet){
    if(packet.checksum == build_checksum(packet)){
        return true;
    }
    else{
        return false;
    }
        
}

int build_checksum(struct pkt packet)
{
    int checksum = 0;
    checksum += livepacket.seqnum;
    checksum += livepacket.acknum;
    for (int i = 0; i < 20; ++i)
        checksum += packet.payload[i];
    return checksum;
}

// void messagetopacket(msg message, int seqnum, int acknum)
// {

//     pkt livepacket = {};
//     strncpy(livepacket.payload, message.data, sizeof(livepacket.payload));
//     livepacket.seqnum = seqnum;
//     livepacket.acknum = acknum;
//     livepacket.checksum = build_checksum(livepacket);
//     tolayer3(0,livepacket);

// }

void makeack(int acknum)
{
    pkt ackpacket = {};
    ackpacket.acknum = acknum;
    ackpacket.checksum = build_checksum(ackpacket);
    tolayer3(1,ackpacket);
}

void gbn_send()
{
    while((A.next_seq_A < buffer.size()) && (A.next_seq_A < A.base_number + A.size_of_window))
    {
        pkt livepacket = {};
        strncpy(livepacket.payload, message.data, sizeof(livepacket.payload));
        livepacket.seqnum = A.next_seq_A;
        livepacket.acknum = A.ack_A;
        livepacket.checksum = build_checksum(livepacket);
        tolayer3(0,livepacket);
        

        if(A.base_number == A.next_seq_A)
            starttimer(0,RTT);
    A.next_seq_A++;    
    }
    
}



/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	buffer.push_back(message);
    //sending the data to lower layer
    gbn_send();
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
    if(ispacket_not_corrupt(packet))
    {
        base_number = packet.acknum + 1;

        if(base_number == A.next_seq_A)
            stoptimer(0);
        else
        {
            stoptimer(0);
            starttimer(0, RTT);
        }
    }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	A.next_seq_A = base_number;
    gbn_send();
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    A.seq_A = 0;
    A.ack_A = 0;
    size_of_window = getwinsize();
    base_number = 0;
    A.next_seq_A = 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/




void B_input(struct pkt packet)
{
	if(ispacket_not_corrupt(packet) && (B.seq_B == packet.seqnum))
    	{
		tolayer5(1,packet.payload);
		makeack(B.seq_B);
        B.seq_B++;
    	}
}
    



/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
     B.seq_B = 0;
}