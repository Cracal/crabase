/**
 * @file cra_serialize.c
 * @author Cracal
 * @brief serializer
 * @version 0.1
 * @date 2024-10-09
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "cra_serialize_inner.h"
#include "cra_malloc.h"

static inline const char *
cra_ser_errcode_to_string(CraSerErr_e errcode)
{
    switch (errcode)
    {
        case CRA_SER_ERR_OK:
            return CRA_SER_ERR_OK_STR;
        case CRA_SER_ERR_NO_BUF:
            return "No buffer space available";
        case CRA_SER_ERR_NO_MEM:
            return "No memory available";
        case CRA_SER_ERR_NESTING:
            return "Nesting level is too deep";
        case CRA_SER_ERR_LENGTH_MAX:
            return "Length exceeds maximum allowed size";
        case CRA_SER_ERR_INIT_FAILED:
            return "Initialization failed";
        case CRA_SER_ERR_OUT_OF_RANGE:
            return "Out of range";
        case CRA_SER_ERR_APPEND_FAILED:
            return "Append item failed";
        case CRA_SER_ERR_INVALID_VALUE:
            return "Invalid value";
        case CRA_SER_ERR_TYPE_MISMATCH:
            return "Type mismatch";
        case CRA_SER_ERR_CANNOT_BE_NULL:
            return "Value object cannot be null";
        case CRA_SER_ERR_DUPLICATE_MEMBER:
            return "Duplicate struct members";
        case CRA_SER_ERR_SKIP_MEMBER_FAILED:
            return "Skip struct member failed";

        default:
            return "Unknown error";
    }
}

void
cra_ser_error_set(CraSerializer *szer, CraSerErr_e errcode)
{
    szer->error.err = errcode;
    const char *reason = cra_ser_errcode_to_string(errcode);
    snprintf(szer->error.msg, sizeof(szer->error.msg), CRA_SER_ERR_FMT, reason);
}

void
cra_ser_error_append(CraSerializer *szer, const CraStringView *name)
{
    assert(szer->error.err != CRA_SER_ERR_OK);

    int len = (int)strlen(szer->error.msg) - 1;
    snprintf(szer->error.msg + len, sizeof(szer->error.msg) - len, ":" CRA_SV_FMT ".", CRA_SV_ARG(*name));
}

bool
cra_check_id_unique(const CraSerMeta *meta, uint16_t *duplicate_id)
{
    assert(meta);
    assert(meta->type == CRA_SER_TYPE_STRUCT);

    CRA_SER_DEF_IDS_BITMAP(id_bitmap);
    for (uint16_t i = 0; i < meta->count; i++)
    {
        if (cra_ser_check_id_duplicate(id_bitmap, meta->for_struct.ids[i]))
        {
            if (duplicate_id)
                *duplicate_id = meta->for_struct.ids[i];
            return false;
        }
    }
    return true;
}
