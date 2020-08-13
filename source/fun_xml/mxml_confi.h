#ifndef MXML_CONFIG_H
#define MXML_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>


/*
 * Constants...
 */

#  define MXML_WRAP		72	/* Wrap XML output at this column position */
#  define MXML_TAB		8	/* Tabs every N columns */

#  define MXML_NO_CALLBACK	0	/* Don't use a type callback */
#  define MXML_NO_PARENT	0	/* No parent for the node */

#  define MXML_DESCEND		1	/* Descend when finding/walking */
#  define MXML_NO_DESCEND	0	/* Don't descend when finding/walking */
#  define MXML_DESCEND_FIRST	-1	/* Descend for first find */

#  define MXML_WS_BEFORE_OPEN	0	/* Callback for before open tag */
#  define MXML_WS_AFTER_OPEN	1	/* Callback for after open tag */
#  define MXML_WS_BEFORE_CLOSE	2	/* Callback for before close tag */
#  define MXML_WS_AFTER_CLOSE	3	/* Callback for after close tag */

#  define MXML_ADD_BEFORE	0	/* Add node before specified node */
#  define MXML_ADD_AFTER	1	/* Add node after specified node */
#  define MXML_ADD_TO_PARENT	NULL	/* Add node relative to parent */


/*
 * Data types...
 */

typedef enum mxml_type_e		/**** The XML node type. ****/
{
  MXML_ELEMENT,				/* XML element with attributes */
  MXML_INTEGER,				/* Integer value */
  MXML_OPAQUE,				/* Opaque string */
  MXML_REAL,				/* Real value */
  MXML_TEXT				/* Text fragment */
} mxml_type_t;

typedef struct mxml_attr_s		/**** An XML element attribute value. ****/
{
  char	*name;				/* Attribute name */
  char	*value;				/* Attribute value */
} mxml_attr_t;

typedef struct mxml_value_s		/**** An XML element value. ****/
{
  char		*name;			/* Name of element */
  int		num_attrs;		/* Number of attributes */
  mxml_attr_t	*attrs;			/* Attributes */
} mxml_element_t;

typedef struct mxml_text_s		/**** An XML text value. ****/
{
  int		whitespace;		/* Leading whitespace? */
  char		*string;		/* Fragment string */
} mxml_text_t;

typedef union mxml_value_u		/**** An XML node value. ****/
{
  mxml_element_t	element;	/* Element */
  int			integer;	/* Integer number */
  char			*opaque;	/* Opaque string */
  double		real;		/* Real number */
  mxml_text_t		text;		/* Text fragment */
} mxml_value_t;

typedef struct mxml_node_s mxml_node_t;	/**** An XML node. ****/

struct mxml_node_s			/**** An XML node. ****/
{
  mxml_type_t	type;			/* Node type */
  mxml_node_t	*next;			/* Next node under same parent */
  mxml_node_t	*prev;			/* Previous node under same parent */
  mxml_node_t	*parent;		/* Parent node */
  mxml_node_t	*child;			/* First child node */
  mxml_node_t	*last_child;		/* Last child node */
  mxml_value_t	value;			/* Node value */
};


/*
 * Prototypes...
 */

extern void		mxmlAdd(mxml_node_t *parent, int where,
                    mxml_node_t *child, mxml_node_t *node);
extern void		mxmlDelete(mxml_node_t *node);
extern const char	*mxmlElementGetAttr(mxml_node_t *node, const char *name);
extern void		mxmlElementSetAttr(mxml_node_t *node, const char *name,
                               const char *value);
extern mxml_node_t	*mxmlFindElement(mxml_node_t *node, mxml_node_t *top,
                             const char *name, const char *attr,
                     const char *value, int descend);
extern mxml_node_t	*mxmlLoadFile(mxml_node_t *top, FILE *fp,
                          mxml_type_t (*cb)(mxml_node_t *));
extern mxml_node_t	*mxmlNewElement(mxml_node_t *parent, const char *name);
extern mxml_node_t	*mxmlNewInteger(mxml_node_t *parent, int integer);
extern mxml_node_t	*mxmlNewOpaque(mxml_node_t *parent, const char *opaque);
extern mxml_node_t	*mxmlNewReal(mxml_node_t *parent, double real);
extern mxml_node_t	*mxmlNewText(mxml_node_t *parent, int whitespace,
                         const char *string);
extern void		mxmlRemove(mxml_node_t *node);
extern int		mxmlSaveFile(mxml_node_t *node, FILE *fp,
                         int (*cb)(mxml_node_t *, int));
extern mxml_node_t	*mxmlWalkNext(mxml_node_t *node, mxml_node_t *top,
                          int descend);
extern mxml_node_t	*mxmlWalkPrev(mxml_node_t *node, mxml_node_t *top,
                          int descend);


#ifdef __cplusplus
}
#endif

#endif
