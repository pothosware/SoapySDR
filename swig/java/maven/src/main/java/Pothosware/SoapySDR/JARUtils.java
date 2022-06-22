// Copyright (c) 2017,2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import java.io.*;
import java.lang.reflect.Field;
import java.nio.file.*;
import java.nio.file.Paths;

class JARUtils {

    // This will never be instanced.
    private JARUtils() {}

    // Order matters.
    private static String[] libraryNames = {
        "SoapySDR",
        "SoapySDRJava"
    };

    public static Path soapyDirectory;
    private static boolean initialized = false;

    public static boolean isWindows() {
        return System.getProperty("os.name").startsWith("Win");
    }

    public static String getLibraryFileName(String libraryName) {
        if(isWindows()) {
            return (libraryName + ".dll");
        } else {
            return ("lib" + libraryName + ".so");
        }
    }

    public static Path extractResourceFromJAR (
        String resourcePath,
        boolean loadLibrary
    ) throws FileNotFoundException, IOException {
        InputStream is = JARUtils.class.getResourceAsStream("/resources/" + resourcePath);
        if(is == null) {
            throw new FileNotFoundException("Could not find resource " + resourcePath);
        }

        Path outputPath = soapyDirectory.resolve(resourcePath);
        if(!Files.exists(outputPath)) {
            outputPath.getParent().toFile().mkdirs();
            OutputStream os = new FileOutputStream(outputPath.toFile());

            // Read from resource and write to temp file.
            byte buffer[] = new byte[1024];
            int readBytes = 0;
            try {
                while((readBytes = is.read(buffer)) != -1) {
                    os.write(buffer, 0, readBytes);
                }
            } finally {
                os.close();
                is.close();
            }

            if(loadLibrary) {
                try {
                    System.load(outputPath.toString());
                } catch (UnsatisfiedLinkError e) {
                    System.err.println("Failed to load " + outputPath.toString());
                }
            }
        }

        outputPath.toFile().deleteOnExit();
        return outputPath;
    }

    public static void initialize() throws FileNotFoundException, IOException {
        /*
         * Create temporary directory for this process. Resource files
         * will be extracted here either on import or as needed.
         */
        if(soapyDirectory == null) {
            try {
                soapyDirectory = Files.createTempDirectory("SoapySDR_");
                soapyDirectory.toFile().deleteOnExit();

                for(String libraryName: libraryNames) {
                    extractResourceFromJAR(getLibraryFileName(libraryName), true);
                }
            } catch(Exception e) {
                System.err.println("Failed to initialize SoapySDR: "+e.toString());
            }
        }
    }

    static {
        try {
            initialize();
        } catch(Exception e) {
        }
    }
}
