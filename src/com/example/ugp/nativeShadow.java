package com.example.ugp;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

public class nativeShadow {
	
	private AssetManager assetManager;
	nativeShadow(AssetManager am){
		assetManager = am;
	}
	static {
		System.loadLibrary("opencv_java"); 
	    System.loadLibrary("myjni");
	  }
	
	  public int Shadow(Bitmap b){
		  
		  Mat drawing  = new Mat(b.getHeight(),b.getWidth(),CvType.CV_8U, new Scalar(4));
		  Utils.bitmapToMat(b,drawing);
		  Imgproc.cvtColor(drawing, drawing, Imgproc.COLOR_RGB2GRAY,4);
		  
		  Log.d("java check","1");
		  //AssetManager assetManager = getApplicationContext().getAssets();
		  int possible;
		  possible  = getShadow(drawing.getNativeObjAddr(),assetManager);
		  Log.d("java check","2");
		  return possible;
		  
		//  Bitmap bmp = null;
		 // Utils.matToBitmap(drawing,b);
		 // return b;
	  }
	  public native int getShadow(long mat,AssetManager assetManager);//(String path);
	  
	 
}
