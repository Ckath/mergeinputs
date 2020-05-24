# see LICENSE file for copyright and license information.

NAME=mergeinputs
LIBS = -lpthread
SRC = ${NAME}.c
OBJ = ${SRC:.c=.o}
CFLAGS = -Os
DESTDIR = /usr/local
CC=gcc

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} ${LIBS} $<

${NAME}: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${CFLAGS} ${LIBS}

clean:
	@echo cleaning
	@rm -f ${NAME} *.o

install: ${NAME}
	@echo installing executable file to ${DESTDIR}/bin
	@mkdir -p ${DESTDIR}/bin
	@cp -f ${NAME} ${DESTDIR}/bin/${NAME}
	@chmod 755 ${DESTDIR}/bin/${NAME}
uninstall: ${NAME}
	@echo removing executable file from ${DESTDIR}/bin
	@rm -f ${DESTDIR}/bin/${NAME}
