package lv.elviss.ouzel;

public class GameLibJNIWrapper
{
	static
	{
		System.loadLibrary("sprite");
	}

	public static native void on_surface_created();

	public static native void on_surface_changed(int width, int height);

	public static native void on_draw_frame();
}