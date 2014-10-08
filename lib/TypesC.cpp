// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Types.h>
#include <stdlib.h>
#include <string.h>

void SoapySDRStringList_append(SoapySDRStringList *list, const char *s)
{
    list->size++;
    list->elems = (char **)realloc(list->elems, sizeof(char *)*list->size);
    list->elems[list->size-1] = strdup(s);
}

void SoapySDRStringList_destroy(SoapySDRStringList *list)
{
    for (size_t i = 0; i < list->size; i++)
    {
        free(list->elems[i]);
    }
    free(list->elems);
    list->size = 0;
    list->elems = NULL;
}
