#include <stdio.h> 

float TIMEOUT = 20.0;

struct HostA
{
    int seq_A;
    int ack_A;
    bool A_is_ready_to_transmit;
    struct pkt buffer_pakcet_A;
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

int build_checksum(struct pkt packet)
{
    int checksum = 0;
    checksum = checksum + packet.sequence_number;
    checksum = checksum + packet.ackowledgement_number;
    for (int i = 0; i < 20; i = i+1)
        checksum = checksum + packet.payload[i];
    return checksum;
}

void A_output(struct msg message)
{
    if (A.A_is_ready_to_transmit)
    {
        A.A_is_ready_to_transmit = false;
        struct pkt packet;
        packet.sequence_number = A.seq_A;
        packet.ackowledgement_number = A.ack_A;
        strncpy(packet.payload, message.data, 20);
        packet.checksum = build_checksum(packet);
        A.buffer_packet_A = packet;
        tolayer3(0, packet);
        starttimer(0, TIMEOUT);
    }
    else
    {
        printf("Host A is not ready to receive any new message");
    }
}

void A_init()
{
    A.A_is_ready_to_transmit = true;
    A.seq_A = 0;
    A.ack_A = 0;
}

void B_init(){
    B.seq_B = 0;
}

void A_timerinterrupt()
{
    A.A_is_ready_to_transmit = false;
    tolayer3(0, A.buffer_packet_A);
    starttimer(0, TIMEOUT);
}

void A_input(struct pkt packet) {
    if (ispacket_not_corrupt(packet)){
        if(packet.ackowledgement_number == A.seq_A){
            stoptimer(0);
            A.seq_A = 1 - A.seq_A;
            A.A_is_ready_to_transmit = false;
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

void B_input(struct pkt packet) {
    char data[20];
    int ack_checksum;
    strncpy(data,packet.payload,20);
    ack_checksum = build_checksum(packet);
    struct pkt ack;
    if((ack_checksum == packet.checksum) && (packet.sequence_number == B.seq_B)){
        tolayer5(1,data);
        ack.acknum = B.seq_B;
        ack.checksum = build_checksum(packet);
        tolayer3(1,ack);
        B.seq_B = 1 - B.seq_B;
    }
    else{
        ack.ackowledgement_number = 1 - B.seq_B;
        ack.checksum = build_checksum(packet);
        tolayer3(1, ack);
    }
}

