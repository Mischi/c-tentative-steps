PROG=		webserv1
SRCS=		webserv1.c zhelpers.h
NOMAN= 		noman

CC=			clang
CTAGS=		ectags

CFLAGS+=	-Wall -I/usr/local/include
LDADD+=		-lzmq -lpthread
LDFLAGS+=	-L/usr/local/lib

DEBUG=		-g
.include <bsd.prog.mk>
