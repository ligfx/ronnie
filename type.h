#ifndef CAOS_TYPE_H
#define CAOS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CaosType {
  CAOS_STRING,
  CAOS_INT,
  CAOS_SYMBOL,
  CAOS_EOI
} CaosType;

#ifdef __cplusplus
}
#endif

#endif // CAOS_TYPE_H
