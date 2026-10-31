#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>

#define RS_UDP_PROTOCOL_NUMBER 17
#define RS_BUFSIZE (1 << 16)


struct rs_socket {
    int fd;
    uint8_t recv_buf[RS_BUFSIZE];
    size_t recv_len;
};

noreturn void die(const char *fmt, ...) __attribute__ ((format(printf, 1, 2)));

void rs_socket_hexdump(struct rs_socket *s);

noreturn void die(const char *fmt, ...)  {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "FATAL: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

void hexdump(uint8_t *buf, size_t buf_size, size_t column_width) {
    static const char *hexdigits = "0123456789abcdef";
    printf("--- dump %p (%zu)---\n", buf, buf_size);

    for (size_t i = 0; i < buf_size; ++i) {
        uint8_t byte = buf[i];
        char msn = hexdigits[byte >> 4];
        char lsn = hexdigits[byte & 0xF];
        
        putchar(msn);
        putchar(lsn);

        if ((i + 1) % column_width == 0) {
            putchar('\n');
        } else {
            putchar(' ');
        }
    }

    if (buf_size % column_width != 0) {
        putchar('\n');
    }
}

void rs_socket_init(struct rs_socket *s) {
    int rc;
    
    memset(s, 0, sizeof(struct rs_socket));
    rc = socket(AF_INET, SOCK_RAW, RS_UDP_PROTOCOL_NUMBER);

    if (rc == -1) {
        die("socket (%s)", strerror(errno));
    }

    s->fd = rc;

    printf("* init socket #%d\n", s->fd);

    // rc = bind(s->fd, )

}

void rs_socket_recv(struct rs_socket *s) {
    printf("* trying to recv(fd: %d, buf: %p, n: %d)\n", s->fd, s->recv_buf, RS_BUFSIZE);
    s->recv_len = recv(s->fd, s->recv_buf, RS_BUFSIZE, 0);
    printf("* received %zd bytes\n", s->recv_len);
    rs_socket_hexdump(s);
    
}

void rs_socket_hexdump(struct rs_socket *s) {
    hexdump(s->recv_buf, s->recv_len, 16);
}

int main() {
    struct rs_socket s;
    rs_socket_init(&s);
    rs_socket_recv(&s);



    return 0;
}