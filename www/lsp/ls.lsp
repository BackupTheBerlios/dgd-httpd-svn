<%+
# include "../../jorinde.h"

string ls(string path)
{
	mixed	**files;
	string	*names, ret;
	int		i, *sizes, *times;

	/* On Igor: files = get_dir_info(path + "*"); */
	files = get_dir(path + "*");
    names   = files[0];
    sizes   = files[1];
    times   = files[2];

	ret = "";
	for(i = 0; i < sizeof(names); i++) {
		ret += "<tr>";
		ret += "<td align=right>" + (sizes[i]!=-2 ? sizes[i] : " ") + "</td>";
		ret += "<td align=right>" + ctime(times[i])[4..15] + "</td>";
		ret += "<td>";
		if(sizes[i] < 0) {
			ret +=	"<a href=\"/lsp/ls.lsp?path=" + path + names[i] + "/\">" +
					names[i] + 
					"</a><br>";
		} else {
			ret +=	"<a href=\"/lsp/view.lsp?file=" + path + names[i] + "\">" +
					names[i] + 
					"</a><br>";
		}
		ret += "</td>";
		ret += "</tr>";
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
	for(i = 0; i < sizeof(arr); i++) {
		arr[i] += "/";
		partial += arr[i];
		ret +=	"<a href=\"/lsp/ls.lsp?path=/" + partial + "\">" +
				arr[i] +
				"</a>&nbsp;";
	}
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
	ls -l <%=create_linked_path( request->item("path") )%><br>
</strong>

<DIV STYLE="background-color: #f0f0f0; overflow: auto; width: 500px; height:400;">
<table width="100%">
<tr>
		<td align=right style="border-bottom: solid 1px black;" width="15%"><i>size</i></td>
		<td align=right style="border-bottom: solid 1px black;" width="30%"><i>modified</i></td>
		<td style="border-bottom: solid 1px black;"><i>filename</i></td>
	</tr>
	<%=ls( (request->item("path") ? request->item("path") : "//") ) %>
	<tr>
		<td colspan=4 style="border-top: solid 1px black;">&nbsp;</td>
	</tr>
</table>
</DIV>

</body>
</html>

