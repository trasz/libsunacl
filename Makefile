# $FreeBSD$

LIB=		sunacl
PREFIX?=	/usr/local
LIBDIR=		${PREFIX}/lib
INCLUDEDIR=	${PREFIX}/include
SHLIB_MAJOR=	1
SRCS=		libsunacl.c opensolaris_acl.c
INCS=		sunacl.h

CFLAGS+=	-D_ACL_PRIVATE

.include <bsd.lib.mk>
