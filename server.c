#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main()
{
    WSADATA wsaData;
    int result;
    int version = MAKEWORD(2, 2);
    result = WSAStartup(version, &wsaData);

    if(result != 0)
    {
        printf("WSAStartup failed with error: %d\n", result);
        
        return 1;
    }
    

}