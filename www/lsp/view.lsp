<%+
inherit "/usr/Jorinde/open/shared/lib/string";
string view_file(string file)
{
	string ret, query;

	ret = read_file(file);
	ret = replace(ret, "<", "&lt;");
	ret = replace(ret, ">", "&gt;");
	if((query = request->item("query"))) {
		ret = replace(ret, query, "<font color=\"0000ff\">"+query+"</font>");
	}
	return ret;
}

string create_linked_path(string path)
{
    int i;
    string *arr, ret, partial;
                                                                                
    if(!path) path = "/";
                                                                                
    ret = "";
    partial = "";
    arr = explode(path, "/");
    for(i = 0; i < (sizeof(arr)-1); i++) {
        arr[i] += "/";
        partial += arr[i];
        ret +=  "<a href=\"/lsp/ls.lsp?path=/" + partial + "\">" +
                arr[i] +
                "</a>&nbsp;";
    }
	ret += arr[i];
    return ret;
}

+%>
<!doctype html public "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
   <meta name="GENERATOR" content="Hands of Joakim Romland">
   <title>browse directory tree example</title>
</head>
<body>
<strong>
    Viewing file: <%=create_linked_path( request->item("file") )%><br>
</strong>

<DIV STYLE="background-color: #f0f0f0; overflow: auto; width: 700px; height:400;">
<pre><%=view_file(request->item("file")) %></pre>
</DIV>

</body>
</html>

