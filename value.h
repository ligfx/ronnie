#ifndef CAOS_VALUE_H
#define CAOS_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"

#include <stdbool.h>
#include <stdint.h>



/*
  Types:
    string
    integer
    float
    agent

    variable
    boolean
    array-string

  String:
    Map of "quarks" - mapping integers to strings
    Variables tell quark-map that they have a reference
    Every couple thousand ticks or so, the quark-map will free all the strings
      that aren't being referenced
    When a string is added to the quark-map, the user can keep ownership or not
      (free should or should not be called)

    Or we could just use pointers?

  Integer:
    BAM!

  Floats:
    I'm feeling like a map would be overkill.
    Can we shove floats into an integer?

  Agent:
    Agents are in a data-oriented framework, identified by a single UUID,
      which is a single integer

  Variables:
    Tricky, probably solved with some sort of enum.
    e.g. 0 is VA00, 100 is OV00, 200 is MV00, um?
    dunno how MAME NAME GAME and EAME work - need another map of some sort
      300 for MAME#0, 400 for NAME#0, etc.?

    Or we could use a pointer?

  Boolean:
    0 or 1

  Array-string (bytestring):
    Same as a string?
    Functions can tokenize it when they need it? Maybe more efficient to
      tokenize ahead of time?
*/

typedef struct CaosValue {
  CaosType type;
  intptr_t value;
} CaosValue;

CaosValue caos_value_int_new (int);
CaosValue caos_value_string_new (char*);
CaosValue caos_value_null ();

bool caos_value_is_integer (CaosValue);
bool caos_value_is_string (CaosValue);

int caos_value_as_integer (CaosValue);
char* caos_value_as_string (CaosValue);

bool caos_value_equal (CaosValue, CaosValue);

#ifdef __cplusplus
}
#endif

#endif // CAOS_VALUE_H
