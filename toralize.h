//https://www.openssh.com/txt/socks4.protocol

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PROXY       "127.0.0.1"
#define PROXYPORT   9050
#define USERNAME "toralize"
#define reqsize sizeof(struct proxy_request)
#define ressize sizeof(struct proxy_response)

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;

/*
		+----+----+----+----+----+----+----+----+----+----+....+----+
		| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
		+----+----+----+----+----+----+----+----+----+----+....+----+
bytes  	   1    1      2              4           variable       1
proxy request
*/
struct proxy_request {
    uint8 vn;
    uint8 cd;
    uint16 dstport;
    uint32 dstip;
    unsigned char userid[8];
};
typedef struct proxy_request Req;

/*
		+----+----+----+----+----+----+----+----+
		| VN | CD | DSTPORT |      DSTIP        |
		+----+----+----+----+----+----+----+----+
bytes	   1    1      2              4
proxy response
*/
struct proxy_response {
    uint8 vn;
    uint8 cd;
    uint16 _;
    uint32 __;
};
typedef struct proxy_response Res;

Req *request(const char*, const int);
int main(int, char**);