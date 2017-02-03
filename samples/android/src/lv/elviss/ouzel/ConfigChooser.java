// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;

import android.opengl.GLSurfaceView;

public class ConfigChooser implements GLSurfaceView.EGLConfigChooser
{
    public ConfigChooser(int r, int g, int b, int a, int depth, int stencil, int sampleBuffers, int samples)
    {
        redSize = r;
        greenSize = g;
        blueSize = b;
        alphaSize = a;
        depthSize = depth;
        stencilSize = stencil;
        sampleBufferCount = sampleBuffers;
        sampleCount = samples;
    }

    // Minimum config, actual config is chosen by chooseConfig
    private static int EGL_OPENGL_ES2_BIT = 4;
    private static int[] configAttribs2 =
    {
        EGL10.EGL_RED_SIZE, 4,
        EGL10.EGL_GREEN_SIZE, 4,
        EGL10.EGL_BLUE_SIZE, 4,
        EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL10.EGL_NONE
    };

    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
    {
        // Get the number of minimally matching EGL configurations
        int[] numConfig = new int[1];
        egl.eglChooseConfig(display, configAttribs2, null, 0, numConfig);

        int numConfigs = numConfig[0];

        if (numConfigs <= 0)
        {
            throw new IllegalArgumentException("No configs match config spec");
        }

        // Allocate then read the array of minimally matching EGL configs
        EGLConfig[] configs = new EGLConfig[numConfigs];
        egl.eglChooseConfig(display, configAttribs2, configs, numConfigs, numConfig);

        // Return the first matching one
        for(EGLConfig config : configs)
        {
            int depth = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
            int stencil = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);
            int sampleBuffers = findConfigAttrib(egl, display, config, EGL10.EGL_SAMPLE_BUFFERS, 0);
            int samples = findConfigAttrib(egl, display, config, EGL10.EGL_SAMPLES, 0);

            // We need at least depthSize and stencilSize bits and sampleBufferCount buffers and sampleCount samples
            if (depth < depthSize || stencil < stencilSize || sampleBuffers < sampleBufferCount || samples < sampleCount)
                continue;

            // We want an exact match for red/green/blue/alpha
            int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
            int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
            int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
            int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);

            if (r == redSize && g == greenSize && b == blueSize && a == alphaSize)
                return config;
        }

        return null;
    }

    private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue)
    {
        if (egl.eglGetConfigAttrib(display, config, attribute, value))
            return value[0];

        return defaultValue;
    }

    protected int redSize;
    protected int greenSize;
    protected int blueSize;
    protected int alphaSize;
    protected int depthSize;
    protected int stencilSize;
    protected int sampleBufferCount;
    protected int sampleCount;
    private int[] value = new int[1];
}