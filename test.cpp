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
