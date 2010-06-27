#include "caos.h"
#include "ronnie.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR(msg) caos_set_error(context, (char*)msg)

// ~ arg_bool ~

// Comparisons

bool c_and (bool left, bool right)
{
  return left && right;
}

bool c_or (bool left, bool right)
{
  return left || right;
}

bool c_eq (CaosValue left, CaosValue right)
{
  return caos_value_equal (left, right);
}

bool c_ne (CaosValue left, CaosValue right)
{
  return !c_eq (left, right);
}

#ifndef streq
#define streq(left, right) (strcmp(left, right) == 0)
#endif

typedef bool (*caos_comparison_t) (CaosValue, CaosValue);
typedef bool (*caos_logical_t) (bool, bool);

caos_comparison_t
comparison_from_symbol (char *sym)
{
  if (streq (sym, "eq")) return c_eq;
  if (streq (sym, "ne")) return c_ne;

  return NULL;
}

caos_logical_t
logical_from_symbol (char *sym)
{
  if (streq (sym, "and")) return c_and;
  if (streq (sym, "or")) return c_or;

  return NULL;
}

bool
caos_arg_bool (CaosContext *context)
{

  CaosValue left, right;
  caos_comparison_t compare_func;
  caos_logical_t logic_func;
  bool ret, second;

  left = caos_arg_value (context);
  compare_func = comparison_from_symbol (caos_arg_symbol (context));
  right = caos_arg_value (context);
  if (caos_get_error (context)) return false;
  if (!compare_func) {
    caos_set_error (context, (char*)"No such comparison");
    return false;
  }
  
  ret = compare_func (left, right);

  while (true) {
    {
      if (caos_done (context)) break;
      CaosValue next = caos_current_token (context);
      if (!caos_value_is_symbol (next)) break;

      logic_func = logical_from_symbol (caos_value_to_symbol (next));
      if (!logic_func) break;
      caos_advance (context); // Because current_token does not advance!
    }

    left = caos_arg_value (context);
    compare_func = comparison_from_symbol (caos_arg_symbol (context));
    right = caos_arg_value (context);
    if (caos_get_error (context)) return false;
    if (!compare_func) {
      caos_set_error (context, (char*)"No such comparison");
      return false;
    }

    second = compare_func (left, right);
    ret = logic_func (ret, second);
  }

  return ret;
}

// ~ typed arg functions ~

float
caos_arg_float (CaosContext *context)
{
  CaosValue next = caos_arg_value (context);
  if (!caos_value_is_float (next)) {
    ERROR ("Expected float");
    return -42;
  }

  //printf ("[DEBUG] Float '%f'\n", caos_value_to_float (next));
  return caos_value_to_float (next);
}

int
caos_arg_int (CaosContext *context)
{
  CaosValue next = caos_arg_value (context);
  if (!caos_value_is_integer (next)) {
    ERROR ("Expected integer");
    return -42;
  }

  //printf ("[DEBUG] Int '%i'\n", caos_value_to_integer (next));
  return caos_value_to_integer (next);
}

char*
caos_arg_string (CaosContext *context)
{
  CaosValue next = caos_arg_value (context);
  if (!caos_value_is_string (next)) {
    ERROR ("Expected string");
    return NULL;
  }

  //printf ("[DEBUG] String '%s'\n", caos_value_to_string (next));
  return caos_value_to_string (next);
}

int*
caos_arg_bytestring (CaosContext *context)
{
  CaosValue next = caos_arg_value (context);
  if (!caos_value_is_bytestring (next)) {
    ERROR ("Expected byte-string");
    return NULL;
  }
  
  //printf ("[debug] bytestring\n");
  return caos_value_to_bytestring (next);
}

// ~ extra value types ~

enum RonnieType {
  CAOS_STRING = 50,
  CAOS_INT,
  CAOS_FLOAT,
  CAOS_BYTESTRING
};

CaosValue
caos_value_int (int i)
{
  CaosValue val = { CAOS_INT, i };
  return val;
}

CaosValue
caos_value_string (char *s)
{
  CaosValue val = { CAOS_STRING, (intptr_t)s };
  return val;
}

CaosValue
caos_value_float (float f)
{
  CaosValue val;
  val.type = CAOS_FLOAT;
  val.value = *(intptr_t*)&f;
  return val;
}

CaosValue
caos_value_bytestring (int *a)
{
  CaosValue val = { CAOS_BYTESTRING, (intptr_t)a };
  return val;
}

bool
caos_value_is_integer (CaosValue val)
{
  return val.type == CAOS_INT;
}

bool
caos_value_is_string (CaosValue val)
{
  return val.type == CAOS_STRING;
}

bool
caos_value_is_float (CaosValue val)
{
  return val.type == CAOS_FLOAT;
}

bool
caos_value_is_bytestring (CaosValue val)
{
  return val.type == CAOS_BYTESTRING;
}

int
caos_value_to_integer (CaosValue val)
{
  return val.value;
}

char*
caos_value_to_string (CaosValue val)
{
  return (char*)val.value;
}

float
caos_value_to_float (CaosValue val)
{
  return *(float*)&val.value;
}

int*
caos_value_to_bytestring (CaosValue val)
{
  return (int*)val.value;
}

bool
caos_value_equal (CaosValue left, CaosValue right)
{
  // TODO: Really, everything should just be numbers
  //   But we're using string pointers because it's easier right now
  if (caos_value_is_string (left) && caos_value_is_string (right)) {
    return strcmp (caos_value_to_string (left), caos_value_to_string (right)) == 0;
  }
  if (left.type != right.type) {
    return false;
  }
  return left.value == right.value;
}

// ~ Helpers ~

typedef struct RonnieScriptRef {
  CaosValue *script;
  CaosValue *i;
} RonnieScriptRef;

void ronnie_script_ref_advance (RonnieScriptRef *ref) { ref->i++; }
CaosValue ronnie_script_ref_get (RonnieScriptRef *ref) { return *ref->i; }
void ronnie_script_ref_jump (RonnieScriptRef *ref, int i) { ref->i = ref->script + i; }
int ronnie_script_ref_mark (RonnieScriptRef *ref) { return ref->i - ref->script; }

struct ICaosScript
ronnie_script_ref_iface()
{
  static struct ICaosScript iface = {
    (caos_script_advance_t)ronnie_script_ref_advance,
    (caos_script_get_t)ronnie_script_ref_get,
    (caos_script_jump_t)ronnie_script_ref_jump,
    (caos_script_mark_t)ronnie_script_ref_mark
  };
  return iface;
}

CaosContext*
ronnie_context_new (CaosRuntime *rt, CaosValue *script)
{
  RonnieScriptRef *ref = malloc (sizeof (RonnieScriptRef));
  ref->script = script;
  ref->i = script;
  return caos_context_new (rt, ref, ronnie_script_ref_iface());
}

void
ronnie_context_destroy (CaosContext *c)
{
  RonnieScriptRef *ref = caos_get_script (c);
  free (ref);
  caos_context_destroy (c);
}

CaosValue* ronnie_script_from_string (enum CaosLexerVersion version, char *source) {
  int i, m;
  i = m = 0;
  CaosLexer lexer = caos_lexer (version, source);
  CaosValue *script = NULL;

  while (true) {
    CaosValue val = caos_lexer_lex(&lexer);
    if (i == m) {
      m = m ? m << 1 : 1;
      script = (CaosValue*) realloc (script, sizeof(CaosValue) * m);
      assert (script);
    }
    script[i++] = val;

    if (caos_value_is_eoi (val)) break;
  }
  
  return script;
}
