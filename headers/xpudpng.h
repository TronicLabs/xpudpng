#ifndef _XPUDPNG_H
#define _XPUDPNG_H

struct xpudpng_packet
{
	from char[6]; /* CLIENT or PLUGIN */
	query char[256]; /* The "query", can be NULL (if reply) or "subscribe foo" etc... */
	reply char[256]; /* The reply, from the plugin, NULL if query */

	dataRefString char[256]; /* The ASCII DataRef String */
	dataRefValue void;
	dataRefType char[5]; /* CHAR FLOAT BYTE INT */

	accuracyValue void;
	accuracyType char[5]; /* CHAR FLOAT BYTE INT */
}


#endif /* _XPUDPNG_H */
