# $Id$

top_builddir = ../../../../..
top_srcdir = $(top_builddir)
srcdir = .

include $(top_srcdir)/makes/var.mk

htmldir=$(default_htmldir)/ja/ref/man8

DOCBOOK_DIR=$(top_srcdir)/doc/docbook/ja/ref/man8
include $(DOCBOOK_DIR)/Makefile.inc
HTMLSRC=$(DOCBOOK)
DOCBOOK2HTML_XSL=../docbook2html.xsl

include $(top_srcdir)/makes/html.mk
