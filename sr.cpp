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

float TIMEOUT = 38.0;


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
    float start;
    bool ackdone;
    char data[20];
};


vector<datapkt> bufferA;
deque<int> packettime;
map <int, pkt> bufferB;

int next_seq_num; 

struct HostA
{
    int seq_A;
    int ack_A;
    int base_number;
    int size_of_window;
} A;


struct HostB
{
    int seq_B;
    int size_of_window;
    int base_number;
} B;

struct pkt livepacket;


int build_checksum(struct pkt packet)
{
    int checksum = 0;
    checksum += packet.seqnum;
    checksum += packet.acknum;
    for (int i = 0; i < 20; ++i)
        checksum += packet.payload[i];
    return checksum;
}

bool is_packet_corrupt(struct pkt packet){
    if(packet.checksum == build_checksum(packet)){
        return false;
    }
    else{
        return true;
    }
        
}

void makeack(int acknum)
{
    pkt ackpacket = {};
    ackpacket.acknum = acknum;
    ackpacket.checksum = calc_checksum(ackpacket);
    tolayer3(1,ackpacket);
}

void msgpkt(datapkt message, int seqnum, int acknum)
{

    pkt currentpacket = {};
    strncpy(currentpacket.payload, message.data, sizeof(currentpacket.payload));
    currentpacket.seqnum = seqnum;
    currentpacket.acknum = acknum;
    currentpacket.checksum = calc_checksum(currentpacket);
    tolayer3(0,currentpacket);

}

void sr_send(int seqnum, bool sr)
{
    
    if((seqnum >= A.base_number) && (seqnum < A.base_number + A.size_of_window) && sr)
    {   
        messagetopacket(buffer[seqnum],seqnum,acknumA);
        buffer[seqnum].start = get_sim_time();
        packettime.push_back(seqnum);
        
        if(packettime.size() == 1)
        {
            starttimer(0,TIMEOUT);
        }        
    }
    
    else if ((next_seq_num >= A.base_number) && (next_seq_num <= A.base_number + A.size_of_window))
    {
        
        messagetopacket(buffer[next_seq_num],next_seq_num,A.ack_A); 
        buffer[next_seq_num].start = get_sim_time();
        packettime.push_back(next_seq_num);

        if(packettime.size() == 1)
        {
            
            starttimer(0,TIMEOUT);
        }
            

        next_seq_num++;
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
        tolayer5(1,pktdata);
        bufferB.erase(i); 
        B.base_number++;
        i = bufferB.find(B.base_number);
    }
    return B.base_number;

}

void A_output(struct msg message)
{
    float current_time = get_sim_time();
    datapkt currentmsg;
    currentmsg.ackdone = false;
    currentmsg.start = -1;
    strncpy(currentmsg.data,message.data,sizeof(message.data));
    buffer.push_back(currentmsg);
    sr_send(-1,false);

}

void A_input(struct pkt packet)
{
    if(!is_packet_corrupt(packet))
    {
         
        buffer[packet.acknum].ackdone = true;

        if(A.base_number == packet.acknum)
        {    
            while(buffer.size()>A.base_number && buffer[A.base_number].ackdone)
            {
                A.base_number++;
            } 
            while(next_seq_num < A.base_number + A.size_of_window && next_seq_num < buffer.size())
            {
                sr_send(-1,false); 
            }
        } 

        if(packettime.front() == packet.acknum)
        {    
            
            packettime.pop_front();
            stoptimer(0);

            while(packettime.size()>0 && packettime.size() <= A.size_of_window && buffer[packettime.front()].ackdone)
            {
                
                packettime.pop_front();
            }
            if(packettime.size()>0 && packettime.size()<= A.size_of_window)
            {
                float nextinterrupt = buffer[packettime.front()].start + TIMEOUT - get_sim_time();
                stoptimer(0);
                starttimer(0,nextinterrupt);
            }
        }

    }

}

void A_timerinterrupt()
{

    int seqnuminterrupted = packettime.front();
    packettime.pop_front();

    while(packettime.size() > 0 && packettime.size() <= A.size_of_window && buffer[packettime.front()].ackdone)
    {
        packettime.pop_front();
    }
    if(packettime.size()>0 && packettime.size() <= A.size_of_window)
    {   
        float nextinterrupt = buffer[packettime.front()].start + RTT - get_sim_time();
        starttimer(0,nextinterrupt);
    }
    
    udt_send(seqnuminterrupted,true);
} 

void A_init()
{
    A.seq_A = 0;
    A.ack_A = 0;
    A.size_of_window = getwinsize()/2;  
    next_seq_num = 0;
    A.base_number = 0;

}

void B_input(struct pkt packet)
{
    if(!is_packet_corrupt(packet) && (B.base_number == packet.seqnum))
    { 
        tolayer5(1,packet.payload);
        makeack(packet.seqnum);
        B.base_number++;

    }

    else if(!is_packet_corrupt(packet) && (B.base_number <= packet.seqnum) && (packet.seqnum < B.base_number + B.size_of_window) )
    {
        bufferB[packet.seqnum] = packet;
        makeack(packet.seqnum);
    }

    else if(!is_packet_corrupt(packet))
    {
        makeack(packet.seqnum);
    }
    
    
   B.base_number = sendbuffered(B.base_number);
}

void B_init()
{
    B.size_of_window = getwinsize()/2;
    B.base_number = 0;
}



