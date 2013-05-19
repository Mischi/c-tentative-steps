PROG=		zmqhwclient
SRCS=		zmqhwclient.c ../zhelpers.h
NOMAN= 		noman

CC= 		clang
CTAGS=		ectags
CFLAGS+=	-Wall -I/usr/local/include
LDADD+=		-lzmq
LDFLAGS+=	-L/usr/local/lib

.include <bsd.prog.mk>
