<%+
# include "../../jorinde.h"
inherit LIB_HTTP_STRING;

void start_index()
{
	(JORINDE_INDEX+"sys/indexd")->create_index("/doc/");
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

	if(request->item("start")) {
		response->write("Starting indexing...<br>You may resume doing " + 
						"whatever you were doing.");
		start_index();
	} else {
%>
		<a href="search_admin.lsp?start=yes">start indexing</a>
<%
	}
%>
</pre>
</body>
</html>

