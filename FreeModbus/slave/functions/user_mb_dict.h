#ifndef _USER_MB_DICT_
#define _USER_MB_DICT_

#include "user_mb_app.h"
#include "user_mb_map.h"
#include "mbconfig.h"
#include "port.h"

#define SLAVE_PROTOCOL_TYPE_ID    0

#if MB_FUNC_READ_INPUT_ENABLED > 0

extern const sMBIndexTable psSRegInTable[];

#endif

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 \
    || MB_FUNC_READ_HOLDING_ENABLED > 0 || MB_FUNC_READWRITE_HOLDING_ENABLED > 0

extern const sMBIndexTable psSRegHoldTable[];

#endif

#if MB_FUNC_READ_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0

extern const sMBIndexTable psSCoilTable[];

#endif

#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0

extern const sMBIndexTable psSDiscInTable[];

#endif

#if MB_FUNC_CPN_READ_ENABLED > 0

extern const sMBIndexTable psSCPNTable[];

#endif

#endif