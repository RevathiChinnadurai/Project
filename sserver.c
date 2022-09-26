#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"
#include <stdlib.h>
#define PORT 20002
#define Busname (50)
#define Bustype (50)
#define Buscity (50)
#define userinsert (Busname + Buscity + Bustype)
struct Bus
{
    char bname[Busname];
    char btype[Bustype];
    char bcity[Buscity];
};
typedef struct Bus Bus;
static int callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    fprintf(stderr, "%s:\n ", (const char *)data);
    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
}
int create(sqlite3 *db);
int insert(sqlite3 *db, Bus *Bus);
int selectbus(sqlite3 *db, Bus *Bus);
int delete (sqlite3 *db, Bus *Bus);
int update(sqlite3 *db, Bus *Bus);
int main()
{

    // Initiate the Socket environment
    WSADATA w;
    int nRet = 0;

    struct sockaddr_in srv;

    nRet = WSAStartup(MAKEWORD(2, 2), &w);
    if (nRet < 0)
    {
        printf("\nCannot Initialize socket library..");
        return -1;
    }
    // Open a socket - listener
    int nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSocket < 0)
    {
        printf("\nCannot Initialize listener socket:%d");
        return -1;
    }
    else
    {
        printf("\n socket created");
    }
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);
    memset(&(srv.sin_zero), 0, 8);

    nRet = bind(nSocket, (struct sockaddr *)&srv, sizeof(srv));
    if (nRet < 0)
    {
        perror("\nCannot bind at port:");
        return -1;
    }
    else
    {
        printf("\n Bound Successfully...");
    }

    nRet = listen(nSocket, 5);
    if (nRet < 0)
    {
        printf("\nCannot listen at port:%d");
        return -1;
    }
    printf("\n Listening....");

    int nClient = 0;
    int addrlen = sizeof(srv);
    nClient = accept(nSocket, (struct sockaddr *)&srv, &addrlen);

    if (nRet < 0)
    {
        printf("\nCannot accept client at port:%d");
        return -1;
    }
    printf("\n Connection Accepted");
    int create(sqlite3 * db);
    sqlite3 *db;
    int rc;
    // Database open
    rc = sqlite3_open("Busdb.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database:%s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    create(db);
    int option;
    char c;
    int continu;
    int shift;
    do
    {
        recv(nClient, (void *)&option, sizeof(option), 0);
        struct Bus Bus;
        int response;
        switch (option)
        {
        case 1:
            recv(nClient, (void *)&Bus, sizeof(Bus), 0);

            printf("Records received...: %s %s  %s\n", Bus.bname, Bus.btype, Bus.bcity);

            insert(db, &Bus);
            if (response == 1)
            {
                send(nClient, (const char *)&response, sizeof(response), 0);
            }

            else if (response == -1)
            {
                send(nClient, (const char *)&response, sizeof(response), 0);
            }
            recv(nClient, (void *)&shift, sizeof(shift), 0);
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
            recv(nClient, (void *)&Bus, sizeof(Bus), 0);
            selectbus(db, &Bus);
            printf("%10s | %10s | %10s\n", Bus.bname, Bus.btype, Bus.bcity);
            send(nClient, (const char *)&Bus, sizeof(Bus), 0);
            if (response == 1)
            {

                send(nClient, (const char *)&response, sizeof(response), 0);
                send(nClient, (const char *)&Bus, sizeof(Bus), 0);
            }

            else if (response == -1)
            {
                send(nClient, (const char *)&response, sizeof(response), 0);
            }
            recv(nClient, (void *)&shift, sizeof(shift), 0);
            if (shift == 1)
            {
                break;
            }
            else if (shift == 2)
            {
                continu = 0;
                break;
            }

        case 3:
            recv(nClient, (void *)&Bus, sizeof(Bus), 0);
            delete (db, &Bus);
            if (response == 1)
            {
                send(nClient, (const char *)&response, sizeof(response), 0);
            }

            else if (response == -1)
            {
                send(nClient, (const char *)&response, sizeof(response), 0);
            }
            recv(nClient, (void *)&shift, sizeof(shift), 0);
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
            recv(nClient, (void *)&Bus, sizeof(Bus), 0);
            update(db, &Bus);
            if (response == 1)
            {
                send(nClient, (const char *)&response, sizeof(response), 0);
            }

            else if (response == -1)
            {
                send(nClient, (const char *)&response, sizeof(response), 0);
            }
            recv(nClient, (void *)&shift, sizeof(shift), 0);
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
            printf("\nInvalid choice");
            break;
        }
    } while (continu != 0);
    sqlite3_close(db);
    closesocket(nSocket);
    closesocket(nClient);
    putc('\n', stdout);

    return 0;
}

int create(sqlite3 *db)
{
    sqlite3_stmt *statement;
    int rc;
    char *errmsg = 0;

    const char *sqlcreate = " CREATE TABLE IF NOT EXISTS bust("
                            "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                            "BUSNAME TEXT NOT NULL,"
                            "TYPE TEXT NOT NULL,"
                            "CITY TEXT NOT NULL);";
    if (sqlite3_exec(db, sqlcreate, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        fprintf(stderr, "\n cannot create table:%s\n", sqlite3_errmsg(db));
        return -1;
    }
    fprintf(stdout, "\n Table created successfully... \n");
    return 0;
}
int insert(sqlite3 *db, Bus *Bus)
{
    int response;
    sqlite3_stmt *statement;
    int rc;
    char sqlinsert[userinsert];
    sprintf(sqlinsert, "INSERT INTO bust(BUSNAME,TYPE,CITY) VALUES ('%s','%s','%s')",
            Bus->bname,
            Bus->btype,
            Bus->bcity);
    rc = sqlite3_prepare_v2(db, sqlinsert, strlen(sqlinsert), &statement, NULL);
    if (rc == SQLITE_OK)
    {
        rc = sqlite3_step(statement);
        response = 1;
        printf("\n Inserted successfully...");
    }
    else
    {
        fprintf(stderr, "Cannot insert the bus:'%s':%s\n", Bus->bname, sqlite3_errmsg(db));
        response = -1;
        return -1;
    }
    sqlite3_finalize(statement);
    return 0;
}
int selectbus(sqlite3 *db, Bus *Bus)
{
    int response;
    sqlite3_stmt *statement;
    int rc;
    char sqlinsert[userinsert];
    sprintf(sqlinsert, "SELECT * FROM bust WHERE BUSNAME = '%s'",
            Bus->bname);
    rc = sqlite3_prepare_v2(db, sqlinsert, strlen(sqlinsert), &statement, NULL);
    if (rc == SQLITE_OK)
    {
        rc = sqlite3_step(statement);
        strcpy(Bus->bname, (const char *)sqlite3_column_text(statement, 1));
        strcpy(Bus->btype, (const char *)sqlite3_column_text(statement, 2));
        strcpy(Bus->bcity, (const char *)sqlite3_column_text(statement, 3));
        printf("\n Selected successfully...");
        response = 1;
    }
    else
    {
        fprintf(stderr, "Cannot Select the bus:'%s':%s\n", Bus->bname, sqlite3_errmsg(db));
        response = -1;
        return -1;
    }
    sqlite3_finalize(statement);
    return 0;
}

int delete (sqlite3 *db, Bus *Bus)
{
    int response;
    sqlite3_stmt *statement;
    int rc;
    char sqlinsert[userinsert];
    sprintf(sqlinsert, "DELETE FROM bust WHERE BUSNAME = '%s'",
            Bus->bname);
    rc = sqlite3_prepare_v2(db, sqlinsert, strlen(sqlinsert), &statement, NULL);
    if (rc == SQLITE_OK)
    {
        rc = sqlite3_step(statement);
        response = 1;
        printf("\n Deleted successfully");
        
    }
    else
    {
        fprintf(stderr, "Cannot delete the bus:'%s':%s\n", Bus->bname, sqlite3_errmsg(db));
        response = -1;
        return -1;
    }
    sqlite3_finalize(statement);
    return 0;
}
int update(sqlite3 *db, Bus *Bus)
{
    int response;
    sqlite3_stmt *statement;
    int rc;
    char sqlinsert[userinsert];
    sprintf(sqlinsert, "UPDATE bust SET TYPE='%s',CITY='%s' WHERE BUSNAME = '%s'",
            Bus->btype, Bus->bcity, Bus->bname);
    rc = sqlite3_prepare_v2(db, sqlinsert, strlen(sqlinsert), &statement, NULL);
    if (rc == SQLITE_OK)
    {
        rc = sqlite3_step(statement);
        response = 1;
        printf("\n Updated successfully");
    }
    else
    {
        fprintf(stderr, "Cannot Update the bus:'%s':%s\n", Bus->bname, sqlite3_errmsg(db));
        response = -1;
        return -1;
    }
    sqlite3_finalize(statement);
    return 0;
}