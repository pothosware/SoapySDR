// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import java.util.AbstractMap;

class Utility
{
    static Kwargs toKwargs(AbstractMap<String, String> args)
    {
        if(args instanceof Kwargs)
            return (Kwargs)args;

        Kwargs kwargs = new Kwargs();
        for(AbstractMap.Entry<String, String> entry: args.entrySet())
            kwargs.put(entry.getKey(), entry.getValue());

        return kwargs;
    }
}
