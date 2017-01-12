#include "stdio.h"
#include <netinet/in.h>
#include <arpa/inet.h> // for inet_ntoa
#include <stdlib.h> // for exit
#include <fcntl.h>
#include <errno.h>
#include "../common/log.h"
#include "../common/parser.h"

struct bgp_t {
	int sock;
	struct sockaddr_in server_addr;
	struct sockaddr_in routerID;
};
#define BGP_MARKER_SIZE                 16      /* size of BGP marker */
/* BGP message types */
#define BGP_OPEN                1
#define BGP_UPDATE              2
#define BGP_NOTIFICATION        3
#define BGP_KEEPALIVE           4
#define BGP_ROUTE_REFRESH       5
#define BGP_CAPABILITY          6
#define BGP_TCP_PORT 179

typedef unsigned char guint8;
typedef unsigned char uchar;
typedef unsigned short guint16;
typedef unsigned int guint32;

/* BGP OPEN message */
struct bgp_open {
    guint8 bgpo_marker[BGP_MARKER_SIZE];
    guint16 bgpo_len;
    guint8 bgpo_type;
    guint8 bgpo_version;
    guint16 bgpo_myas;
    guint16 bgpo_holdtime;
    guint32 bgpo_id;
    guint8 bgpo_optlen;
    /* options should follow */
};

jsonData_t* parse (FILE *flog, char* configFile);
jsonData_t* jsonData;
struct bgp_t bgpStruct;
struct bgp_t *bgp = &bgpStruct;

sendBgpData (FILE* fp, uchar *ptr, int length) {
    int sent;

    log_debug(fp, "BGP: SendData: Sending %d Bytes", length); fflush(stdout);
    sent = sendto(bgp->sock, ptr, length, 0,
            (struct sockaddr*)&bgp->server_addr, sizeof(bgp->server_addr));
    if(sent == -1) {
            perror(" - send error: ");
    } else {
            log_debug(fp, " :%d Bytes", sent);
    }
    fflush(fp);
}


initConnectionToPeer(FILE *fp) {
    int arg, err;

    if((bgp->sock=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket:");
            log_error(fp, "BGP ERROR: socket creation failed"); fflush(fp);
            exit(1);
    }
    bgp->server_addr.sin_family = AF_INET;
    bgp->server_addr.sin_port = htons(BGP_TCP_PORT);
    if(inet_aton(jsonData->serverIP, &bgp->server_addr.sin_addr) == 0) {
        log_error(fp, "BGP ERROR: create in inet_aton"); fflush(fp);
	exit(1);
    }
    log_info(fp, "BGP: Connect to %s", jsonData->serverIP); fflush(fp);
        // Set non-blocking
        if( (arg = fcntl(bgp->sock, F_GETFL, NULL)) < 0) {
                perror("F_GETFL:");
                exit(0);
        }
        arg |= O_NONBLOCK;
        if( fcntl(bgp->sock, F_SETFL, arg) < 0) {
                perror("F_SETFL:");
                exit(0);
        }
    err = connect(bgp->sock, (struct sockaddr *)&bgp->server_addr,
                sizeof(struct sockaddr));
        if (errno != EINPROGRESS) {
                // Note: connect on blocking socket returns EINPROGRESS
        log_error(fp, "BGP ERROR: create connecting to server"); fflush(fp);
        perror("Connect Error:");
        exit(1);
    } else {
                int result;
                socklen_t result_len = sizeof(result);
                if (getsockopt(bgp->sock, SOL_SOCKET, SO_ERROR,
                                &result, &result_len) < 0) {
                        // error, fail somehow, close socket
                        return;
                }
                if (result != 0) {
                        // connection failed; error code is in 'result'
                        return;
                }
                // socket is ready for read()/write()
                //log_info(fp, "TCP Connected.."); fflush(fp);
        }
    log_info(fp, "BGP TCP connection created to %s, sock:%d",
        jsonData->serverIP, bgp->sock);
    fflush(fp);
}

sendOpen(FILE *fp, int sock) {
  struct bgp_open open;
        int i;

        log_info(fp, "BGP: Send OPEN"); fflush(stdout);
        memset(open.bgpo_marker, 0xFF, 16);
        open.bgpo_len = htons(29);
        open.bgpo_type = BGP_OPEN;
        open.bgpo_version = 4; // jsonData->version;
        open.bgpo_myas = htons(jsonData->myAS);
        open.bgpo_holdtime = 0;

    if(inet_aton(jsonData->myRouterID, &bgp->routerID.sin_addr) == 0) {
                log_error(fp, "BGP: inet_aton failed");
                exit(1);
        }
        log_info(fp, "BGP self router ID = %x", bgp->routerID.sin_addr.s_addr);
        open.bgpo_id = bgp->routerID.sin_addr.s_addr;
        open.bgpo_optlen = 0;

        sendBgpData(fp, (uchar*)&open, 29);
}

main() {
	FILE *fp;
        fp = fopen("/root/ameet/simulator/logs.txt", "a+");
	if (fp == NULL)
	{
		printf ("could not open logfile\n");
		exit(1);
	}

	jsonData = parse(fp, NULL);
	initConnectionToPeer(fp);
	sendOpen(fp, bgp->sock);
	//cliLoop();
}

	
