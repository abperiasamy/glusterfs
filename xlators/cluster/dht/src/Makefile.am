
xlator_LTLIBRARIES = dht.la nufa.la switch.la
xlatordir = $(libdir)/glusterfs/$(PACKAGE_VERSION)/xlator/cluster


dht_common_source = dht-layout.c dht-helper.c dht-linkfile.c \
		dht-selfheal.c dht-rename.c dht-hashfn.c dht-diskusage.c \
		$(top_builddir)/xlators/lib/src/libxlator.c

dht_la_SOURCES = $(dht_common_source) dht.c 

nufa_la_SOURCES = $(dht_common_source) nufa.c
switch_la_SOURCES = $(dht_common_source) switch.c

dht_la_LDFLAGS = -module -avoidversion
dht_la_LIBADD = $(top_builddir)/libglusterfs/src/libglusterfs.la

nufa_la_LDFLAGS = -module -avoidversion
nufa_la_LIBADD = $(top_builddir)/libglusterfs/src/libglusterfs.la

switch_la_LDFLAGS = -module -avoidversion
switch_la_LIBADD = $(top_builddir)/libglusterfs/src/libglusterfs.la

noinst_HEADERS = dht-common.h dht-common.c dht-mem-types.h $(top_builddir)/xlators/lib/src/libxlator.h

AM_CFLAGS = -fPIC -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -Wall -D$(GF_HOST_OS) \
	-I$(top_srcdir)/libglusterfs/src -shared -nostartfiles $(GF_CFLAGS) \
	-I$(top_srcdir)/xlators/lib/src

CLEANFILES = 

uninstall-local:
	rm -f $(DESTDIR)$(xlatordir)/distribute.so

install-data-hook:
	ln -sf dht.so $(DESTDIR)$(xlatordir)/distribute.so
