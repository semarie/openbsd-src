#	$OpenBSD$
CFLAGS+=	-Wall -Werror

PASS_TARGETS=	pass-_exit \
		pass-stdio \
		pass-tty
FAIL_TARGETS=	catching-sigabrt \
		fail-dns

# generate PASS tests
.for _t in ${PASS_TARGETS}
REGRESS_TARGETS+=	test-${_t}
CLEANFILES+=		${_t}

test-${_t}: ${_t}
	@echo "running $@"
	@sh ${.CURDIR}/run.sh pass ${.OBJDIR}/${_t} \
		| diff -u ${.CURDIR}/${_t}.out -
.endfor

# generate FAIL tests
.for _t in ${FAIL_TARGETS}
REGRESS_TARGETS+=	test-${_t}
CLEANFILES+=		${_t}

test-${_t}: ${_t}
	@echo "running $@"
	@sh ${.CURDIR}/run.sh fail ${.OBJDIR}/${_t} \
		| diff -u ${.CURDIR}/${_t}.out -
.endfor

.include <bsd.regress.mk>
