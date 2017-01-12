#ifndef PARSER_FILE
#define PARSER_FILE
typedef struct {
    int custID;
    char serverIP[30]; // Same for Ping and SSL server
    int protocol;
    int myAS;
    char myRouterID[30];
    int myKeepalive;
    char myPassword[30];
    int myMaxPath;
    int peerAS;
    char peerRouterID[30];
    int peerKeepalive;
    char peerPassword[30];
    int peerMaxPath;
} jsonData_t;
#endif
