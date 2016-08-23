// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Types.hpp>
#include <cstdlib>
#include <cstdio>

bool checkArgsInLhs__(const SoapySDR::Kwargs &lhs, const SoapySDR::Kwargs &rhs)
{
    const auto strLhs = SoapySDR::KwargsToString(lhs);
    const auto strRhs = SoapySDR::KwargsToString(rhs);
    for (const auto &pair : lhs)
    {
        if (rhs.count(pair.first) == 0)
        {
            printf("FAIL: {%s}[%s] not found in {%s}\n",
                strLhs.c_str(), pair.first.c_str(), strRhs.c_str());
            return false;
        }
        if (pair.second != rhs.at(pair.first))
        {
            printf("FAIL: {%s}[%s] != {%s}[%s]\n",
                strLhs.c_str(), pair.first.c_str(), strRhs.c_str(), pair.first.c_str());
            return false;
        }
    }
    return true;
}

int main(void)
{
    #define checkArgsEq(lhs, rhs) \
    { \
        printf("Test line %d\n", __LINE__); \
        if (not checkArgsInLhs__(lhs, rhs)) return EXIT_FAILURE; \
        if (not checkArgsInLhs__(rhs, lhs)) return EXIT_FAILURE; \
    }

    //string to args - empty string
    checkArgsEq(SoapySDR::KwargsFromString(""), SoapySDR::Kwargs());
    checkArgsEq(SoapySDR::KwargsFromString(" "), SoapySDR::Kwargs());

    //single keyword
    SoapySDR::Kwargs args0;
    args0["Foo"] = "Bar";
    checkArgsEq(SoapySDR::KwargsFromString("Foo=Bar"), args0);
    checkArgsEq(SoapySDR::KwargsFromString(" Foo = Bar "), args0);
    checkArgsEq(SoapySDR::KwargsFromString(" Foo = Bar, "), args0);

    //two keywords
    SoapySDR::Kwargs args1;
    args1["Foo"] = "Bar";
    args1["Baz"] = "123";
    checkArgsEq(SoapySDR::KwargsFromString("Foo=Bar, Baz=123"), args1);
    checkArgsEq(SoapySDR::KwargsFromString("Baz=123,Foo = Bar "), args1);
    checkArgsEq(SoapySDR::KwargsFromString("Baz=123,Foo = Bar , "), args1);

    //empty value
    SoapySDR::Kwargs args2;
    args2["Foo"] = "Bar";
    args2["Baz"] = "";
    checkArgsEq(SoapySDR::KwargsFromString("Foo=Bar, Baz="), args2);
    checkArgsEq(SoapySDR::KwargsFromString("Baz=,Foo = Bar "), args2);
    checkArgsEq(SoapySDR::KwargsFromString("Baz= ,Foo = Bar , "), args2);
    checkArgsEq(SoapySDR::KwargsFromString("Baz ,Foo = Bar"), args2);
    checkArgsEq(SoapySDR::KwargsFromString("Baz,Foo = Bar"), args2);

    //loopback arg to markup to arg
    checkArgsEq(SoapySDR::KwargsFromString(SoapySDR::KwargsToString(SoapySDR::Kwargs())), SoapySDR::Kwargs());
    checkArgsEq(SoapySDR::KwargsFromString(SoapySDR::KwargsToString(args0)), args0);
    checkArgsEq(SoapySDR::KwargsFromString(SoapySDR::KwargsToString(args1)), args1);
    checkArgsEq(SoapySDR::KwargsFromString(SoapySDR::KwargsToString(args2)), args2);

    printf("DONE!\n");
    return EXIT_SUCCESS;
}
