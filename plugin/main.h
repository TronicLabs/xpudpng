#ifndef _MAIN_H
#define _MAIN_H

#include "config_build.h"

#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if IBM
#warning "UNTESTED !"
#include <windows.h>
#else /* IBM */
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif /* IBM */

#define XPUDPNG_SEND_PORT 49020
#define XPUDPNG_RECV_PORT 49021
#define XPUDPNG_RECV_FREQ_CALL 0.01
#define XPUDPNG_RECV_FREQ_RETURN 0.016 // 60hz

#endif /* _MAIN_H */
