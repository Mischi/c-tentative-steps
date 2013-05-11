PROG=		zmqhwworker
SRCS=		zmqhwworker.c ../zhelpers.h
NOMAN= 		noman

CFLAGS+=	-Wall -I/usr/local/include
LDADD+=		-lzmq
LDFLAGS+=	-L/usr/local/lib

.include <bsd.prog.mk>
