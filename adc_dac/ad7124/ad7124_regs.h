#ifndef __AD7124_REGS_H__
#define __AD7124_REGS_H__

#include "ad7124.h"

/*! Array holding the info for the ad7124 registers - address, initial value,
    size and access type. */
extern struct ad7124_st_reg ad7124_regs[AD7124_REG_NO];

extern struct ad7124_st_reg ad7124_regsR[AD7124_REG_NO];

extern struct ad7124_st_reg ad7124_regsW[AD7124_REG_NO];
#endif /* __AD7124_REGS_H__ */
