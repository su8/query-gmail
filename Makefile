CFLAGS+=-Wall -Wextra -O2
LDFLAGS+=-lcurl

PACKAGE=mail
PROG=mail.c

all:
	$(CC) -DGMAIL_ACC=\"'$(ACC)'\" -DGMAIL_PASS=\"'$(PASS)'\" $(CFLAGS) $(LDFLAGS) -o $(PACKAGE) $(PROG)

install: 
	install -D -s -m 755 $(PACKAGE) /usr/bin/$(PACKAGE)

clean:
	rm -f /usr/bin/$(PACKAGE)
	rm -f $(PACKAGE)

.PHONY: all install clean
