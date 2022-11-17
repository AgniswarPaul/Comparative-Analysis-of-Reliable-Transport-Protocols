#include <stdio.h>

struct msg
{
    char data[20];
    int data_len;
} message;

int main()
{
    message.data_len = *(&message.data + 1) - message.data;
    printf("Length: %d\n", message.data_len );

    return 0;
}

//////////////////////////////////////////////////////////////////////
struct msg
{
    char data[20];
    int data_len;
} message;

int main()
{
    message.data_len = *(&message.data + 1) - message.data;
    int payload[20];
    int checksum = 0;
    for (int i=0; i<message.data_len; i=i+1){
        checksum = checksum + payload[i];
    }
    printf("Length: %d\n", checksum );

    return checksum;
}

//////////////////////////////////////////////////////////////////////
