#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float TIMEOUT = 20.0;

int next_position;
int next_seq_num=0;
int base_val;
int window_size;
int request_seq_num;
int buffer=0;
int sequence_number = 0;
int last_packet_sent = 0;


struct msg message_last;
struct msg acknowledgement_ack;
 

struct packet_details{
    struct pkt packet;
    bool packet_sent;
    bool acknowledgement;
}

struct packet_details *packetdata;


void A_output(struct msg message){
    struct pkt packet;
    packet.seqnum = A.seq_A;
    packet.acknum = 0;
    strncpy(packet.payload,message.data, 20);
    packet.checksum = get_checksum(packet);
    packetdata[next_position].packet = packet;
    packetdata[next_position].packet_sent = false;
    packetdata[next_position].acknowledgement = false;
    if(next_seq_num < base_val + window_size){
        if(next_seq_num == base_val){
            starttimer(0, TIMEOUT);
        }
        if(packetdata[next_seq_num].packet_sent == false && packetdata[next_seq_num].acknowledgement == false){
            tolayer3(0, packetdata[next_seq_num].packet);
            
        }
    }

    next_seq_num++;
    next_position++;
    A.seq_A++;

}







struct HostA
{
    int seq_A;
    int ack_A;
    int next_seq_A;
    int base_number;
    int next_buffer; 
    int size_of_window;
    struct pkt buffer_pakcet_A;
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

struct pkt
{
    int seqnum;
    int acknum;
    int checksum;
    char payload[20];
};

int get_checksum(struct pkt packet)
{
    int checksum = 0;
    checksum += packet.seqnum;
    checksum += packet.acknum;
    for (int i = 0; i < 20; ++i)
        checksum += packet.payload[i];
    return checksum;
}

void A_init() 
{
    size_of_window=N;

}

void A_output(struct msg message)
{
    if (A.A_is_ready_to_transmit)
    {
        struct pkt packet;
        packet.sequence_number = A.seq_A;
        packet.ackowledgement_number = A.ack_A;
        strncpy(packet.payload, message.data, 20);
        packet.checksum = get_checksum(packet);
        A.buffer_packet_A = packet;
        tolayer3(0, packet);
        starttimer(0, TIMEOUT);
    }
    else
    {
        printf("Host A is not ready to receive any new message");
    }
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

