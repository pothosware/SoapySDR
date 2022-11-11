// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using NUnitLite;
using System.Reflection;

internal class TestRunner
{
    // Run the NUnit infrastructure from a standalone executable rather than
    // calling the NUnit app on a DLL.
    public static int RunNUnitTest(string testFixtureName)
    {
        string[] args = new string[]
        {
            string.Format("--test={0}", testFixtureName),
            "--noresult"
        };

        return new AutoRun(Assembly.GetExecutingAssembly()).Execute(args);
    }
}