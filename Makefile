#
#

BASE = ${PWD}

MAKE_DIR  = ${BASE}/make
SAL_DIR   = ${BASE}/sal
LANG_DIR  = ${BASE}/lang
CUNIT_DIR = ${BASE}/cunit

TEST_DIR  = ${BASE}/test

CC = gcc
LD = gcc
RM = rm -rf
ECHO = echo
MAKE = make


CPPFLAGS = -I${BASE}
CFLAGS = -g -Wall -Werror
LDFLAGS = -L${CUNIT_DIR} -L${LANG_DIR} -L${SAL_DIR}

export CC LD RM ECHO MAKE
export CPPFLAGS
export CFLAGS
export LDFLAGS

.PHONY: all test cunit lang sal panda

all: test

cunit:
	${MAKE} -C cunit -f ${MAKE_DIR}/Makefile.pub

sal:
	${MAKE} -C sal -f ${MAKE_DIR}/Makefile.pub

lang: sal
	${MAKE} -C lang -f ${MAKE_DIR}/Makefile.pub

test: clean cunit lang panda
	${MAKE} -C test -f ${MAKE_DIR}/Makefile.pub
	${TEST_DIR}/test

panda: lang
	${MAKE} -C panda -f ${MAKE_DIR}/Makefile.pub

clean:
	${MAKE} -C cunit clean -f ${MAKE_DIR}/Makefile.pub
	${MAKE} -C sal   clean -f ${MAKE_DIR}/Makefile.pub
	${MAKE} -C lang  clean -f ${MAKE_DIR}/Makefile.pub
	${MAKE} -C test  clean -f ${MAKE_DIR}/Makefile.pub
	${MAKE} -C panda  clean -f ${MAKE_DIR}/Makefile.pub

