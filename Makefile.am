ACLOCAL_AMFLAGS = -I build-aux

bin_PROGRAMS = yaspell
yaspell_CFLAGS = $(AM_CFLAGS)
yaspell_SOURCES = yaspell.c dict.c email.c transform.c shim.c

.PHONY: test
test: yaspell
	./yaspell test/sherlock_holmes.txt yaspell-dict/british-english yaspell-dict/doyle-english | wc -c | grep '^0$$'