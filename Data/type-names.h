#ifndef TYPE_NAMES_H
#define TYPE_NAMES_H
#include "numbers.h"

//---------------------------------------------------------------
//                      Exported
//---------------------------------------------------------------
#define BASIC_TYPE(WHAT, NAME, CLASS) \
inline QString type_name(WHAT ) { return #NAME;}; \
inline Data to_data(WHAT input)     { return CLASS(input);}; \
inline void convertTo(const Data& data, NAME& target) {target = (CLASS)data;};
BASIC_TYPES
#undef BASIC_TYPE
inline QString type_name(Data ) { return "Data";};
inline Data    to_data(Data blah) {return blah;};
inline void    convertTo(const Data& data, Data& target) {target = data;};
#endif
