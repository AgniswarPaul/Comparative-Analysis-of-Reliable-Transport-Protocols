#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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
} A;


struct HostB
{
    int seq_B;
} B;

struct pkt livepacket;

int size_of_window, base_of_window; 

int next_seq_num;
  
static const float RTT = 40.0; 

vector <msg> buffer;

int make_the_checksum(struct pkt packet)
{
    int checksum = 0;
    checksum += packet.seqnum;
    checksum += packet.acknum;
    for (int i = 0; i < 20; i++){
        checksum += packet.payload[i];
    }
    return checksum;
}

bool is_packet_corrupt(struct pkt packet){
    if(packet.checksum == make_the_checksum(packet)){
        return false;
    }
    else{
        return true;
    }
        
}


void packetmessage(msg message, int seqnum, int acknum)
{

    pkt livepacket = {};
    strncpy(livepacket.payload, message.data, sizeof(livepacket.payload));
    livepacket.seqnum = seqnum;
    livepacket.acknum = acknum;
    livepacket.checksum = make_the_checksum(livepacket);
    tolayer3(0,livepacket);

}


void makeack(int acknum)
{
    pkt ackpacket = {};
    ackpacket.acknum = acknum;
    ackpacket.checksum = make_the_checksum(ackpacket);
    tolayer3(1,ackpacket);
}


void usegbn()
{
    while((next_seq_num < buffer.size()) && (next_seq_num < base_of_window + size_of_window))
    {
        packetmessage(buffer[next_seq_num],next_seq_num,A.ack_A);
        

        if(base_of_window == next_seq_num)
            starttimer(0,RTT);
    next_seq_num++;    
    }
    
}


/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	buffer.push_back(message);
    	usegbn();
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
    if(!is_packet_corrupt(packet))
    {
        base_of_window = packet.acknum + 1;

        if(base_of_window == next_seq_num)
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
	next_seq_num = base_of_window;
    	usegbn();
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    A.seq_A = 0;
    A.ack_A = 0;
    size_of_window = getwinsize();
    base_of_window = 0;
    next_seq_num = 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/




void B_input(struct pkt packet)
{
	if(!is_packet_corrupt(packet) && (B.seq_B == packet.seqnum))
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