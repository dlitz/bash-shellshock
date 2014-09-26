all: bash-shellshock

clean:
	rm -f bash-shellshock

bash-shellshock: bash-shellshock.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

install:
	install -m 0755 -d $(DESTDIR)/bin
	install -m 0755 bash-shellshock $(DESTDIR)/bin/bash-shellshock

.PHONY: all clean install
