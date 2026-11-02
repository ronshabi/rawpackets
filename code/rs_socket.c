#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "die.h"

#include "rs_socket.h"
#include "utils.h"

// FIXME: should not die
int rs_socket_init(struct rs_socket *s)
{
	int rc;
	memset(s, 0, sizeof(struct rs_socket));

	//
	// Create the socket
	//
	// I'm using AF_INET + SOCK_RAW without IPPROTO_RAW
	// Since I want to receive packets as well.
	//
	// https://man7.org/linux/man-pages/man7/raw.7.html
	//

	rc = socket(AF_INET, SOCK_RAW, RS_TCP_PROTOCOL_NUMBER);

	if (rc == -1) {
		die("socket (%s)", strerror(errno));
	}

	s->fd = rc;
	printf("* init socket #%d\n", s->fd);

	//
	// Set socket option: IP_HDRINCL
	//
	// INET RAW sockets will always RECEIVE a IP header,
	// but to SEND our crafted header, we need to set IP_HDRINCL as a
	// socket option.
	//
	// https://man7.org/linux/man-pages/man7/raw.7.html
	// https://man7.org/linux/man-pages/man2/setsockopt.2.html
	// https://man7.org/linux/man-pages/man2/IP_HDRINCL.2const.html
	//

	int yes = 1;
	rc = setsockopt(s->fd, IPPROTO_IP, IP_HDRINCL, &yes, sizeof(yes));

	if (rc != 0) {
		die("setsockopt(IPPROTO_IP, IP_HDRINCL, 1): %s",
		    strerror(errno));
	}

	printf("* set IP_HDRINCL\n");
	return 0;
}


// FIXME: should not die
int rs_socket_recv(struct rs_socket *s)
{
	printf("* trying to recv(fd: %d, buf: %p, n: %d)\n", s->fd, s->recvbuf,
	       RS_SOCKET_BUFSIZE);
	s->recvlen = recv(s->fd, s->recvbuf, RS_SOCKET_BUFSIZE, 0);
	printf("* received %zd bytes\n", s->recvlen);

	if (s->recvlen < 0 || s->recvlen > RS_SOCKET_BUFSIZE) {
		die("recv length %zd (bufsize is %d)", s->recvlen,
		    RS_SOCKET_BUFSIZE);
	}

	rs_socket_hexdump(s);

	return 0;
}

void rs_socket_hexdump(struct rs_socket *s)
{
	hexdump(s->recvbuf, s->recvlen, 16);
}
void rs_socket_teardown(struct rs_socket *s)
{
	printf("* closing socket #%d\n", s->fd);
	close(s->fd);
}