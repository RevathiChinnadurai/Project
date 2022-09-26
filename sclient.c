
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#define PORT 20002
#define Busname (50)
#define Bustype (50)
#define Buscity (50)
#define userinsert (Busname + Buscity + Bustype)
// info structure
struct
{
    char bname[Busname];
    char btype[Bustype];
    char bcity[Buscity];

} Bus;
int main()
{
    // Initiate the Socket environment
    WSADATA w;
    int nRet = 0;

    struct sockaddr_in srv;

    nRet = WSAStartup(MAKEWORD(2, 2), &w);
    if (nRet < 0)
    {
        printf("\nCannot Initialize socket lib");
        return -1;
    }
    // Open a socket - listener
    int nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSocket < 0)
    {
        printf("\nCannot Initialize listener socket:%d");
        return -1;
    }
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    srv.sin_port = htons(PORT);
    memset(&(srv.sin_zero), 0, 8);

    nRet = connect(nSocket, (struct sockaddr *)&srv, sizeof(srv));
    if (nRet < 0)
    {
        perror("\nCannot connect to server");
        return -1;
    }
    printf("Socket Connected Successfully");

    int option;
    char c;
    int shift;
    int continu;
    int response;
    do
    {
        printf("\n 1. NEW BUS REGISTRATION");
        printf("\n 2. DISPLAY BUS DETAILS");
        printf("\n 3. DELETE THE BUS");
        printf("\n 4. UPDATE EXISTING BUS");
        printf("\n 5. EXIT");
        printf("\nEnter your choice to be performmed\t:");
        scanf("%d", &option);
        send(nSocket, (const char *)&option, sizeof(option), 0);
        struct Bus;
        switch (option)
        {
        case 1:

            printf("Please enter the (%d) bus name:", Busname);
            scanf("%s", Bus.bname);
            printf("Please enter the (%d) bus type:", Bustype);
            scanf("%s", Bus.btype);
            printf("Please enter the (%d) city name:", Buscity);
            scanf("%s", Bus.bcity);
            send(nSocket, (const char *)&Bus, sizeof(Bus), 0);
            printf("\nInformation sent successfully\n");
            recv(nSocket, (void *)&response, sizeof(response), 0);
            if (response == 1)
            {
                printf("\n Response from server :  Records added successfully\n");
            }
            else if (response == -1)
            {
                printf("\n Response from server : Records cannot added...\n");
            }

            printf("\nPress 1 to continue, press 2 to exit:\n");
            scanf("%d", &shift);
            send(nSocket, (const char *)&shift, sizeof(shift), 0);
            if (shift == 1)
            {
                break;
            }
            else if (shift == 2)
            {
                continu = 0;
                break;
            }
        case 2:
            printf("\n Enter the Busname to be selected:");
            scanf("%s", Bus.bname);
            send(nSocket, (const char *)&Bus, sizeof(Bus), 0);
            printf("\nInformation sent successfully\n");
            recv(nSocket, (void *)&Bus, sizeof(Bus), 0);
            printf("%10s | %10s |%10s\n", "Busname", "Bustype", "City");
            recv(nSocket, (void *)&response, sizeof(response), 0);
            printf("%10s | %10s | %10s\n", Bus.bname, Bus.btype, Bus.bcity);
            if (response == 1)
            {
                printf("\n Response from server :  Records selected successfully\n");
            }
            else if (response == -1)
            {
                printf("\n Response from server : Records cannot selected...\n");
            }

            printf("\nPress 1 to continue, press 2 to exit:\n");
            scanf("%d", &shift);
            send(nSocket, (const char *)&shift, sizeof(shift), 0);
            if (shift == 1)
            {
                break;
            }
            else if (shift == 2)
            {
                continu = 0;
                break;
            }

            break;
        case 3:
            printf("\n Enter the Busname to be deleted:");
            scanf("%s", Bus.bname);
            send(nSocket, (const char *)&Bus, sizeof(Bus), 0);
            printf("\nInformation sent successfully\n");
            recv(nSocket, (void *)&response, sizeof(response), 0);
            if (response == 1)
            {
                printf("\n Response from server :  Records deleted successfully\n");
            }
            else if (response == -1)
            {
                printf("\n Response from server : Records cannot be deleted...\n");
            }

            printf("\nPress 1 to continue, press 2 to exit:\n");
            scanf("%d", &shift);
            send(nSocket, (const char *)&shift, sizeof(shift), 0);
            if (shift == 1)
            {
                break;
            }
            else if (shift == 2)
            {
                continu = 0;
                break;
            }

        case 4:
            printf("\n Enter the Busname to be Updated:");
            scanf("%s", Bus.bname);
            printf("\n Enter the new bus type :");
            scanf("%s", Bus.btype);
            printf("\n Enter the new city :");
            scanf("%s", Bus.bcity);
            send(nSocket, (const char *)&Bus, sizeof(Bus), 0);
            printf("\nInformation sent successfully\n");
            recv(nSocket, (void *)&response, sizeof(response), 0);
            if (response == 1)
            {
                printf("\n Response from server :  Records Updated successfully\n");
            }
            else if (response == -1)
            {
                printf("\n Response from server : Records cannot be updated...\n");
            }

            printf("\nPress 1 to continue, press 2 to exit:\n");
            scanf("%d", &shift);
            send(nSocket, (const char *)&shift, sizeof(shift), 0);
            if (shift == 1)
            {
                break;
            }
            else if (shift == 2)
            {
                continu = 0;
                break;
            }
        case 5:
            exit(0);
            break;

        default:
        {
            printf("\nEnter valid choice:\n");
            continu = 0;
            break;
        }
        }
    } while (continu != 0);
    closesocket(nSocket);
}