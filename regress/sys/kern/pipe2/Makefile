#	$OpenBSD$

PROGS +=	readwrite

LDADD_readwrite +=	-lpthread
DPADD_readwrite +=	${LIBPTHREAD}

WARNINGS +=	Yes

REGRESS_TARGETS +=	readwrite-rand-test \
	readwrite-1r-4w-test \
	readwrite-4r-1w-test \
	readwrite-1r-1w-test

readwrite-rand-test: readwrite
	./readwrite

readwrite-1r-4w-test: readwrite
	./readwrite -r 1 -w 4 -t 2

readwrite-4r-1w-test: readwrite
	./readwrite -r 4 -w 1 -t 2

readwrite-1r-1w-test: readwrite
	./readwrite -r 1 -w 1 -t 2


.include <bsd.regress.mk>
