#include "value.h"

// bool valuesEqual(Value a, Value b) {
//   if (a.type != b.type) return false;
//   switch (a.type) {
//     case VAL_BOOL:
//       return AS_BOOL(a) == AS_BOOL(b);
//     case VAL_INT:
//       return AS_INT(a) == AS_INT(b);
//     default:
//       return false;  // Unreachable.
//   }
// }