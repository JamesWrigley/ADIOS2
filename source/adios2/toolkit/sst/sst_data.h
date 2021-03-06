#ifndef _SST_DATA_H_
#define _SST_DATA_H_

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

struct _SstFullMetadata
{
    int WriterCohortSize;
    struct _SstData **WriterMetadata;
    void **DP_TimestepInfo;
};

struct _SstData
{
    size_t DataSize;
    char *block;
};

struct _SstBlock
{
    size_t BlockSize;
    char *BlockData;
};

#define SST_FOREACH_PARAMETER_TYPE_4ARGS(MACRO)                                \
    MACRO(MarshalMethod, MarshalMethod, size_t, 1)                             \
    MACRO(RegistrationMethod, RegMethod, size_t, 0)                            \
    MACRO(DataTransport, String, char *, NULL)                                 \
    MACRO(RendezvousReaderCount, Int, int, 1)                                  \
    MACRO(QueueLimit, Int, int, 0)                                             \
    MACRO(QueueFullPolicy, QueueFullPolicy, size_t, 0)                         \
    MACRO(IsRowMajor, IsRowMajor, int, 0)                                      \
    MACRO(ControlTransport, String, char *, NULL)                              \
    MACRO(NetworkInterface, String, char *, NULL)                              \
    MACRO(CompressionMethod, CompressionMethod, size_t, 0)

typedef enum {
    SstRegisterFile,
    SstRegisterScreen,
    SstRegisterCloud
} SstRegistrationMethod;

struct _SstParams
{
#define declare_struct(Param, Type, Typedecl, Default) Typedecl Param;
    SST_FOREACH_PARAMETER_TYPE_4ARGS(declare_struct)
#undef declare_struct
};

#endif /* !_SST_DATA_H_ */
