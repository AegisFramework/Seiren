bin_PROGRAMS = seiren hello

seiren_SOURCES = main.c main.h text.c text.h server.c server.h
seiren_CFLAGS = $(SEIREN_CFLAGS)

seiren_LDADD = $(SEIREN_LIBS)

hello_SOURCES = hello.c
hello_CFLAGS = $(SEIREN_CFLAGS)

hello_LDADD = $(SEIREN_LIBS)

-include $(top_srcdir)/git.mk
