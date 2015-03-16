#include "ugp.h"
#include <string>
#include "include/start.h"


#ifdef __cplusplus
extern "C" {
#endif
int callme(Mat &,jobject &, JNIEnv *);

JNIEXPORT jint Java_com_example_ugp_nativeShadow_getShadow(JNIEnv *env, jobject thisObj, jlong sptr, jobject Amgr){

	__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 0");
	 //return env->NewStringUTF( "/Download/sketchpad3.png");
	 Mat& src  = *(Mat*)sptr;
	 Mat invSrc =  cv::Scalar::all(255) - src;
	 //return 2;
	 return callme(invSrc,Amgr,env);

}

#ifdef __cplusplus
}
#endif

int callme(Mat &src, jobject &Amgr, JNIEnv *env){
	Mat rsrc;
	__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 11");
	 loading(Amgr,env);
	__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 12");
	int patch_row,patch_col,overlap;
	int resize_row,resize_col;
	resize_row = 600;//src.rows;
	resize_col = 600;//src.cols;
	patch_row = 180;
	patch_col = 180;
	overlap = 0.5;

	rsrc.create(resize_row,resize_col,src.type());
	resize(src,rsrc,rsrc.size());

	vector<Mat> patch;

	int nextrow,nextcol,patch_id=0;
	nextrow = patch_row * (1-overlap);
	nextcol = patch_col * (1-overlap);
	//cout<<nextrow<<' '<<nextcol<<endl;

	for(int i=0;i<resize_row;i += nextrow){
		for(int j=0;j<resize_col; j += nextcol){
			Mat pt;//(patch_row,patch_col,rsrc.type());


			if(i+patch_row > resize_row || j+patch_col> resize_col)
				continue;

			pt = rsrc(Rect(j,i, patch_col, patch_row));
			patch.push_back(pt);

		}
	}



		//careful declared in descriptor.cpp also
		int nx,ny,n_o;
		nx = 18;	// x bins
		ny = 8;		// y bins
		n_o = 4;
		Mat hist;
		hist.create(nx,ny,CV_32FC(n_o));

		int a,b,c;
		a = b = c = 0;
		__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 3");
		for(int i=0;i<patch.size();++i){
			vector<bool> tmp;
			getDescriptor(patch[i],tmp,hist);
			match(tmp,a,b,c);
		}
		__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 4");
		int d=max(max(a,b),c),d2;

		__android_log_print(ANDROID_LOG_DEBUG, "Match a", "%d",a);
		__android_log_print(ANDROID_LOG_DEBUG, "Match b", "%d",b);
		__android_log_print(ANDROID_LOG_DEBUG, "Match c", "%d",c);


		if(a==d) return 1;
		if(b==d) return 2;
		return 3;



}
/*
int start(){
	Mat src=imread("img1.jpg"),rsrc;
	//cvtColor( src, src, CV_RGB2GRAY );
	int resize_row,resize_col,patch_row,patch_col;
	double overlap;
	resize_row = 1280;
	resize_col = 800;
	patch_row = 180; 	// this should be in multiple of resize_row 
	patch_col = 180;	// add checks to include all patch sizes
	overlap = 0.5;
	//convertScaleAbs(src,src);
	rsrc.create(resize_row,resize_col,src.type());
	resize(src,rsrc,rsrc.size());
	//convertScaleAbs(rsrc,rsrc);
	imwrite("resize.jpg",rsrc);


	vector<Mat> patch;

	int nextrow,nextcol,patch_id=0;
	nextrow = patch_row * (1-overlap);
	nextcol = patch_col * (1-overlap);
	//cout<<nextrow<<' '<<nextcol<<endl;
	
	for(int i=0;i<resize_row;i += nextrow){
		for(int j=0;j<resize_col; j += nextcol){
			Mat pt;//(patch_row,patch_col,rsrc.type());	
			
			
			if(i+patch_row > resize_row || j+patch_col> resize_col)
				continue;
			
			pt = rsrc(Rect(j,i, patch_col, patch_row));
			patch.push_back(pt);
			
		}
	}
	
	/*

	//cout<<"patches : "<<patch.size()<<endl;

	//writing patches to images
	string c = "patch/0.jpg";
	for(int i=0;i<patch.size();++i){
		ostringstream convert;  
		convert <<  i;
		c = "patch/"+convert.str()+".jpg";
		//cout<<c<<endl;
		//remove(c);
		imwrite(c,patch[i]);
	}

	vector< vector<bool> > descrip;
	
	//careful declared in descriptor.cpp also
	int nx,ny,n_o;
	nx = 18;	// x bins
	ny = 8;		// y bins
	n_o = 4;
	Mat hist;
	hist.create(nx,ny,CV_32FC(n_o));

	for(int i=0;i<patch.size();++i){
		vector<bool> tmp;
		getDescriptor(patch[i],tmp,hist);
		descrip.push_back(tmp);	
	}


	hist.release();

	// writing descriptor to images

	loading();
	//cout<<"1";
	for(int i=0;i<descrip.size();++i){
		inserting(descrip[i],0,i); 
		//cout<<"done"<<endl;
	}

	return 0;

}

*/
