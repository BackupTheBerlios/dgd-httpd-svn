/**
 * Description: Initialize various Jorinde projects.
 * Version:     $Id: initd.c 210 2004-06-15 08:27:28Z romland $
 * License:     (c)2004 Joakim Romland, see doc/License
 */
# include "./open/jorinde.h"

static void create(varargs int clone)
{
	COMPILE(JORINDE_ERRORD +"initd");
	COMPILE(JORINDE_SHARED +"initd");
	COMPILE(JORINDE_XML	+	"initd");
	COMPILE(JORINDE_LUNIT +	"initd");
	COMPILE(JORINDE_HTTPD +	"initd");
#ifdef __TEST_INDEXD__
	COMPILE(JORINDE_INDEX + "initd");
#endif
}
