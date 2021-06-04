PREFIX ?= /usr

all: config.h sue

config.h:
	cp config.def.h config.h

sue: sue.o config.h
	${CC} ${LDFLAGS} sue.o -o sue -static

clean:
	rm -f sue.o sue

install: sue
	install -Dt $(DESTDIR)$(PREFIX)/bin sue
	chmod u+s $(DESTDIR)$(PREFIX)/bin/sue
