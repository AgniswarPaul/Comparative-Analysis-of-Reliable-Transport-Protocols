#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float TIMEOUT = 20.0;

// #define size_of_buffer 1000

static const float RTT = 64.0; // Figure out Later

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

struct msg buffer;

struct pkt
{
    int seqnum;
    int acknum;
    int checksum;
    char payload[20];
};

// struct pkt livepacket;

// struct msg message_last;
// struct msg acknowledgement_ack;
 

// struct packet_details{
//     struct pkt packet;
//     bool packet_sent;
//     bool acknowledgement;
// }

// struct packet_details *packetdata;

int build_checksum(struct pkt livepacket)
{
    int checksum = 0;
    checksum += livepacket.seqnum;
    checksum += livepacket.acknum;
    for (int i = 0; i < 20; ++i)
        checksum += packet.payload[i];
    return checksum;
}

bool ispacket_not_corrupt(struct pkt livepacket){
    if(livepacket.checksum == build_checksum(livepacket)){
        return true;
    }
    else{
        return false;
    }
        
}

void A_init() 
{
    size_of_window=N;

}

void messagetopacket(msg message, int seqnum, int acknum)
{

    pkt currentpacket = {};
    strncpy(currentpacket.payload, message.data, sizeof(currentpacket.payload));
    currentpacket.seqnum = seqnum;
    currentpacket.acknum = acknum;
    currentpacket.checksum = calc_checksum(currentpacket);
    tolayer3(0,currentpacket);

}



void udt_send() // change name
{
    while((seqnumnext < buffer.size()) && (seqnumnext < winbeginning + winsize))
    {
        messagetopacket(buffer[seqnumnext],seqnumnext,acknumA);

        if(winbeginning == seqnumnext)
            starttimer(0,RTT);
    seqnumnext++;    
    }
    
}


void A_output(struct msg message){

}


void A_timerinterrupt()
{
    A.A_is_ready_to_transmit = false;
    tolayer3(0, A.buffer_packet_A);
    starttimer(0, TIMEOUT);
}

void A_input(struct pkt packet) {
    if (ispacket_not_corrupt(packet)){
        if(packet.acknum == A.seq_A){
            stoptimer(0);
            A.seq_A = 1 - A.seq_A;
            A.A_is_ready_to_transmit = false;
        }
    }
    else{
        printf("packet is corrupted\n");
    }
 
}

void B_init(){
    B.seq_B = 0;
}

void B_input(struct pkt packet) {
    tolayer5(1,packet.payload);
    B.B.acknum=B.seq_B;
    B.B.checksum=get_checksum(packet);
    tolayer3(1,B.B);
    B.seq_B++;  
}

