<%+
# include "../../jorinde.h"
inherit LIB_HTTP_STRING;

int cost;

string parse(string grammar, string to_parse)
{
	string str, err;

	if(!strlen(grammar)) {
		return "no grammar";
	}
	
	grammar = replace(grammar, "\r", "");
	grammar = replace(grammar, "\n", " ");
	
	grammar = replace(grammar, "\\b", "\b");
	grammar = replace(grammar, "\\n", "\n");
	grammar = replace(grammar, "\\r", "\r");
	grammar = replace(grammar, "\\t", "\t");
	grammar = replace(grammar, "\\\\", "\\");

	response->write("grammar:<br>" + grammar + "<br>");
	err = catch (
		str = make_string( parse_string(grammar, to_parse ) )
	);

	if(err) {
		str = "<strong>" + err + "</strong>";
	}

	return str;
}

mixed *craft_array(string *arr)
{
	return ({ arr  });
}


+%>
<!doctype html public "-//w3c//dtd html 4.0 transitional//en">
<html>
<head>
   <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
   <meta name="GENERATOR" content="Hands of Joakim Romland">
   <title>parse_string test</title>
</head>
<body>
test parse_string<br><br>

enter grammar in 1st box, end text to parse in 2nd<br><br>
<form method="POST" action="parse.lsp">

	<textarea style="width: 400px; height: 250px;" name="grammar"><%=request->form_string("grammar")%></textarea>

	<textarea style="width: 400px; height: 100px;" name="input"><%=request->form_string("input")%></textarea>

	<input type="submit" value="..">
</form>

<%
	rlimits(10; 100000) {
		cost = AssessCost();
		response->write( "<br>result:<br>" + 
				parse(	request->form_string("grammar"), 
						request->form_string("input")
					)
		);
		cost = GetCost(cost);
	}
	
	response->write("<br><br>cost:<br>" + cost + 
					" (for " + strlen( request->form_string("input") ) + 
					" bytes)<br>");

/*
whitespace = /[\b\n\r\t ]+/
word       = /[a-zA-Z]+/
sentence   : sentence word
sentence   : word

foo bar

---------------

number = /[0-9]+/
whitespace = /[\b\n\r\t ]+/
Expr : number
Expr : Expr '+' Expr

75 + 30

----------------------

whitespace = /[\b\n\r\t ]+/
word = /[a-zA-Z]+/
sentence : 'put' obj 'in' obj
obj : adjective word ?  craft_array
obj : word ?  craft_array
adjective : word word ? craft_array
adjective : word  ? craft_array

put the goddamn ball in the fucking box

---------------------------
whitespace = /[ .,!`~!@#$%\\^&*()\\-_=+\\\\|[\\]{}:;\"'<,>./?\b\r\n\t]/
word       = /([a-z]|[A-Z]|[0-9])+/
sentence   : sentence word
sentence   : word

*/
%>


</body>
</html>

