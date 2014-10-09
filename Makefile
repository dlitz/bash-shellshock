NORMAL_FLAGS = -DREAL_BASH=\"/bin/bash.real\"

TEST_FLAGS = -DREAL_BASH=\"/usr/bin/env\" -DCONFIG_FILE_LOG_ONLY=\"testout/bash-shellshock.log-only\" -DCONFIG_FILE_STRIP_VARS=\"testout/bash-shellshock.strip-vars\"

all: bash-shellshock test

clean:
	rm -f bash-shellshock bash-shellshock.t testenv

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(NORMAL_FLAGS)

%.t: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(TEST_FLAGS)

install:
	install -m 0755 -d $(DESTDIR)/bin
	install -m 0755 bash-shellshock $(DESTDIR)/bin/bash-shellshock

test: bash-shellshock.t testenv
	./runtests

.PHONY: all clean install test
