TOP=	.
PROG=	xpudpng.xpl
SRCS=	main.c

include	${TOP}/Makefile.config
include	${TOP}/mk/build.prog.mk

# Defined in ${TOP}/Makefile.config
CFLAGS+=	-fPIC -DXPLM200=1 \
		-Wall -Wno-sign-compare -Wno-unknown-pragmas \
		-I${PREFIX}/include \
		-I${XPLANE_SDK_INCLUDES}/CHeaders/XPLM \
		-I${XPLANE_SDK_INCLUDES}/CHeaders/Widgets \
		-I${TOP}/../headers

LDFLAGS+=	-shared -static-libgcc -fPIC

reconfigure:
	rm -f configure
	cat configure.in | mkconfigure > configure
	chmod 755 configure
