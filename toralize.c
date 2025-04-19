#include "toralize.h"
Req *request(const char *dstip, const int dstport){
    Req *req;

    req = malloc(reqsize);
    req->vn = 4;
    req->cd = 1;
    req->dstport = htons(dstport);
    req->dstip = inet_addr(dstip);
    strncpy(req->userid, USERNAME, 7);

    return req;
}   

int main(int argc, char *argv[]){
    char *host;
    int port, s;
    struct sockaddr_in sock;
    Req *req;
    Res *res;
    char buf[ressize];
    int success;
    char tmp[512];

    // Check if the user provided the required arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        return -1;
    }

    // Parse the command line arguments
    host = argv[1];
    port = atoi(argv[2]);

    // Create a socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");
        return -1;
    }

    // Set up the sockaddr_in structure
    sock.sin_family = AF_INET;  
    sock.sin_port = htons(PROXYPORT);                                                    
    sock.sin_addr.s_addr = inet_addr(PROXY);

    // Connect to the proxy server 
    if (connect(s, (struct sockaddr *)&sock, sizeof(sock))) {
        perror("connect");
        return -1;
    }

    // Send the request to the proxy server
    req = request(host, port);
    write(s, req, reqsize);

    // Read the response from the proxy server
    memset(buf, 0, ressize);
    if (read(s, buf, ressize) < 1) {
        perror("read");
        free(req);
        close(s);
        return -1;
    }

    // Check the response from the proxy server
    res = (Res *)buf;
    success = (res->cd == 90);
    if (!success) {
        fprintf(stderr, "Unable to traverse the proxy, error code: %d\n", res->cd);

        close(s);
        free(req);
        return -1;
    }
    printf("Proxy connection established to: %s:%d\n", host, port);
    
    memset(tmp, 0, 512);
    snprintf(tmp, 511,
        "HEAD / HTTP/1.0\r\n"
        "Host: www.networktechnology.org\r\n"
        "\r\n"
    );
    write(s, tmp, strlen(tmp));
    memset(tmp, 0, 512);
    read(s, tmp, 511);
    printf("'%s'\n", tmp);

    close(s);
    free(req);
    return 0;
}