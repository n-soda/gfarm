# $Id$

top_builddir = ../../..
top_srcdir = $(top_builddir)
srcdir = .

include $(top_srcdir)/makes/var.mk

LIBRARY = libgfutil.la
LIBRARY_RESULT = # do not install

SRCS =	alloc.c \
	assert.c \
	backtrace.c \
	daemon.c \
	fdutil.c \
	gfevent.c \
	gfnetdb.c \
	hash.c \
	hash_strptr.c \
	id_table.c \
	limit.c \
	logutil.c \
	lru_cache.c \
	nanosec.c \
	privlock.c \
	random.c \
	send_no_sigpipe.c \
	sleep.c \
	thrbarrier.c \
	thrsubr.c \
	timer.c \
	timeval.c \
	utf8.c
OBJS =	alloc.lo \
	assert.lo \
	backtrace.lo \
	daemon.lo \
	fdutil.lo \
	gfevent.lo \
	gfnetdb.lo \
	hash.lo \
	hash_strptr.lo \
	id_table.lo \
	limit.lo \
	logutil.lo \
	lru_cache.lo \
	msgdigest.lo \
	nanosec.lo \
	privlock.lo \
	random.lo \
	send_no_sigpipe.lo \
	sleep.lo \
	thrbarrier.lo \
	thrsubr.lo \
	timer.lo \
	timeval.lo \
	utf8.lo

CFLAGS = $(pthread_includes) $(COMMON_CFLAGS) -I$(GFUTIL_SRCDIR) \
	$(openssl_includes)

all: $(LIBRARY)

include $(top_srcdir)/makes/lib.mk
include $(top_srcdir)/makes/gflog.mk

###

$(OBJS): $(DEPGFARMINC)
backtrace.lo: gfutil.h
daemon.lo: gfutil.h
fdutil.lo: gfutil.h fdutil.h assert.h
gfevent.lo: $(INC_SRCDIR)/gfarm_misc.h gfutil.h assert.h gfevent.h
gfnetdb.lo: gfnetdb.h
id_table.lo: id_table.h
hash.lo: gfutil.h hash.h
hash_strptr.lo: hash.h
limit.lo: gfutil.h
logutil.lo: gfutil.h gflog_reduced.h
lru_cache.lo: lru_cache.h assert.h
msgdigest.lo: msgdigest.h
nanosec.lo: nanosec.h
privlock.lo: nanosec.h thrsubr.h gfutil.h
random.lo: thrsubr.h
sleep.lo: nanosec.h gfutil.h
timer.lo: timer.h
timeval.lo: gfutil.h
