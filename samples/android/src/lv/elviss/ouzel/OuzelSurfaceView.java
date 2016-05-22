// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

public class OuzelSurfaceView extends GLSurfaceView
{
	public OuzelSurfaceView(Context context)
	{
	    super(context);
	    
        this.setFocusableInTouchMode(true);
    }
    
    @Override
    public boolean onTouchEvent(final MotionEvent motionEvent)
    {
        switch (motionEvent.getAction() & MotionEvent.ACTION_MASK)
        {
            case MotionEvent.ACTION_POINTER_DOWN:
            {
                final int indexPointerDown = motionEvent.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                final int pointerId = motionEvent.getPointerId(indexPointerDown);
                final float x = motionEvent.getX(indexPointerDown);
                final float y = motionEvent.getY(indexPointerDown);

                this.queueEvent(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        OuzelLibJNIWrapper.handleActionDown(pointerId, x, y);
                    }
                });
                break;
            }

            case MotionEvent.ACTION_DOWN:
            {
                final int pointerId = motionEvent.getPointerId(0);
                final float x = motionEvent.getX(0);
                final float y = motionEvent.getY(0);

                this.queueEvent(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        OuzelLibJNIWrapper.handleActionDown(pointerId, x, y);
                    }
                });
                break;
            }

            case MotionEvent.ACTION_MOVE:
            {
                final int pointerId = motionEvent.getPointerId(0);
                final float x = motionEvent.getX(0);
                final float y = motionEvent.getY(0);

                this.queueEvent(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        OuzelLibJNIWrapper.handleActionMove(pointerId, x, y);
                    }
                });
                break;
            }

            case MotionEvent.ACTION_POINTER_UP:
            {
                final int indexPointUp = motionEvent.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                final int pointerId = motionEvent.getPointerId(indexPointUp);
                final float x = motionEvent.getX(indexPointUp);
                final float y = motionEvent.getY(indexPointUp);

                this.queueEvent(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        OuzelLibJNIWrapper.handleActionUp(pointerId, x, y);
                    }
                });
                break;
            }

            case MotionEvent.ACTION_UP:
            {
                final int pointerId = motionEvent.getPointerId(0);
                final float x = motionEvent.getX(0);
                final float y = motionEvent.getY(0);

                this.queueEvent(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        OuzelLibJNIWrapper.handleActionUp(pointerId, x, y);
                    }
                });
                break;
            }

            case MotionEvent.ACTION_CANCEL:
            {
                final int pointerId = motionEvent.getPointerId(0);
                final float x = motionEvent.getX(0);
                final float y = motionEvent.getY(0);

                this.queueEvent(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        OuzelLibJNIWrapper.handleActionCancel(pointerId, x, y);
                    }
                });
                break;
            }
        }
        return true;
    }
}
