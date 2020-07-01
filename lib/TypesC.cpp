// Copyright (c) 2014-2020 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "ErrorHelpers.hpp"
#include "TypeHelpers.hpp"
#include <SoapySDR/Types.h>
#include <cstdlib>
#include <cstring>

extern "C" {

SoapySDRKwargs SoapySDRKwargs_fromString(const char *markup)
{
    __SOAPY_SDR_C_TRY
    return toKwargs(SoapySDR::KwargsFromString(markup));
    __SOAPY_SDR_C_CATCH_RET(SoapySDRKwargs());
}

char *SoapySDRKwargs_toString(const SoapySDRKwargs *args)
{
    __SOAPY_SDR_C_TRY
    return toCString(SoapySDR::KwargsToString(toKwargs(args)));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

void SoapySDR_free(void *ptr)
{
    std::free(ptr);
}

void SoapySDRStrings_clear(char ***elems, const size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        SoapySDR_free((*elems)[i]);
    }
    SoapySDR_free(*elems);
    *elems = NULL;
}

int SoapySDRKwargs_set(SoapySDRKwargs *args, const char *key, const char *val)
{
    for (size_t i = 0; i < args->size; i++)
    {
        if (std::strcmp(args->keys[i], key) == 0)
        {
            auto new_val = strdup(val);
            if (new_val == nullptr) return -1;
            SoapySDR_free(args->vals[i]);
            args->vals[i] = new_val;
            return 0;
        }
    }

    //increase the memory size by 1 element and assign the new pointer on success
    //the container will continue to be in a good state even after realloc failure
    auto new_keys = (char **)std::realloc(args->keys, sizeof(char *)*(args->size+1));
    auto new_vals = (char **)std::realloc(args->vals, sizeof(char *)*(args->size+1));
    if (new_keys != nullptr) args->keys = new_keys;
    if (new_vals != nullptr) args->vals = new_vals;

    //error: the current allocation has no space for the new element
    if (new_keys == nullptr or new_vals == nullptr) return -1;

    //make copies of the key and value to store
    auto new_key = strdup(key);
    auto new_val = strdup(val);

    //error: could not make a copy of the key or value string
    //free both pointers in case one of them was allocated
    if (new_key == nullptr or new_val == nullptr)
    {
        SoapySDR_free(new_key);
        SoapySDR_free(new_val);
        return -1;
    }

    //assign the new entry
    args->keys[args->size] = new_key;
    args->vals[args->size] = new_val;
    args->size++;
    return 0;
}

const char *SoapySDRKwargs_get(const SoapySDRKwargs *args, const char *key)
{
    for (size_t i = 0; i < args->size; i++)
    {
        if (strcmp(args->keys[i], key) == 0)
        {
            return args->vals[i];
        }
    }
    return NULL;
}

void SoapySDRKwargs_clear(SoapySDRKwargs *args)
{
    SoapySDRStrings_clear(&args->keys, args->size);
    SoapySDRStrings_clear(&args->vals, args->size);
    args->size = 0;
}

void SoapySDRKwargsList_clear(SoapySDRKwargs *args, const size_t length)
{
    for (size_t i = 0; i < length; i++) SoapySDRKwargs_clear(args+i);
    SoapySDR_free(args);
}

void SoapySDRArgInfo_clear(SoapySDRArgInfo *info)
{
    //clear strings
    SoapySDR_free(info->key);
    info->key = NULL;

    SoapySDR_free(info->value);
    info->value = NULL;

    SoapySDR_free(info->name);
    info->name = NULL;

    SoapySDR_free(info->description);
    info->description = NULL;

    SoapySDR_free(info->units);
    info->units = NULL;

    //clear options
    SoapySDRStrings_clear(&info->options, info->numOptions);
    SoapySDRStrings_clear(&info->optionNames, info->numOptions);
    info->numOptions = 0;
}

void SoapySDRArgInfoList_clear(SoapySDRArgInfo *info, const size_t length)
{
    for (size_t i = 0; i < length; i++) SoapySDRArgInfo_clear(info+i);
    SoapySDR_free(info);
}

}
