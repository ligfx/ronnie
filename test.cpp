#include "ronnie.h"

#include <gtest/gtest.h>

#ifdef NULL
#undef NULL
#endif
#define NULL ((void*)0)

class Lexer : public ::testing::Test {
  protected:
  CaosLexError *error;
  CaosScript *script;
  
  public:
  Lexer() : error (0) {}
  virtual ~Lexer() { delete error; }
};

TEST_F (Lexer, MisleadingUnaryPlus) {
  script = caos_script_from_string (CAOS_EXODUS, &error, (char*) "+");
  
  ASSERT_NE (error, NULL);
  EXPECT_EQ (error->type, CAOS_MISLEADING_UNARY_PLUS);
}

TEST_F (Lexer, MisleadingUnaryMinus) {
  script = caos_script_from_string (CAOS_EXODUS, &error, (char*) "-");
  
  ASSERT_NE (error, NULL);
  EXPECT_EQ (error->type, CAOS_MISLEADING_UNARY_MINUS);
}

TEST_F (Lexer, UnclosedExodusString) {
  script = caos_script_from_string (CAOS_EXODUS, &error, (char*)"\"");
  
  ASSERT_NE (error, NULL);
  EXPECT_EQ (error->type, CAOS_UNCLOSED_STRING);
}
 
TEST_F (Lexer, UnclosedAlbianString) {
  script = caos_script_from_string (CAOS_ALBIA, &error, (char*)"[");
  
  ASSERT_NE (error, NULL);
  EXPECT_EQ (error->type, CAOS_UNCLOSED_STRING);
}
  
TEST_F (Lexer, UnclosedBytestring) {
  script = caos_script_from_string (CAOS_EXODUS, &error, (char*)"[");
  
  ASSERT_NE (error, NULL);
  EXPECT_EQ (CAOS_UNCLOSED_BYTESTRING, error->type);
}

TEST_F (Lexer, NonIntegerInByteString) {
  script = caos_script_from_string (CAOS_EXODUS, &error, (char*)"[hi]");
  
  ASSERT_NE (error, NULL);
  EXPECT_EQ (CAOS_BYTESTRING_EXPECTED_INTEGER, error->type);
}

TEST_F (Lexer, MisleadingSingleQuote) {
  script = caos_script_from_string (CAOS_EXODUS, &error, (char*)"'test'");
  
  ASSERT_NE (error, NULL);
  EXPECT_EQ (error->type, CAOS_MISLEADING_SINGLE_QUOTE);
}

TEST_F (Lexer, UnrecognizedCharacter) {
  script = caos_script_from_string (CAOS_EXODUS, &error, (char*)"^");
  
  ASSERT_NE (error, NULL);
  EXPECT_EQ (error->type, CAOS_UNRECOGNIZED_CHARACTER);
  EXPECT_EQ ('^', *((char*)error->data));
}

TEST_F (Lexer, LineReporting) {
  script = caos_script_from_string (CAOS_EXODUS, &error, (char*)"\n\nhi \n [6 7 \n 8]j\n \"ph\ni\" 42 [");
  
  ASSERT_NE (error, NULL);
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

TEST (Machine, Test) {
  CaosRuntime *r = caos_runtime_new();
  caos_register_function (r, (char*)"assert-eq", caos_assert_eq, 0);
  
  CaosLexError *error = 0;
  CaosScript *s = caos_script_from_string (CAOS_EXODUS, &error, (char*)"assert-eq 0 1");
  ASSERT_EQ (0, error);
  
  CaosContext *c = caos_context_new (r, s);
  
  caos_tick (c, NULL);
  ASSERT_EQ (caos_get_error (c), NULL);
}
