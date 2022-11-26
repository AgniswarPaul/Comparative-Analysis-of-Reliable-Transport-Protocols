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

