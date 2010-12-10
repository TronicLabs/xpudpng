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

void AboutMenuHandler(void *, void *);
void CreateAboutWidget(int x1, int y1, int w, int h);
int AboutHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long iParam1, long inParam2);

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
	XPLMMenuID PluginMenu;
	int PluginSubMenuItem;

	strcpy(outName, "XPUDPng " VERSION);
	strcpy(outSig, "xpudpng.rhaamo.li");
	strcpy(outDesc, "Send choosed DataRefs values to registered clients");

	XPLMDebugString("XPUDPNG: Plugin " VERSION "\n");

	/* Create the Menu */
	PluginSubMenuItem = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "XPUDPng", NULL, 1);
	PluginMenu = XPLMCreateMenu("XPUDPng " VERSION, XPLMFindPluginsMenu(), PluginSubMenuItem, AboutMenuHandler, NULL);
	XPLMAppendMenuItem(PluginMenu, "About XPUDPNG", (void *)+1, 1);

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
}

PLUGIN_API int XPluginEnable(void)
{
	return 1;
}

PLUGIN_API int XPluginDisable(void)
{
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

