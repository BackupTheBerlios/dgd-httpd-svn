<%+
# include "../../jorinde.h"
inherit LIB_HTTP_STRING;

void visualize_index()
{
    int i, sz;
    string *chars;
	mapping index;

	index = (JORINDE_INDEX+"sys/indexd")->get_index();

/*    chars = map_indices(index);*/
	chars = ({ "z" });
    for(i = 0; i < sizeof(chars); i++) {
        response->write( chars[i] + "\n");
        if( index[chars[i]] ) {
            response->write("\n" +
				object_name(index[chars[i]]) + "\n" +
				index[chars[i]]->visualize_index(chars[i], 1) + "\n");
        }
    }
}

+%>
<!doctype html public "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
   <meta name="GENERATOR" content="Hands of Joakim Romland">
   <title>testing jindex</title>
</head>
<body>
<pre>
<%
#ifndef __TEST_INDEXD__
	response->write("Indexing engine is not available. Enable __TEST_INDEXD__");
	return;
#endif
	visualize_index();
%>
</pre>
</body>
</html>

