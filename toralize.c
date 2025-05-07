#include "toralize.h"
Req *request(struct sockaddr_in *addr) {
    Req *req;

    req = malloc(reqsize);
    req->vn = 4;
    req->cd = 1;
    req->dstport = addr->sin_port;
    req->dstip = addr->sin_addr.s_addr;
    strncpy(req->userid, USERNAME, 8);

    return req;
}   

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){

    int s;
    struct sockaddr_in sock;
    Req *req;
    Res *res;
    char buf[ressize];
    int success;
    char tmp[512];
    int (*p)(int, const struct sockaddr*, socklen_t);

    p = dlsym(RTLD_NEXT, "connect");

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
    if (p(s, (struct sockaddr *)&sock, sizeof(sock))) {
        perror("connect");
        return -1;
    }

    // Send the request to the proxy server
    req = request((struct sockaddr_in *)&addr);
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
    printf("Proxy connection established \n");

    dup2(s, sockfd);
    free(req);
    return 0;
}