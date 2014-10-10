// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Types.h>
#include <cstdlib>
#include <cstring>

extern "C" {

void SoapySDRStrings_clear(char ***elems, const size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        free((*elems)[i]);
    }
    free(*elems);
    *elems = NULL;
}

void SoapySDRKwargs_set(SoapySDRKwargs *args, const char *key, const char *val)
{
    for (size_t i = 0; i < args->size; i++)
    {
        if (strcmp(args->keys[i], key) == 0)
        {
            free(args->vals[i]);
            args->vals[i] = strdup(val);
            return;
        }
    }

    args->size++;
    args->keys = (char **)realloc(args->keys, sizeof(char *)*args->size);
    args->vals = (char **)realloc(args->vals, sizeof(char *)*args->size);

    args->keys[args->size-1] = strdup(key);
    args->vals[args->size-1] = strdup(val);
}

const char *SoapySDRKwargs_get(SoapySDRKwargs *args, const char *key)
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

}
