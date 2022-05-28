-- Copyright (c) 2021-2022 Nicholas Corgan
-- SPDX-License-Identifier: BSL-1.0

local debug = require("debug")
local ffi = require("ffi")
local lib = require("SoapySDR.Lib")
local math = require("math")

local Utility = {}

function Utility.isNil(obj)
    local objTypeName = tostring(type(obj))

    return (objTypeName == "nil")
end

function Utility.isNativeLuaType(obj)
    local objTypeName = tostring(type(obj))

    return (objTypeName == "nil") or (objTypeName == "boolean") or
           (objTypeName == "number") or (objTypeName == "string") or
           (objTypeName == "table")
end

--
-- FFI ctypes for comparison, these shouldn't be made often
--

local ffiCharPtrType = ffi.typeof("char*")
local ffiCharPtrPtrType = ffi.typeof("char**")
local ffiUnsignedIntPtrType = ffi.typeof("unsigned int*")
local ffiDoublePtrType = ffi.typeof("double*")

local ffiBoolType = ffi.typeof("bool")
local ffiChar = ffi.typeof("char")
local ffiSignedChar = ffi.typeof("signed char")
local ffiShortType = ffi.typeof("short")
local ffiIntType = ffi.typeof("int")
local ffiLongType = ffi.typeof("long")
local ffiLongLongType = ffi.typeof("long long")
local ffiInt64Type = ffi.typeof("int64_t")
local ffiUnsignedChar = ffi.typeof("unsigned char")
local ffiUnsignedShortType = ffi.typeof("unsigned short")
local ffiUnsignedIntType = ffi.typeof("unsigned int")
local ffiUnsignedLongType = ffi.typeof("unsigned long")
local ffiUnsignedLongLongType = ffi.typeof("unsigned long long")
local ffiUInt64Type = ffi.typeof("uint64_t")
local ffiFloatType = ffi.typeof("float")
local ffiDoubleType = ffi.typeof("double")

local ffiComplexFloatType = ffi.typeof("complex float")
local ffiComplexType = ffi.typeof("complex")

local ffiArgInfoType = ffi.typeof("SoapySDRArgInfo")
local ffiKwargsType = ffi.typeof("SoapySDRKwargs")
local ffiRangeType = ffi.typeof("SoapySDRRange")

local ffiArgInfoPtrType = ffi.typeof("SoapySDRArgInfo*")
local ffiKwargsPtrType = ffi.typeof("SoapySDRKwargs*")
local ffiRangePtrType = ffi.typeof("SoapySDRRange*")

local ffiStreamPtrType = ffi.typeof("SoapySDRStream*")

function Utility.isFFIBool(obj)
    return (not Utility.isNativeLuaType(obj)) and (ffi.typeof(obj) == ffiBoolType)
end

-- Note: the char types aren't triggering this, but it's enough of a corner case that it's
-- fine.
function Utility.isFFINumeric(obj)
    if Utility.isNativeLuaType(obj) then return false; end

    local ffiType = ffi.typeof(obj)

    return ((ffiType == ffiSignedCharType) or (ffiType == ffiUnsignedCharType) or
            (ffiType == ffiShortType) or (ffiType == ffiUnsignedShortType) or
            (ffiType == ffiIntType) or (ffiType == ffiUnsignedIntType) or
            (ffiType == ffiLongType) or (ffiType == ffiUnsignedLongType) or
            (ffiType == ffiLongLongType) or (ffiType == ffiUnsignedLongLongType) or
            (ffiType == ffiInt64Type) or (ffiType == ffiUInt64Type) or
            (ffiType == ffiFloatType) or (ffiType == ffiUnsignedFloatType) or
            (ffiType == ffiDoubleType) or (ffiType == ffiUnsignedDoubleType))
end

function Utility.isRawUnsignedPtr(obj)
    return (not Utility.isNativeLuaType(obj)) and (ffi.typeof(obj) == ffiUnsignedIntPtrType)
end

function Utility.isComplex(obj)
    return (not Utility.isNativeLuaType(obj)) and
           ((ffi.typeof(obj) == ffiComplexFloatType) or (ffi.typeof(obj) == ffiComplexType))
end

function Utility.isFFIRawString(obj)
    return (not Utility.isNativeLuaType(obj)) and (ffi.typeof(obj) == ffiCharPtrType)
end

function Utility.isFFIRawStringList(obj)
    return (not Utility.isNativeLuaType(obj)) and (ffi.typeof(obj) == ffiCharPtrPtrType)
end

function Utility.isFFIRawArgInfo(obj)
    return (not Utility.isNativeLuaType(obj)) and (ffi.typeof(obj) == ffiArgInfoType)
end

function Utility.isFFIRawKwargs(obj)
    return (not Utility.isNativeLuaType(obj)) and (ffi.typeof(obj) == ffiKwargsType)
end

function Utility.isFFIRawRange(obj)
    return (not Utility.isNativeLuaType(obj)) and (ffi.typeof(obj) == ffiRangeType)
end

function Utility.isFFIRawArgInfoPtr(obj)
    return (not Utility.isNativeLuaType(obj)) and (ffi.typeof(obj) == ffiArgInfoPtrType)
end

function Utility.isFFIRawKwargsPtr(obj)
    return (not Utility.isNativeLuaType(obj)) and (ffi.typeof(obj) == ffiKwargsPtrType)
end

function Utility.isFFIRawRangePtr(obj)
    return (not Utility.isNativeLuaType(obj)) and (ffi.typeof(obj) == ffiRangePtrType)
end

function Utility.isFFIRawStreamPtr(obj)
    return (not Utility.isNativeLuaType(obj)) and (ffi.typeof(obj) == ffiStreamPtrType)
end

--
-- Handling C <-> Lua types
--

local ffiTrue = ffi.new("bool", true)

function Utility.processBool(bool)
    return (bool == ffiTrue)
end

function Utility.toComplex(val)
    if Utility.isComplex(val) then return val
    else return ffi.new("complex", val, 0)
    end
end

function Utility.toString(val)
    if Utility.isNil(val) then return "" -- By default, would return "nil"
    elseif Utility.isNativeLuaType(val) then return tostring(val)
    elseif Utility.isFFIBool(val) then return tostring(Utility.processBool(val))
    elseif (ffi.typeof(val) == ffiFloatType) or (ffi.typeof(val) == ffiDoubleType) then
        local nativeVal = tonumber(val)
        if math.floor(nativeVal + 0.5) == nativeVal then return tostring(nativeVal) .. ".0"
        else                                             return tostring(nativeVal)
        end
    elseif Utility.isFFINumeric(val) then return tostring(tonumber(val))
    elseif Utility.isFFIRawString(val) then return Utility.processRawString(val)
    else return tostring(val) -- No idea what this is, hopefully this works
    end
end

function Utility.kwargsToTable(kwargs)
    local tbl = {}
    for i = 0, tonumber(kwargs.size)-1 do
        tbl[ffi.string(kwargs.keys[i])] = ffi.string(kwargs.vals[i])
    end

    return tbl
end

function Utility.tableToKwargs(tbl)
    local kwargs = ffi.gc(ffi.new("SoapySDRKwargs"), lib.SoapySDRKwargs_clear)

    for k,v in pairs(tbl) do
        local code = lib.SoapySDRKwargs_set(kwargs, tostring(k), tostring(v))
        if code ~= 0 then
            error(lib.SoapySDR_errToStr(code))
        end
    end

    return kwargs
end

function Utility.toKwargs(arg)
    local ret = nil
    local argType = tostring(type(arg))

    if Utility.isNil(arg) then ret = Utility.tableToKwargs({})
    elseif Utility.isFFIRawKwargs(arg) then ret = arg
    elseif argType == "table" then ret = Utility.tableToKwargs(arg)
    else
        ret = ffi.gc(lib.SoapySDRKwargs_fromString(tostring(arg)), lib.SoapySDRKwargs_clear)
    end

    return ret
end

function Utility.rangeToTable(range)
    local ret =
    {
        minimum = range.minimum,
        maximum = range.maximum,
        step = range.step,
    }

    return ret
end

function Utility.soapySettingToString(setting)
    return Utility.toString(setting)
end

-- See Types.hpp
function Utility.stringToSoapySetting(settingStr, settingType)
    if settingType == SoapySDR.ArgInfoType.BOOL then
        if (#settingStr == 0) or (settingStr == tostring(false)) then return false
        elseif settingStr == tostring(true) then return true
        else return (tonumber(settingStr) ~= 0)
        end
    elseif settingType == SoapySDR.ArgInfoType.INT then return math.floor(tonumber(settingStr) + 0.5)
    elseif settingType == SoapySDR.ArgInfoType.FLOAT then return tonumber(settingStr)
    else return settingStr
    end
end

function Utility.processRawString(str)
    -- Copy to a Lua string and add garbage collection for the C string.
    return ffi.string(ffi.gc(str, lib.SoapySDR_free))
end

function Utility.processRawKwargs(kwargs)
    local ret = Utility.kwargsToTable(kwargs)
    lib.SoapySDRKwargs_clear(kwargs)

    return ret
end

function Utility.__processRawArgInfo(argInfo)
    local ret =
    {
        key = ffi.string(argInfo.key),
        value = Utility.stringToSoapySetting(ffi.string(argInfo.value), argInfo.type),
        name = ffi.string(argInfo.name),
        description = ffi.string(argInfo.description),
        units = ffi.string(argInfo.units),
        argType = tonumber(argInfo.type),
        range = Utility.rangeToTable(argInfo.range),
        options = {},
        optionNames = {}
    }
    for i = 0, tonumber(argInfo.numOptions)-1 do
        ret[options][i+1] = ffi.string(argInfo.options[i])
        ret[optionNames][i+1] = ffi.string(argInfo.optionNames[i])
    end

    return ret
end

function Utility.processRawArgInfo(argInfo)
    local ret = Utility.__processRawArgInfo(argInfo)
    lib.SoapySDRArgInfo_clear(argInfo)

    return ret
end

function Utility.processRawPrimitiveList(arr, lengthPtr, ffiTypeName)
    local len = tonumber(lengthPtr[0])

    -- Copy the data into a newly allocated array. This allows the data
    -- to be indexed as an array and properly garbage-collected. Generally,
    -- these allocations shouldn't be large enough for this to be an issue.
    local ret = ffi.new(ffiTypeName .. "[?]", len)
    ffi.copy(ret, arr, ffi.sizeof(ret))
    lib.SoapySDR_free(arr)

    return ret
end

function Utility.processRawStringList(stringList, lengthPtr)
    local ret = {}
    local len = tonumber(lengthPtr[0])

    -- Copy to a Lua "array" of strings and clear the C string array.
    for i = 0,len-1 do
        ret[i+1] = ffi.string(stringList[i])
    end

    lib.SoapySDRStrings_clear(ffi.new("char**[1]", {cStrs}), len)

    return ret
end

function Utility.processRawArgInfoList(argInfoList, lengthPtr)
    local ret = {}
    local len = tonumber(lengthPtr[0])

    for i = 0,len-1 do
        ret[i+1] = Utility.__processRawArgInfo(argInfoList)
    end

    lib.SoapySDRArgInfoList_clear(argInfoList, len)

    return ret
end

function Utility.processRawKwargsList(kwargs, lengthPtr)
    local ret = {}
    local len = tonumber(lengthPtr[0])

    for i = 0,len-1 do
        ret[i+1] = Utility.kwargsToTable(kwargs[i])
    end

    lib.SoapySDRKwargsList_clear(kwargs, len)

    return ret
end

function Utility.processRawRangeList(ranges, lengthPtr)
    local ret = {}
    local len = tonumber(lengthPtr[0])

    for i = 0,len-1 do
        ret[i+1] = Utility.rangeToTable(ranges[i])
    end

    lib.SoapySDR_free(buffer)

    return ret
end

function Utility.processRawCTypeBuffer(buffer, lengthPtr)
    local ret = {}
    local len = tonumber(lengthPtr[0])

    for i = 0,len-1 do
        ret[i+1] = buffer[i]
    end

    lib.SoapySDR_free(buffer)

    return ret
end

function Utility.luaArrayToFFIArray(arr, ffiTypeName)
    local ret = ffi.new(ffiTypeName .. "[?]", #arr)

    for i = 0,#arr-1 do
        ret[i] = arr[i+1]
    end

    return ret
end

-- Note: lengthPtr is only needed for lists
function Utility.processOutput(obj, lengthPtr)
    if Utility.isNativeLuaType(obj) then return obj
    elseif Utility.isFFINumeric(obj) then return tonumber(obj)
    elseif Utility.isFFIRawString(obj) then return Utility.processRawString(obj)
    elseif Utility.isFFIRawStringList(obj) then return Utility.processRawStringList(obj, lengthPtr)
    elseif Utility.isFFIBool(obj) then return Utility.processBool(obj)
    elseif Utility.isFFIRawArgInfo(obj) then return Utility.processRawArgInfo(obj)
    elseif Utility.isFFIRawKwargs(obj) then return Utility.processRawKwargs(obj)
    elseif Utility.isFFIRawRange(obj) then return Utility.rangeToTable(obj)
    elseif Utility.isFFIRawArgInfoPtr(obj) then return Utility.processRawArgInfoList(obj, lengthPtr)
    elseif Utility.isFFIRawKwargsPtr(obj) then return Utility.processRawKwargsList(obj, lengthPtr)
    elseif Utility.isFFIRawRangePtr(obj) then return Utility.processRawRangeList(obj, lengthPtr)
    elseif Utility.isRawUnsignedPtr(obj) then return Utility.processRawCTypeBuffer(obj, lengthPtr)
    elseif Utility.isFFIRawStreamPtr(obj) then return obj
    end

    print(string.format("Warning: returning unhandled type %s.", ffi.typeof(obj)))

    return obj
end

return Utility
