/* Client for 5.1.1-5.1.3 */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/* Code taken from https://www.geeksforgeeks.org/modular-exponentiation-power-in-modular-arithmetic
    used to calculate large powers without overflow. */
int power(int g, unsigned int b, int p)
{
    int res = 1;     // Initialize result

    while (b > 0)
    {
        // If y is odd, multiply x with result
        if (b & 1)
            res = res*g;

        // n must be even now
        b = b>>1; // y = y/2
        g = g*g;  // Change x to x^2
    }
    return res%p;
}



int main(int argc, char ** argv)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent * server;

    char buffer[256];
    int g = 15;
    int p = 97;
    /*if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(EXIT_FAILURE);
    }*/

    portno = 7800;
    int b = atoi(argv[1]);
    /* Translate host name into peer's IP address ;
     * This is name translation service by the operating system */
    server = gethostbyname("172.26.37.44");

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }

    /* Building data structures for socket */

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy(server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    /* Create TCP socket -- active open
     * Preliminary steps: Setup: creation of active open socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(EXIT_FAILURE);
    }

    /* Do processing */
    while (1)
    {

        char* username = "samr1\n";

        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
        }

        /*if (!strncmp(buffer, "GOODBYE-CLOSE-TCP", 17))
            break;*/

        char sent[256];
        sprintf(sent, "%d", power(g, b, p));
        n = write(sockfd, sent, strlen(sent));
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
        }
        char received[256];
        n = read(sockfd, received, 255);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(EXIT_FAILURE);
        }
        received[n] = 0;
        int fromServer = atoi(received);
        char final[256];
        sprintf(final, "%d", power(fromServer, b, p));
        n = write(sockfd, final, strlen(final));

        n = read(sockfd, buffer, 255);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(EXIT_FAILURE);
        }
        printf("%s\n", buffer);


    }

    close(sockfd);

    return 0;
}
