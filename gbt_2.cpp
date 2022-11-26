#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <queue>
#include <vector>

float TIMEOUT = 20.0;

static const float RTT = 64.0; 

struct HostA
{
    int seq_A;
    int ack_A;
    int next_seq_A;
    int base_number;
    int next_buffer; 
    int size_of_window;
    struct pkt buffer_pakcet_A[size_of_buffer];
} A;


struct HostB
{
    int seq_B;
    int ack_B;
    struct pkt B;
} B;

struct msg
{
    char data[20];
};

vector <msg> buffer; 

struct pkt
{
    int seqnum;
    int acknum;
    int checksum;
    char payload[20];
};


int build_checksum(struct pkt livepacket)
{
    int checksum = 0;
    checksum += livepacket.seqnum;
    checksum += livepacket.acknum;
    for (int i = 0; i < 20; ++i)
        checksum += packet.payload[i];
    return checksum;
}

bool ispacket_not_corrupt(struct pkt receivepacket){
    if(receivepacket.checksum == build_checksum(receivepacket)){
        return true;
    }
    else{
        return false;
    }
        
}

void A_init() 
{
    A.seq_A=0;
    A.ack_A=0;
    A.size_of_window=getwinsize();
    A.base_number=0;
    A.next_seq_A=0;

}


void gbn_send() // change name
{
    while((A.next_seq_A < buffer.size()) && (A.next_seq_A < A.base_number + A.size_of_window))
    {
        pkt livepacket = {};
        struct msg message;
        int i=0;
        while (i < 20){
            livepacket.payload[i] = message.data[i];  // Need to change into a different function
            i++;
        }
        livepacket.seqnum = A.next_seq_A;
        livepacket.acknum = A.ack_A;
        livepacket.checksum = build_checksum(livepacket);
        tolayer3(0,livepacket);
        

        if(A.base_number == A.next_seq_A)
            starttimer(0,RTT);
    A.next_seq_A++;    
    }
    
}


void A_output(struct msg message){
    buffer.push_back(message);
    gbn_send();

}


void A_timerinterrupt()
{
    A.next_seq_A=A.base_number;
    gbn_send();
}

void A_input(struct pkt receivepacket) {
    if (ispacket_not_corrupt(receivepacket)){
        A.base_number=receivepacket.acknum+1;
        if (A.base_number==A.next_seq_A){
            stoptimer(0);
        }
        else{
            stoptimer(0);
            starttimer(0, RTT);
        }
        
    }
    
}

void B_init(){
    B.seq_B = 0;
}

struct pkt prev_ack;

void B_input(struct pkt receivepacket) {

    if(ispacket_not_corrupt(receivepacket)==false && B.seq_B==receivepacket.seqnum){
        tolayer5(1,receivepacket.payload);
        pkt ackpacket = {};
        ackpacket.acknum = B.seq_B;
        ackpacket.checksum = build_checksum(ackpacket);
        prev_ack=ackpacket;
        tolayer3(1,ackpacket);

        B.seq_B++;
    }
    
    else{
         tolayer3(1,prev_ack);
    } 
}

