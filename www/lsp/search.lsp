<%+
# include "../../jorinde.h"
inherit LIB_HTTP_STRING;

string formquery;

string get_result(string query)
{
	string *res, ret;
	int i;

	res = (JORINDE_INDEX+"sys/indexd")->get_hits(query);

	ret = "";
	ret += sizeof(res) + " hits<br>";
	for(i = 0; i < sizeof(res); i++) {
		ret += "<a href=\"view.lsp?file=/" + res[i] + "&query=" +query+ "\">";
		ret += res[i] + "</a><br>";
	}
	return ret;
}

void output_index()
{
	mapping index;
	string *arr;
	int i;

	return;

	index = (JORINDE_INDEX+"sys/indexd")->get_index();
	arr = map_indices(index);
	for(i = 0; i < sizeof(arr); i++) {
		response->write(arr[i] + "<br>");
	}

}

string get_stats()
{
	mixed *res;
	string ret;

	ret = "";
	res = (JORINDE_INDEX+"sys/indexd")->get_stats();

	ret += "Files: " +				res[0] + "<br>";
	ret += "Folders: " +			res[1] + "<br>";
	ret += "Words: " +				res[2] + "<br>";
	ret += "Unique words: " +		res[3] + "<br>";
	ret += "Bytes: " +				res[4] + "<br>";
	ret += "Fileword obs: " +		res[5] + "<br>";
	ret += "Avg. word length: " +	res[6] + "<br>";
	ret += "Bad sized words: " +	res[7] + "<br>";
	ret += "Broken words: " +		res[8] + "<br>";
	ret += "Index objects: " +		res[9] + "<br>";
	ret += "Time spent indexing: "+	res[10] + " sec.<br>";

	ret += "Objs / word: " +		((float)res[9]/(float)res[2]) + "<br>";
	ret += "Objs / unique word: " +	((float)res[9]/(float)res[3]) + "<br>";
	return ret;
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
<%
#ifndef __TEST_INDEXD__
	response->write("Indexing engine is not available. Enable __TEST_INDEXD__");
	return;
#endif
	
	if(request->form("query")) {
		formquery = lower_case(request->form("query")->content_tostring());
	} else {
		formquery = "";
	}
%>
<table>
<tr>
<td width="70%" valign="top">
	<strong>Disclaimers:</strong><br>
	The indexing and searching application was thrown together in a couple 
	of hours on 08aug2004. Some time was spent optimizing after that.<br>
	<br>
	I know precicely jack-shit about indexing and searching.<br>
	<br>

<form action="search.lsp" method="post">
	Search (one word only):<br>
	<input type="text" name="query" value="<%=formquery%>">
	<input type="submit" name="submit" value="Submit">
</form>

<%
	if(formquery != "") {
		response->write( "Result<br>"+get_result(formquery) );
	} else {
		output_index();
	}
%>
</td>
<td valign="top">
	<%=get_stats()%>
</td>
</tr>
</table>
</body>
</html>

