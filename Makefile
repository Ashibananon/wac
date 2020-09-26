CC=gcc
CFLAGS += -Wall
#LDFLAGS += -fPIC -shared

TARGET:=wac

.PHONY: target
target: ${TARGET}

objs:=main.o

${TARGET}: ${objs}
	${CC} -o $@ ${CFLAGS} $^

.SUFFIXES: .c
.c.o:
	${CC} -c ${CFLAGS} $<

.PHONY: clean
clean:
	rm -rf $(objs)
