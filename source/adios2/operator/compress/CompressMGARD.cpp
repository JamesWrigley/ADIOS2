/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * CompressMGARD.cpp :
 *
 *  Created on: Aug 3, 2018
 *      Author: William F Godoy godoywf@ornl.gov
 */

#include "CompressMGARD.h"

#include <cstring> //std::memcpy

extern "C" {
#include <mgard_capi.h>
}

#include "adios2/helper/adiosFunctions.h"

namespace adios2
{
namespace core
{
namespace compress
{

CompressMGARD::CompressMGARD(const Params &parameters, const bool debugMode)
: Operator("mgard", parameters, debugMode)
{
}

size_t CompressMGARD::Compress(const void *dataIn, const Dims &dimensions,
                               const size_t elementSize, const std::string type,
                               void *bufferOut, const Params &parameters) const
{
    const size_t ndims = dimensions.size();
    if (m_DebugMode)
    {
        if (ndims != 2)
        {
            throw std::invalid_argument(
                "ERROR: ADIOS2 MGARD compression: no more "
                "than 2-dimensions is supported.\n");
        }
    }

    // set type
    int mgardType = -1;
    if (type == "float")
    {
        if (m_DebugMode)
        {
            throw std::invalid_argument(
                "ERROR: ADIOS2 operator "
                "MGARD only supports double precision, in call to Put\n");
        }
    }
    else if (type == "double")
    {
        mgardType = 1;
    }

    int r[ndims];

    for (auto i = 0; i < ndims; i++)
    {
        r[ndims - i - 1] = static_cast<int>(dimensions[i]);
    }

    // Parameters
    auto itTolerance = parameters.find("tolerance");
    if (m_DebugMode)
    {
        if (itTolerance == parameters.end())
        {
            throw std::invalid_argument("ERROR: missing mandatory parameter "
                                        "tolerance for MGARD compression "
                                        "operator, in call to Put\n");
        }
    }

    double tolerance = std::stod(itTolerance->second);

    int sizeOut = 0;
    unsigned char *dataOutPtr =
        mgard_compress(mgardType, const_cast<void *>(dataIn), &sizeOut, r[0],
                       r[1], &tolerance);

    const size_t sizeOutT = static_cast<size_t>(sizeOut);
    std::memcpy(bufferOut, dataOutPtr, sizeOutT);

    return sizeOutT;
}

size_t CompressMGARD::Decompress(const void *bufferIn, const size_t sizeIn,
                                 void *dataOut, const Dims &dimensions,
                                 const std::string type,
                                 const Params & /*parameters*/) const
{
    int mgardType = -1;
    size_t elementSize = 0;

    if (type == "float")
    {
        if (m_DebugMode)
        {
            throw std::invalid_argument(
                "ERROR: ADIOS2 operator "
                "MGARD only supports double precision, in call to Get\n");
        }
    }
    else if (type == "double")
    {
        mgardType = 1;
        elementSize = 8;
    }
    else
    {
        if (m_DebugMode)
        {
            throw std::invalid_argument(
                "ERROR: ADIOS2 operator "
                "MGARD only supports double precision, in call to Get\n");
        }
    }

    const size_t ndims = dimensions.size();
    int r[ndims];

    for (auto i = 0; i < ndims; i++)
    {
        r[ndims - i - 1] = static_cast<int>(dimensions[i]);
    }

    void *dataPtr = mgard_decompress(
        mgardType,
        reinterpret_cast<unsigned char *>(const_cast<void *>(bufferIn)),
        static_cast<int>(sizeIn), r[0], r[1]);

    const size_t dataSizeBytes = helper::GetTotalSize(dimensions) * elementSize;
    std::memcpy(dataOut, dataPtr, dataSizeBytes);

    return static_cast<size_t>(dataSizeBytes);
}

} // end namespace compress
} // end namespace core
} // end namespace adios2
