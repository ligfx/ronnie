#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <ronnie/ronnie.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <list>
#include <string>

int main () {
	std::string str = "new:     simp 1 3 4 -\"flower.c16\" 4 5 6 * this is a comment\n doif 5 = 6 outs \"hi there!\" endi";
	CaosLexer lexer = caos_lexer (CAOS_EXODUS, str.c_str());
	std::list<CaosValue> values;
	
	while (true)
	{
		CaosValue val = caos_lexer_lex (&lexer);
		if (caos_value_is_eoi (val)) break;
		values.push_back (val);
	}
	
	for (std::list<CaosValue>::reverse_iterator i = values.rbegin();
		 i != values.rend();
		 ++i)
	{
		CaosValue val = *i;
		
		char *type = NULL;
		if (caos_value_is_symbol (val)) type = "symbol";
		if (caos_value_is_integer (val)) type = "integer";
		if (caos_value_is_string (val)) type = "string";
		if (caos_value_is_error (val)) type = "error";
		if (caos_value_is_comment (val)) type = "comment";
		assert (type);
		
		str.insert (val.location + val.extent, "</span>");
		str.insert (val.location, std::string("<span class=") + type + ">");
	}
	
	size_t pos = std::string::npos;
	while ((pos = str.find("\n")) != std::string::npos) {
		str.replace (pos, 2, "  ");
		str.insert (pos, "<br/>");
	}
	
	printf ("%s\n", str.c_str());
}