#ifndef CAOS_TYPE_H
#define CAOS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CaosType {
  CAOS_STRING,
  CAOS_INT,
  CAOS_BOOL,
  CAOS_SYMBOL,
  CAOS_FLOAT,
  CAOS_EOI,
  CAOS_NULL
} CaosType;

#ifdef __cplusplus
}
#endif

#endif // CAOS_TYPE_H
