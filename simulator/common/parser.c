#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "../jsmn/jsmn.h"
#include "parser.h"

//int cliLoop(void);

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

/*
 * configFile takes presendence. If this is NULL, then id is used
 * to pick up the file at /var/monT/100/config.json
 */
jsonData_t* parse (FILE *flog, char* configFile) {
	jsmn_parser p;
	jsmntok_t *tok;
	size_t tokcount = 256;
	int i, r, len, tlen, ret;
#define BUFFSIZE 8048
	char buff[BUFFSIZE];
	FILE *fp;
	char filePath[100];
	jsonData_t* jsonData;
	jsmntok_t *t;

	// jsonData is returnd to the caller, that needs to free it
	jsonData = malloc(sizeof(jsonData_t));
	if (jsonData == NULL) {
		log_debug(flog, "\n Malloc failure while alocation jsonData");
		return NULL;
	}
	strcpy(filePath, "config.json");

	log_debug(flog, "Opening Customer Config File: %s", filePath);
	fp = fopen(filePath, "r");
	if (fp == NULL) {
		log_error(flog, "Config file not present..");
		fflush(flog);
		return NULL;
	}

	len = (int)fread(buff, sizeof(char), BUFFSIZE, fp); 
	log_debug(flog,"Read %d bytes from config file", len);

	jsmn_init(&p);
	tok = malloc(sizeof(*tok) * tokcount);
	if (tok == NULL) {
		log_error(flog, "malloc error in parser");
		exit(1);
	}
    r = jsmn_parse(&p, buff, len, tok, tokcount);
    if (r < 0) {
            if (r == JSMN_ERROR_NOMEM) {
				log_error(flog, "Parse: Allocate more mem to tok"); return NULL; }
			if (ret == JSMN_ERROR_INVAL) {
				log_error(flog, "Parse: invalid JSON string"); return NULL; }
			if (ret == JSMN_ERROR_PART) {
				log_error(flog, "Parse: truncated JSON string"); return NULL; }
	} else {
		char s[20];
		log_debug(flog, "jsmn returned %d tokens", r);
		for (i = 0; i < r; i++) {
			strncpy(s, buff + tok[i+1].start,  tok[i+1].end-tok[i+1].start); 
			s[tok[i+1].end-tok[i+1].start] = '\0';
			log_debug(flog, "%.*s: %.*s", 
					tok[i].end-tok[i].start, buff + tok[i].start,
					tok[i+1].end-tok[i+1].start, buff + tok[i+1].start);
			if (jsoneq(buff, &tok[i], "Params") == 0) {
				log_debug(flog, "  ****"); i++; continue;
			} 
			if (jsoneq(buff, &tok[i], "customerID") == 0) {
				jsonData->custID = strtol(s, NULL,0);
			} else if (jsoneq(buff, &tok[i], "serverIP") == 0) {
				strcpy(jsonData->serverIP, s); 
			} else if (jsoneq(buff, &tok[i], "protocol") == 0) {
				jsonData->protocol = strtol(s, NULL,0); 
			} else if (jsoneq(buff, &tok[i], "myAS") == 0) {
				jsonData->myAS = strtol(s, NULL,0);
			} else if (jsoneq(buff, &tok[i], "myRouterID") == 0) {
				strcpy(jsonData->myRouterID, s); 
			} else if (jsoneq(buff, &tok[i], "myKeepalive") == 0) {
                                jsonData->myKeepalive = strtol(s, NULL,0);
			} else if (jsoneq(buff, &tok[i], "myPassword") == 0) {
                                strcpy(jsonData->myPassword, s);
			} else if (jsoneq(buff, &tok[i], "myMaxPath") == 0) {
                                jsonData->myMaxPath = strtol(s, NULL,0);
			} else if (jsoneq(buff, &tok[i], "peerAS") == 0) {
				jsonData->peerAS = strtol(s, NULL,0);
			} else if (jsoneq(buff, &tok[i], "peerRouterID") == 0) {
				strcpy(jsonData->peerRouterID, s); 
			} else if (jsoneq(buff, &tok[i], "peerKeepalive") == 0) {
                                jsonData->peerKeepalive = strtol(s, NULL,0);
			} else if (jsoneq(buff, &tok[i], "peerPassword") == 0) {
                                strcpy(jsonData->peerPassword, s);
			} else if (jsoneq(buff, &tok[i], "peerMaxPath") == 0) {
                                jsonData->peerMaxPath = strtol(s, NULL,0);
			}
			i++; 
		}
	}
	fflush(flog);
	fclose(fp);
	return jsonData;
}

/*
main() {
	jsonData_t* jsonData;
	jsonData = parse(stdout, NULL);
	//cliLoop();
}
*/
