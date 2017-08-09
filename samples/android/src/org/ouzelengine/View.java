// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

package org.ouzelengine;

import android.content.Context;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceView;

public class View extends SurfaceView
{
    private final InputManager inputManager;

    public View(Context context)
    {
        super(context);

        inputManager = new InputManager();

        setFocusableInTouchMode(true);
    }

    @Override public boolean onTouchEvent(MotionEvent event)
    {
        switch (event.getAction() & MotionEvent.ACTION_MASK)
        {
            case MotionEvent.ACTION_POINTER_DOWN:
            {
                final int indexPointerDown = event.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                final int pointerId = event.getPointerId(indexPointerDown);
                final float x = event.getX(indexPointerDown);
                final float y = event.getY(indexPointerDown);

                OuzelLibJNIWrapper.handleActionDown(pointerId, x, y);
                return true;
            }

            case MotionEvent.ACTION_DOWN:
            {
                final int pointerId = event.getPointerId(0);
                final float x = event.getX(0);
                final float y = event.getY(0);

                OuzelLibJNIWrapper.handleActionDown(pointerId, x, y);
                return true;
            }

            case MotionEvent.ACTION_MOVE:
            {
                final int pointerId = event.getPointerId(0);
                final float x = event.getX(0);
                final float y = event.getY(0);

                OuzelLibJNIWrapper.handleActionMove(pointerId, x, y);
                return true;
            }

            case MotionEvent.ACTION_POINTER_UP:
            {
                final int indexPointUp = event.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                final int pointerId = event.getPointerId(indexPointUp);
                final float x = event.getX(indexPointUp);
                final float y = event.getY(indexPointUp);

                OuzelLibJNIWrapper.handleActionUp(pointerId, x, y);
                return true;
            }

            case MotionEvent.ACTION_UP:
            {
                final int pointerId = event.getPointerId(0);
                final float x = event.getX(0);
                final float y = event.getY(0);

                OuzelLibJNIWrapper.handleActionUp(pointerId, x, y);
                return true;
            }

            case MotionEvent.ACTION_CANCEL:
            {
                final int pointerId = event.getPointerId(0);
                final float x = event.getX(0);
                final float y = event.getY(0);

                OuzelLibJNIWrapper.handleActionCancel(pointerId, x, y);
                return true;
            }
        }

        return super.onTouchEvent(event);
    }

    @Override public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        inputManager.onInputEvent(event);

        OuzelLibJNIWrapper.handleKeyDown(keyCode);
        return super.onKeyDown(keyCode, event);
    }

    @Override public boolean onKeyUp(int keyCode, KeyEvent event)
    {
        inputManager.onInputEvent(event);
        
        OuzelLibJNIWrapper.handleKeyUp(keyCode);
        return super.onKeyUp(keyCode, event);
    }
}
