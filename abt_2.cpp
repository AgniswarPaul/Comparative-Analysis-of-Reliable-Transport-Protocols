#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float TIMEOUT = 20.0;

struct HostA
{
    int seq_A;
    int ack_A;
    bool A_ready_to_transmit;
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

queue<msg> buffer;

struct pkt
{
    int seqnum;
    int acknum;
    int checksum;
    char payload[20];
};


int build_checksum(struct pkt packet)
{
    int checksum = 0;
    checksum = checksum + packet.seqnum;
    checksum = checksum + packet.acknum;
    for (int i = 0; i < 20; i = i+1) {
        checksum = checksum + packet.payload[i];
    } 
    return checksum;
}

void A_init()
{
    A.A_ready_to_transmit = true;
    A.seq_A = 0;
    A.ack_A = 0;
}

void A_output(struct msg message)
{
    if (A.A_ready_to_transmit)
    {
        A.A_ready_to_transmit = false;
        struct pkt packet;
        packet.seqnum = A.seq_A;
        packet.acknum = A.ack_A;
        int i = 0;
        while (i < 20){
            packet.payload[i] = message.data[i];
            i++;
        }
        packet.checksum = build_checksum(packet);
        tolayer3(0, packet);
        starttimer(0, TIMEOUT);
    }
    else
    {
        printf("Host A is not ready to receive any new message and its waiting to receive an acknowledgement");
        buffer.push(message);
    }
}


void A_timerinterrupt()
{
    A.A_ready_to_transmit = true;
    tolayer3(0, packet_buffer[first_packet]);
    starttimer(0, TIMEOUT);
}


void A_input(struct pkt packet) {
    if (ispacket_not_corrupt(packet)){
        if(packet.acknum == A.seq_num_of_A){
            stoptimer(0);
            A.seq_A = 1 - A.seq_A;
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
    if((receiver_checksum == packet.checksum) && (packet.seqnum == B.seq_B)){
        tolayer5(1,data);
        ack.acknum = B.seq_B;
        ack.checksum = build_checksum(packet);
        while (i < data_len){
            ack.payload[i] = data[i];
            i++;
    }
        tolayer3(1,ack);
        B.seq_B = 1 - B.seq_B; 
    }
    else{
        ack.acknum = 1 - B.seq_B;
        ack.checksum = build_checksum(packet);
        tolayer3(1, ack);
    }
}