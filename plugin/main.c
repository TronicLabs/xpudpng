#include "main.h"

int gMenuItem;
char AboutText[50][200] = {
	"   XPUDPng permit clients to use UDP to register to the plugin",
	"   then telling the plugin what DataRefs to send to the client.",
	"   ",
	"   This plugin have been inspired by the ExtPlane plugin, doing",
	"   approxymatively the same thing, but using TCP.",
	"   ",
	"   XPUDPng is distributed under the New BSD License.",
	"   ",
	"   Project HomePage: http://github.com/rhaamo/xpudpng",
	"   Contact email: rhaamo@gruik.at",
	"end"
};

XPWidgetID AboutWidget = NULL;
XPWidgetID AboutWindow = NULL;
XPWidgetID AboutTextWidget[50] = {
	NULL
};
XPLMMenuID PluginMenu;
int PluginSubMenuItem;

void AboutMenuHandler(void *, void *);
void CreateAboutWidget(int x1, int y1, int w, int h);
int AboutHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long iParam1, long inParam2);
void create_menu(void);

/* Socket Stuff */
void set_addresses(void);
int ensure_socket_bound(void);

int plugin_enabled;
int send_enabled;
int recv_enabled;
int recv_socket_open;

int receiverSocket;

struct sockaddr_in receiverSocketSrcAddr;

fd_set receiverReadfs;

float ReceiveClientTalkCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop,
    int inCounter, void *inRefcon);


PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{


	strcpy(outName, "XPUDPng " VERSION);
	strcpy(outSig, "li.rhaamo.xpudpng");
	strcpy(outDesc, "Send simuator data to an XPUDPNG-compliant application using UDP.");

	/* Initialize status variables */
	plugin_enabled = 0; /* set to 1 if the plugin is enabled from the menu. 0 otherwise */
	send_enabled = 1;   /* set to 1 if socket is open and preferences window is not shown. 0 otherwise */
	recv_enabled = 1;   /* set to 1 if socket is open and preferences window is not shown. 0 otherwise */
	recv_socket_open = 0;

	XPLMDebugString("XPUDPNG: Plugin " VERSION "\n");


	/* Create socket and addresses */
	receiverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	(void)set_addresses();
	(void)create_menu();

	/* Flag to tell us if the widget is being displayed */
	gMenuItem = 0;
	return 1;
}

PLUGIN_API void XPluginStop(void)
{
	if (gMenuItem == 1)
	    {
		    XPDestroyWidget(AboutWidget, 1);
		    gMenuItem = 0;
	    }
	if (recv_socket_open == 1) {
		if (close(receiverSocket) == -1) {
			XPLMDebugString("XPUDPNG: failed - Caught error while closing socket!(");
			XPLMDebugString((char * const)strerror(errno));
			XPLMDebugString(")\n");
		} else {
			XPLMDebugString("XPUDPNG: Closed socket\n");
		}
		recv_socket_open = 0;
		recv_enabled = 0;
	}
}

PLUGIN_API int XPluginEnable(void)
{
	recv_enabled = 1;
	plugin_enabled = 1;
	XPLMDebugString("XPUDPNG: enabled\n");

	/* Register Flight Loop Callbacks */
	XPLMRegisterFlightLoopCallback(ReceiveClientTalkCallback, XPUDPNG_RECV_FREQ_CALL, NULL);

	return ensure_socket_bound();
}

PLUGIN_API int XPluginDisable(void)
{
	XPLMUnregisterFlightLoopCallback(ReceiveClientTalkCallback, NULL);
	XPLMDebugString("XPUDPNG: disabled\n");
	plugin_enabled = 0;
	recv_enabled = 0;
	return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, long inMsg, void *inParam)
{
}

/* Handle the Menu */
void AboutMenuHandler(void *inMenuRef, void *inItemRef)
{
	switch ( (int)inItemRef )
	    {
	    case 1:
		    if (gMenuItem == 0) {
			    CreateAboutWidget(50, 900, 500, 250); /* left, top, right, bottom */
			    gMenuItem = 1;
		    } else {
			    if(!XPIsWidgetVisible(AboutWidget))
				    XPShowWidget(AboutWidget);
		    }
		    break;
	    }
}

/* Create the Widget dialog */
void CreateAboutWidget(int x, int y, int w, int h)
{
	int Index;
	int x2 = x + w;
	int y2 = y - h;

	/* Create the Main Widget window */
	AboutWidget = XPCreateWidget(x, y, x2, y2,
	    1, /* Visible */
	    "XPUDPng " VERSION, /* Description */
	    1, /* root */
	    NULL, /* No container */
	    xpWidgetClass_MainWindow);

	/* Add Close Box to the Main Widget. Other options are available */
	XPSetWidgetProperty(AboutWidget, xpProperty_MainWindowHasCloseBoxes, 1);

	/* Print each line of the About Text */
	for (Index=0;Index < 50; Index++)
	    {
		    if(strcmp(AboutText[Index],"end") == 0)
			    break;

		    /* Create a text widget */
		    AboutTextWidget[Index] = XPCreateWidget(x+10, y-(30+(Index*20)), x2-10, y-(42+(Index*20)),
			1, /* Visible */
			AboutText[Index], /* Description */
			0, /* root */
			AboutWidget,
			xpWidgetClass_Caption);
	    }

	/* Register our widget handler */
	XPAddWidgetCallback(AboutWidget, AboutHandler);
}

/* The Widget handler */
int AboutHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if (inMessage == xpMessage_CloseButtonPushed)
	    {
		    if (gMenuItem == 1)
			    XPHideWidget(AboutWidget);
		    return 1;
	    }
	return 0;
}

/* Create the Menu */
void create_menu(void)
{
	PluginSubMenuItem = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "XPUDPng", NULL, 1);
	PluginMenu = XPLMCreateMenu("XPUDPng " VERSION, XPLMFindPluginsMenu(), PluginSubMenuItem, AboutMenuHandler, NULL);
	XPLMAppendMenuItem(PluginMenu, "About XPUDPNG", (void *)+1, 1);
}

void set_addresses(void)
{
	XPLMDebugString("XPUDPNG: Setting addresses\n");

	receiverSocketSrcAddr.sin_family = AF_INET;
	receiverSocketSrcAddr.sin_port = htons(XPUDPNG_RECV_PORT);
	receiverSocketSrcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	char for_port[100];
	sprintf(for_port, "XPUDPNG: Prefs: IP=any, RECV-PORT=%i\n", XPUDPNG_RECV_PORT);
	XPLMDebugString(for_port);
}

int ensure_socket_bound(void)
{
	if (recv_socket_open != 1) {
		if (bind(receiverSocket, (struct sockaddr*)&receiverSocketSrcAddr, sizeof(struct sockaddr)) == -1) {
			XPLMDebugString("XPUDPNG: caught error while binding socket (");
			XPLMDebugString((char * const)strerror(errno));
			XPLMDebugString(")\n");
			recv_socket_open = 0;
			return -1;
		} else {
			XPLMDebugString("XPUDPNG: Socket bound!\n");
			recv_socket_open = 1;
			return 1;
		}
	} else {
		return 1;
	}
}

float
ReceiveClientTalkCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop,
    int inCounter, void *inRefcon)
{
	if ((plugin_enabled == 1) && (recv_enabled == 1) && (recv_socket_open == 1)) {
		char buffer[256];
		int ret = 0;
		struct timeval nowait;
		nowait.tv_sec = 0;
		nowait.tv_usec = 0; /* nowait please */
		FD_ZERO(&receiverReadfs);
		FD_SET(receiverSocket, &receiverReadfs);
		if ((ret = select(receiverSocket + 1, &receiverReadfs, NULL, NULL, &nowait)) < 0) {
			XPLMDebugString("XPUDPNG: caught error while select (");
			XPLMDebugString((char * const)strerror(errno));
			XPLMDebugString(")\n");
			return -1;
		}

		if (ret == 0) {
		}

		if (FD_ISSET(receiverSocket, &receiverReadfs))
		    {
			    recv(receiverSocket, buffer, sizeof(buffer), 0);
			    XPLMDebugString("XPUDPNG: Got input packet\n");
			    return XPUDPNG_RECV_FREQ_RETURN;
		    }
		return XPUDPNG_RECV_FREQ_RETURN;
	} else {
		return 1;
	}
}
