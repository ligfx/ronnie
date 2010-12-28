#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "ronnie.h"

#include <gtest/gtest.h>

void test_error_with_version (char *str, CaosLexErrorType err_type, enum CaosLexerVersion version)
{
	CaosLexError *error = NULL;
	CaosScript *script = NULL;
	
	script = caos_script_from_string (version, &error, str);
	
	ASSERT_TRUE (error);
	EXPECT_EQ (error->type, err_type);
}
#define test_error(str,err) test_error_with_version(str,err,CAOS_EXODUS)
#define test_error_albia(str,err) test_error_with_version(str,err,CAOS_ALBIA)

TEST (Lexer, MisleadingUnaryPlus)    { test_error ("+", CAOS_MISLEADING_UNARY_PLUS); }
TEST (Lexer, MisleadingUnaryMinus)   { test_error ("-", CAOS_MISLEADING_UNARY_MINUS); }
TEST (Lexer, UnclosedExodusString)   { test_error ("\"", CAOS_UNCLOSED_STRING); }
TEST (Lexer, UnclosedAlbianString)   { test_error_albia ("[", CAOS_UNCLOSED_STRING); }
TEST (Lexer, UnclosedBytestring)     { test_error ("[", CAOS_UNCLOSED_BYTESTRING); }
TEST (Lexer, NonIntegerInByteString) { test_error ("[hi]", CAOS_BYTESTRING_EXPECTED_INTEGER); }
TEST (Lexer, MisleadingSingleQuote)  { test_error ("'test'", CAOS_MISLEADING_SINGLE_QUOTE); }
TEST (Lexer, UnrecognizedCharacter)  {
	CaosLexError *error = NULL;
	CaosScript *script = NULL;
	
	script = caos_script_from_string (CAOS_EXODUS, &error, "^");
  
	ASSERT_TRUE (error);
	EXPECT_EQ (error->type, CAOS_UNRECOGNIZED_CHARACTER);
	EXPECT_EQ ('^', *((char*)error->data));
}

TEST (Lexer, LineReporting) {
	CaosLexError *error = NULL;
	CaosScript *script = NULL;
	
	script = caos_script_from_string (CAOS_EXODUS, &error, "\n\nhi \n [6 7 \n 8]j\n \"ph\ni\" 42 [");

	ASSERT_TRUE (error);
	EXPECT_EQ (CAOS_UNCLOSED_BYTESTRING, error->type);
	EXPECT_EQ (7, error->lineno);
}

std::ostream&
operator<< (std::ostream& o, CaosValue v)
{
  if (caos_value_is_integer (v)) o << caos_value_to_integer (v);
  else o << "value";
  return o;
}

void caos_assert_eq (CaosContext *c)
{
  CaosValue l = caos_arg_value(c);
  CaosValue r = caos_arg_value(c);
  if (caos_get_error (c)) return;
  
  EXPECT_PRED2 (caos_value_equal, l, r);
}

TEST (Runtime, Test) {
  CaosRuntime *r = caos_runtime_new();
  caos_register_function (r, "assert-eq", caos_assert_eq, NULL);
  
  CaosLexError *error = NULL;
  CaosScript *s = caos_script_from_string (CAOS_EXODUS, &error, "assert-eq 7 7");
  ASSERT_FALSE (error);
  
  CaosContext *c = caos_context_new (r, s);
  
  caos_tick (c, NULL);
  ASSERT_FALSE (caos_get_error (c));
}

TEST (RuntimeErrors, ExpectedCommand) {
	CaosRuntime *runtime = caos_runtime_new();
	
	CaosLexError *lex_error = NULL;
	CaosScript *script = caos_script_from_string (CAOS_EXODUS, &lex_error, "notacommand ");
	ASSERT_FALSE (lex_error);

	CaosContext *context = caos_context_new (runtime, script);

	// Expected command, got Symbol:notacommand, at line 1
	caos_tick (context, NULL);
	CaosError *error = NULL;
	error = caos_get_error (context);
	ASSERT_TRUE (error);  

	EXPECT_EQ (CAOS_EXPECTED_COMMAND, error->type);
	EXPECT_PRED1 (caos_value_is_symbol, error->token);
	EXPECT_STREQ ("notacommand", caos_value_to_symbol (error->token));
}

void c_outs (CaosContext *context)
{ 
	char *string = caos_arg_string (context);
	if (caos_get_error (context)) return;
}

TEST (RuntimeErrors, ExpectedExpression) {
	CaosRuntime *runtime = caos_runtime_new();
	caos_register_function (runtime, "outs", c_outs, NULL);
	
	
	CaosLexError *lex_error = NULL;
	CaosScript *script = caos_script_from_string (CAOS_EXODUS, &lex_error, "outs outs");
	ASSERT_FALSE (lex_error);
	
	CaosContext *context = caos_context_new (runtime, script);
	
	caos_tick (context, NULL);
	CaosError *error = NULL;
	error = caos_get_error (context);
	ASSERT_TRUE (error);  
	
	EXPECT_EQ (CAOS_EXPECTED_EXPRESSION, error->type);
	EXPECT_PRED1 (caos_value_is_symbol, error->token);
	EXPECT_STREQ ("outs", caos_value_to_symbol (error->token));
}

void
c_doif (CaosContext *context) {
	bool match = caos_arg_bool (context);
	if (caos_get_error (context)) return;
	
	if (!match)
		caos_fast_forward (context, "elif", "else", "endi", 0);
}

TEST (RuntimeErrors, FailedToFastForward) {
	CaosRuntime *runtime = caos_runtime_new();
	caos_register_function (runtime, "doif", c_doif, NULL);
	
	
	CaosLexError *lex_error = NULL;
	CaosScript *script = caos_script_from_string (CAOS_EXODUS, &lex_error, "doif 0 = 4");
	ASSERT_FALSE (lex_error);
	
	CaosContext *context = caos_context_new (runtime, script);
	
	caos_tick (context, NULL);
	CaosError *error = NULL;
	error = caos_get_error (context);
	ASSERT_TRUE (error);  
	
	EXPECT_EQ (CAOS_FAILED_TO_FAST_FORWARD, error->type);
	EXPECT_PRED1 (caos_value_is_eoi, error->token);
}