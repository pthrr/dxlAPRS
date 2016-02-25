/* XDS v2.60: Copyright (c) 1999-2011 Excelsior, LLC. All Rights Reserved. */
/* Generated by XDS Modula-2 to ANSI C v4.20 translator */

#ifndef LowReal_H_
#define LowReal_H_
#ifndef X2C_H_
#include "X2C.h"
#endif

#define LowReal_radix 2

#define LowReal_places 32

#define LowReal_expoMin (-126)

#define LowReal_expoMax 127

#define LowReal_large 3.402823E38

#define LowReal_small 1.175494E-38

#define LowReal_IEEE 1

#define LowReal_ISO 0

#define LowReal_rounds 1

#define LowReal_gUnderflow 0

#define LowReal_exception 1

#define LowReal_extend 1

#define LowReal_nModes 6

typedef X2C_CARD8 LowReal_Modes;

extern X2C_INT32 LowReal_exponent(X2C_REAL);

extern X2C_REAL LowReal_fraction(X2C_REAL);

extern X2C_REAL LowReal_sign(X2C_REAL);

extern X2C_REAL LowReal_succ(X2C_REAL);

extern X2C_REAL LowReal_ulp(X2C_REAL);

extern X2C_REAL LowReal_pred(X2C_REAL);

extern X2C_REAL LowReal_intpart(X2C_REAL);

extern X2C_REAL LowReal_fractpart(X2C_REAL);

extern X2C_REAL LowReal_scale(X2C_REAL, X2C_INT32);

extern X2C_REAL LowReal_trunc(X2C_REAL, X2C_INT32);

extern X2C_REAL LowReal_round(X2C_REAL, X2C_INT32);

extern X2C_REAL LowReal_synthesize(X2C_INT32, X2C_REAL);

extern void LowReal_setMode(X2C_CARD8);

extern X2C_CARD8 LowReal_currentMode(void);

extern X2C_BOOLEAN LowReal_IsLowException(void);


extern void LowReal_BEGIN(void);


#endif /* LowReal_H_ */