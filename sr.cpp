#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <deque>
#include <map>
#include <unistd.h>

using namespace std;

float TIMEOUT = 60.0;


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

struct datapkt
{
    float simtime;
    bool ackdone;
    char data[20];
};


vector<datapkt> buffer;
deque<int> packettime;
map <int, pkt> bufferB;

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
    int size_of_window;
    int base_number;
    struct pkt B;
} B;


vector<datapkt> buffer;
deque<int> packettimer;
map <int, pkt> bufferB;

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

void udt_send(int seqnum, bool sr) // change name
{
    if((seqnum >= A.base_number) && (seqnum < A.base_number + A.size_of_window) && sr)
    {
        pkt livepacket = {};
        int i=0;
        while (i < 20){
            livepacket.payload[i] = message.data[i]; // Need to change into a different function
            i++;
        }
        livepacket.seqnum = seqnum;
        livepacket.acknum = A.ack_A;
        livepacket.checksum = build_checksum(livepacket);
        tolayer3(0,livepacket);
        buffer[seqnum].simtime = get_sim_time();
        packettimer.push_back(seqnum);

        if(packettimer.size() == 1)
        {
            starttimer(0,TIMEOUT);
        }
        
    else if ((A.next_seq_A >= A.base_number) && (A.next_seq_A <= A.base_number + A.size_of_window))
    {
        
        messagetopacket(buffer[A.next_seq_A],A.next_seq_A,A.ack_A);
        pkt livepacket = {};
        int i=0;
        while (i < 20){
            livepacket.payload[i] = message.data[i];
            i++;
        }
        livepacket.seqnum = A.next_seq_A;
        livepacket.acknum = A.ack_A;
        livepacket.checksum = build_checksum(livepacket);
        tolayer3(0,livepacket); 
        buffer[A.next_seq_A].start = get_sim_time();
        packettime.push_back(A.next_seq_A);

        if(packettime.size() == 1)
        {
            
            starttimer(0,TIMEOUT);
        }
            

        A.next_seq_A++;
    }


   
    }
    
}

int bufferatB(int B.base_number)
{

    map <int, pkt> :: iterator i;
    i = bufferB.find(B.base_number);

    while(i!=bufferB.end())
    {   
        char pktdata[20];
        pkt packetinbuffer = i->second; 
        strncpy(pktdata,packetinbuffer.payload, sizeof(pktdata));
        int i=0;
        while (i < 20){
            packetinbuffer.payload[i] = pktdata[i];
            i++;
        }
        tolayer5(1,pktdata);
        bufferB.erase(i); 
        B.base_number++;
        i = bufferB.find(B.base_number);
    }
    return B.base_number;

}

void A_output(struct msg message){
    buffer.push_back(message);
    udt_send();

}


