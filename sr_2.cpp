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

int seqnumA,acknumA,winsizeA,seqnumnext,baseA,seqnumB,winsizeB,baseB;


float RTT = 38.0; // should change and check

struct packetdata{
    float start;
    bool ackdone;
    char data[20];
};

bool IsCorrupted(struct pkt packet);
int calc_checksum(pkt packet);
void udt_send(int seqnum, bool sr);
void messagetopacket(packetdata message, int seqnum, int acknum);
void makeack(int acknum);
int sendbuffered(int baseB);

vector<packetdata> buffer;
deque<int> packettime;
map <int, pkt> bufferB;

bool IsCorrupted(struct pkt packet)
{

    if(packet.checksum == calc_checksum(packet))
    {
        return false;
        //cout<<" not corrupt"<<endl;
    }
        
    else
        return true;
}


int calc_checksum(struct pkt packet)
{
//computes checksum
int checksum = packet.seqnum + packet.acknum;
for(int i =0; i<20;i++)
{
  checksum+=packet.payload[i];
}
return checksum;
}

void messagetopacket(packetdata message, int seqnum, int acknum)
{

    pkt currentpacket = {};
    strncpy(currentpacket.payload, message.data, sizeof(currentpacket.payload));
    currentpacket.seqnum = seqnum;
    currentpacket.acknum = acknum;
    currentpacket.checksum = calc_checksum(currentpacket);
    tolayer3(0,currentpacket);

}

void makeack(int acknum)
{
    pkt ackpacket = {};
    ackpacket.acknum = acknum;
    ackpacket.checksum = calc_checksum(ackpacket);
    tolayer3(1,ackpacket);
}

void udt_send(int seqnum, bool sr)
{
    
    // if the packets have to be resent
    if((seqnum >= baseA) && (seqnum < baseA + winsizeA) && sr)
    {   
        messagetopacket(buffer[seqnum],seqnum,acknumA);
        buffer[seqnum].start = get_sim_time();
        packettime.push_back(seqnum);
        
        if(packettime.size() == 1)
        {
            starttimer(0,RTT);
        }        
    }
    // sending packets in sender window for the first time
    else if ((seqnumnext >= baseA) && (seqnumnext <= baseA + winsizeA))
    {
        
        messagetopacket(buffer[seqnumnext],seqnumnext,acknumA); 
        buffer[seqnumnext].start = get_sim_time();
        packettime.push_back(seqnumnext);

        if(packettime.size() == 1)
        {
            
            starttimer(0,RTT);
        }
            

        seqnumnext++;
    }
     
    
}

int sendbuffered(int baseB)
{

    map <int, pkt> :: iterator i;
    i = bufferB.find(baseB);

    while(i!=bufferB.end())
    {   // retrieving data from the buffer, sending to layer 5, till the end of the buffer at receiver side
        char pktdata[20];
        pkt packetinbuffer = i->second; // dereferencing the iterator, to get the packet in the buffer
        //payload of the buffered packet sent to layer 5
        strncpy(pktdata,packetinbuffer.payload, sizeof(pktdata));
        tolayer5(1,pktdata);
        bufferB.erase(i);
        //updating baseB after removing
        baseB++;
        i = bufferB.find(baseB);
    }
    return baseB;

}

void A_output(struct msg message)
{
    float timenow = get_sim_time();
    //cout<<"At A_output : current time :"<<timenow<<"current base "<<baseA<<endl;
    packetdata currentmsg;
    currentmsg.ackdone = false;
    currentmsg.start = -1;
    strncpy(currentmsg.data,message.data,sizeof(message.data));
     //adding message to buffer
    buffer.push_back(currentmsg);

    //sending the data to lower layer
    udt_send(-1,false);

}


void A_input(struct pkt packet)
{
    if(!IsCorrupted(packet))
    {
         
        buffer[packet.acknum].ackdone = true;

        if(baseA == packet.acknum)
        {    
            while(buffer.size()>baseA && buffer[baseA].ackdone)
            {
                baseA++;
            } 
            while(seqnumnext < baseA + winsizeA && seqnumnext < buffer.size())
            {
                udt_send(-1,false); 
            }
        } 

        if(packettime.front() == packet.acknum)
        {    
            
            packettime.pop_front();
            stoptimer(0);

            while(packettime.size()>0 && packettime.size() <= winsizeA && buffer[packettime.front()].ackdone)
            {
                
                packettime.pop_front();
            }
            if(packettime.size()>0 && packettime.size()<= winsizeA)
            {
                float nextinterrupt = buffer[packettime.front()].start + RTT - get_sim_time();
                stoptimer(0);
                starttimer(0,nextinterrupt);
            }
        }

    }

}

void A_timerinterrupt()
{   // as all acknowledged packets are popped from the packettime vector, the interrupted packet will be at the front
    int seqnuminterrupted = packettime.front();
    packettime.pop_front();

    //all the acknowledged packets within the sender window is popped 
    while(packettime.size() > 0 && packettime.size() <= winsizeA && buffer[packettime.front()].ackdone)
    {
        packettime.pop_front();
    }
    // for unacked packets in the window
    if(packettime.size()>0 && packettime.size() <= winsizeA)
    {   //starting the timer
        float nextinterrupt = buffer[packettime.front()].start + RTT - get_sim_time();
        starttimer(0,nextinterrupt);
    }
    //sending the packets which have not been acked in the window
    udt_send(seqnuminterrupted,true);
}  


void A_init()
{
    seqnumA = 0;
    acknumA = 0;
    winsizeA = getwinsize()/2; // the window size divided 
    seqnumnext = 0;
    baseA = 0;

}


void B_input(struct pkt packet)
{
    if(!IsCorrupted(packet) && (baseB == packet.seqnum))
    {// If the sequence number is base and the packet is not corrupted, it is sent to layer 5 
        tolayer5(1,packet.payload);
        makeack(packet.seqnum);
        // base is updated once the packet is sent to layer 5 and acknowledgement is also sent
        baseB++;

    }

    else if(!IsCorrupted(packet) && (baseB <= packet.seqnum) && (packet.seqnum < baseB + winsizeB) )
    {
        // if the sequence number is greater than the base, and falls within the window on receiver side, the packet is buffered
        bufferB[packet.seqnum] = packet;
        makeack(packet.seqnum);
    }

    else if(!IsCorrupted(packet))
    {
        //if the packet is not corrupted, but the sequence number does not fall within the receiver window, acknowledgment is sent, packet is not buffered 
        makeack(packet.seqnum);
    }
    
    // sending the buffered packets and updating the receiver side base
   baseB = sendbuffered(baseB);
}



/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
    winsizeB = getwinsize()/2;
    baseB = 0;
}