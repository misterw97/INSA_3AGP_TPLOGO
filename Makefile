EXEC = logo
SRCS_COMMONS = logo_functions.c pen.c
SRCS_LOGO = ${EXEC}.tab.c lex.yy.c
SRCS_TEST = test.c
HDRS_COMMONS = logo_type.h logo_functions.h pen.h
HDRS_LOGO = ${EXEC}.tab.h

CC = gcc
CFLAGS += -c -g -Wall
LDFLAGS += -lm

all: ${EXEC}

exe: ${EXEC} clean

examples: exe
	for f in examples/*.logo; do \
		./logo < $$f ; \
	done

${EXEC}: $(SRCS_COMMONS:.c=.o) $(SRCS_LOGO:.c=.o) ${HDRS_COMMONS} ${HDRS_LOGO}
	${CC} $^ -o $@ ${LDFLAGS}

test: $(SRCS_COMMONS:.c=.o) $(SRCS_TEST:.c=.o) ${HDRS_COMMONS}
	${CC} $^ -o $@ ${LDFLAGS}

%.o: %.c
	${CC} ${CFLAGS} $< -o $@

${EXEC}.tab.c: ${EXEC}.y
	bison -d $^

lex.yy.c: ${EXEC}.l
	flex $^

clean:
	@rm -f *.o ${EXEC}.tab.c ${EXEC}.tab.h lex.yy.c

fclean: clean
	@rm -f ${EXEC} test 
	@rm -f *.svg
