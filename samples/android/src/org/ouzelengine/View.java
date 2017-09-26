// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

package org.ouzelengine;

import com.sun.javafx.font.directwrite.RECT;

import android.content.Context;
import android.graphics.Rect;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class View extends SurfaceView implements SurfaceHolder.Callback
{
    private final InputManager inputManager;

    public View(Context context)
    {
        super(context);
        getHolder().addCallback(this);
        inputManager = new InputManager();
        setFocusableInTouchMode(true);
    }

    @Override public boolean onTouchEvent(MotionEvent event)
    {
        return OuzelLibJNIWrapper.onTouchEvent(event);
    }

    @Override public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        inputManager.onInputEvent(event);
        OuzelLibJNIWrapper.onKeyDown(keyCode);
        return super.onKeyDown(keyCode, event);
    }

    @Override public boolean onKeyUp(int keyCode, KeyEvent event)
    {
        inputManager.onInputEvent(event);
        OuzelLibJNIWrapper.onKeyUp(keyCode);
        return super.onKeyUp(keyCode, event);
    }

    @Override public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        OuzelLibJNIWrapper.onSurfaceChanged(holder.getSurface(), width, height);
    }

    @Override public void surfaceCreated(SurfaceHolder holder)
    {
        Rect surfaceFrame = holder.getSurfaceFrame();
        OuzelLibJNIWrapper.onSurfaceCreated(holder.getSurface(), surfaceFrame.right - surfaceFrame.left, surfaceFrame.bottom - surfaceFrame.top);
    }

    @Override public void surfaceDestroyed(SurfaceHolder holder)
    {
        OuzelLibJNIWrapper.onSurfaceDestroyed();
    }
}
