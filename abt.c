#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


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

float round_trip_time =20.0;

struct HostA
{
    int seq_num_of_A;
    int ack_num_of_A;
    bool A_ready_to_transmit;
} A;

struct HostB
{
    int seq_num_of_B;
    
} B;

queue<msg> buffer;

struct pkt livepacket;


int make_the_checksum(struct pkt packet)
{
    int checksum = 0;
    checksum = checksum + packet.seqnum;
    checksum = checksum + packet.acknum;
    for (int i = 0; i < 20; i = i+1) {
        checksum = checksum + packet.payload[i];
    } 
    return checksum;
}

bool ispacket_not_corrupt(struct pkt packet)
{
    if(packet.checksum == make_the_checksum(packet)){
        return true;
    }
    else{
        return false;
    }
        
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	if (A.A_ready_to_transmit)
    {
        A.A_ready_to_transmit = false;
        livepacket = {};
        livepacket.seqnum = A.seq_num_of_A;
        livepacket.acknum = A.ack_num_of_A;
        int i = 0;
        while (i < 20){
            livepacket.payload[i] = message.data[i];
            i++;
        }
        livepacket.checksum = make_the_checksum(livepacket);
        tolayer3(0, livepacket);
        starttimer(0, round_trip_time);
    }
    else
    {
        printf("Host A is not ready to receive any new message and its waiting to receive an acknowledgement");
        buffer.push(message);
    }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	if (ispacket_not_corrupt(packet) && packet.acknum == A.seq_num_of_A){
		    stoptimer(0);
		    A.seq_num_of_A = 1 - A.seq_num_of_A;
		    if(!buffer.empty())
		    {
		        struct msg next_message = buffer.front();
		        buffer.pop();
		        A.A_ready_to_transmit = true;
		        A_output(next_message);	
		    }
		    else{
		        A.A_ready_to_transmit = true;  // In case buffer is empty
		    }
        
    }
    else{
        printf("1)packet could be corrupted or 2)sequence and acknowledegement numbers don't match\n");
    }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	A.A_ready_to_transmit = false;
    	tolayer3(0, livepacket);
    	starttimer(0, round_trip_time);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	A.A_ready_to_transmit = true;
    	A.seq_num_of_A = 0;
    	A.ack_num_of_A = 0;
}




/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    pkt livepacket = {};
    if(ispacket_not_corrupt(packet) && (packet.seqnum==B.seq_num_of_B))
    {
        tolayer5(1,packet.payload);
        livepacket.acknum = B.seq_num_of_B;
        livepacket.checksum = make_the_checksum(livepacket);
        tolayer3(1,livepacket);
        B.seq_num_of_B = 1-B.seq_num_of_B;
    }
    else{
        livepacket.acknum = 1-B.seq_num_of_B;
        livepacket.checksum = make_the_checksum(packet);
        tolayer3(1, livepacket);
    }
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	B.seq_num_of_B = 0;
}