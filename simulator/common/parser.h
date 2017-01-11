#ifndef PARSER_FILE
#define PARSER_FILE
typedef struct {
    int custID;
    char serverIP[30]; // Same for Ping and SSL server
    int protocol;
    int sslPerSec;
} jsonData_t;
#endif
