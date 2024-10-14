package com.example.init;

import android.util.Log;

public class GLESNativeLib {

    static {
        System.loadLibrary("glNative");
    }

    /**
     * @param width the current opengl view window width
     * @param height the current opengl view window view height
     */
    public static native void init( String apkFilePath );
    public static native void resize(int width, int height );
    public static native void step();
}
