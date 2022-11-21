#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float TIMEOUT = 20.0;

struct HostA
{
    int seq_A;
    int ack_A;
    bool A_wait_ACK;
} A;

struct HostB
{
    int seq_B;
    int ack_B;
} B;

struct msg
{
    char data[20];
};

struct pkt
{
    int sequence_number;
    int ackowledgement_number;
    int checksum;
    char payload[20];
};

struct pkt packet_buffer[1000];
int first_packet = 0;
int last_packet = -1;

int build_checksum(struct pkt packet)
{
    int checksum = 0;
    checksum = checksum + packet.sequence_number;
    checksum = checksum + packet.ackowledgement_number;
    for (int i = 0; i < 20; i = i+1) {
        checksum = checksum + packet.payload[i];
    } 
    return checksum;
}

void A_init()
{
    A.A_wait_ACK = false;
    A.seq_A = 0;
    A.ack_A = 0;
}

void A_output(struct msg message)
{
    if (A.A_is_ready_to_transmit)
    {
        A.A_is_ready_to_transmit = false;
        struct pkt packet;
        packet.sequence_number = A.seq_num_of_A;
        packet.ackowledgement_number = A.ack_num_of_A;
        int i = 0;
        while (i < 20){
            packet.payload[i] = message.data[i];
            i++;
        }
        packet.checksum = build_checksum(packet);
        A.buffer_packet_A = packet; // doubt - in case of re-transmission??
        tolayer3(0, packet);
        starttimer(0, TIMEOUT);
    }
    else
    {
        printf("Host A is not ready to receive any new message and its waiting to receive an acknowledgement");
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
        if(packet.ackowledgement_number == A.seq_num_of_A){
            stoptimer(0);
            A.seq_num_of_A = 1 - A.seq_num_of_A;
            A.A_is_ready_to_transmit = true;
        }
    }
    else{
        printf("1)packet could be corrupted or 2)sequence and acknowledegement numbers don't match\n");
    }
 
}

bool ispacket_not_corrupt(struct pkt packet){
    if(packet.checksum == build_checksum(packet)){
        return true;
    }
    else{
        return false;
    }
        
}

void B_init(){
    B.seq_B = 0;
}


void B_input(struct pkt packet) {
    char data[20]; // we can remove this part. Right??
    int data_len = *(&data + 1) - data;
    int receiver_checksum;
    int i = 0;
    while (i < data_len){
            packet.payload[i] = data[i];
            i++;
    }
    receiver_checksum = build_checksum(packet); 
    struct pkt ack;
    if((receiver_checksum == packet.checksum) && (packet.sequence_number == B.seq_B)){
        tolayer5(1,data);
        ack.ackowledgement_number = B.seq_B;
        ack.checksum = build_checksum(packet);
        while (i < data_len){
            ack.payload[i] = data[i];
            i++;
    }
        tolayer3(1,ack);
        B.seq_B = 1 - B.seq_B; 
    }
    else{
        ack.ackowledgement_number = 1 - B.seq_B;
        ack.checksum = build_checksum(packet);
        tolayer3(1, ack);
    }
}