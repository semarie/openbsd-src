# $OpenBSD: Makefile,v 1.1 2021/09/15 10:21:30 fcambus Exp $

.include <bsd.own.mk>

LLVM_V=		11.1.0
LIBDIR=		/usr/lib/clang/${LLVM_V}/lib

.if ${COMPILER_VERSION:L} != "clang"
CC=		clang
CXX=		clang++
.endif

.if ${BUILD_CLANG:L} == "yes"

LIB=	clang_rt.profile
NOPIC=
NOPROFILE=

CFLAGS+=	-fPIC -std=gnu99 -fvisibility=hidden -fno-stack-protector
CFLAGS+=	-DCOMPILER_RT_HAS_ATOMICS=1
CFLAGS+=	-DCOMPILER_RT_HAS_FCNTL_LCK=1
CFLAGS+=	-DCOMPILER_RT_HAS_UNAME=1
CPPFLAGS+=	-DVISIBILITY_HIDDEN
CPPFLAGS+=	-I${BSDSRCDIR}/gnu/llvm/compiler-rt/include/

.PATH: ${.CURDIR}/../../../llvm/compiler-rt/lib/profile

SRCS+=		GCDAProfiling.c \
		InstrProfiling.c \
		InstrProfilingBiasVar.c \
		InstrProfilingBuffer.c \
		InstrProfilingFile.c \
		InstrProfilingInternal.c \
		InstrProfilingMerge.c \
		InstrProfilingMergeFile.c \
		InstrProfilingNameVar.c \
		InstrProfilingPlatformOther.c \
		InstrProfilingRuntime.cpp \
		InstrProfilingUtil.c \
		InstrProfilingValue.c \
		InstrProfilingWriter.c

beforeinstall:
	${INSTALL} -d -o ${BINOWN} -g ${BINGRP} -m ${DIRMODE} \
		${DESTDIR}${LIBDIR}

.include <bsd.lib.mk>

.else
NOPROG=
.include <bsd.prog.mk>
.endif
