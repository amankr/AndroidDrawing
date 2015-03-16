
#include "ugp.h"


inline float dist(float x1,float y1,float x2, float y2){
	return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
}
void GetOrientations(Mat & src,Mat & dst){
	int kernel_size = 3;
  	int scale = 1;
  	int delta = 0;
  	int ddepth = CV_32F;
  	Mat grad_x, grad_y;
  	Sobel( src, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  	Sobel( src, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );

  	float *px,*py,*pd;
  	dst.create(src.size(),ddepth);
    for(int i=0;i<src.rows;++i){
    	px = grad_x.ptr<float>(i);
    	py = grad_y.ptr<float>(i);
    	pd = dst.ptr<float>(i);
  		for(int j=0;j<src.cols;++j){
  			if(px[j]==0)
  				pd[j] = M_PI/2;
  			else	
  				pd[j] = atan(py[j]/px[j]);
  		}
  	}

}
void edgeMake(Mat & E,Mat & A,Mat & src,Mat & dst,bool did){
	dst.create(src.size(),CV_32F);
	int scale = 1;
  	int delta = 0;
  	int ddepth = CV_32F;
  	Mat grad_x, grad_y;
  	Sobel( src, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  	Sobel( src, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  	float X,Y;
  	for(int i=0;i<src.rows;++i){
  		for(int j=0;j<src.cols;++j){
  			if(did){
  				X = /*cos(A.at<float>(i,j))*cos(A.at<float>(i,j))*/ grad_x.at<float>(i,j);//* E.at<float>(i,j);
  				Y = /*sin(A.at<float>(i,j))*sin(A.at<float>(i,j))*/ grad_y.at<float>(i,j);//* E.at<float>(i,j);
  			}
  			else{
  				X = cos(A.at<float>(i,j))*cos(A.at<float>(i,j))* grad_x.at<float>(i,j)* E.at<float>(i,j);
  				Y = sin(A.at<float>(i,j))*sin(A.at<float>(i,j))* grad_y.at<float>(i,j)* E.at<float>(i,j);	
  			}
  			dst.at<float>(i,j) = dist(X,Y,0,0);
  		}
  	}
  	convertScaleAbs(dst,dst);
}

void Rotate(Mat &src,Mat &angle,Mat &Rx,Mat &Ry){
	int cx,cy;
	cx = src.rows/2;
	cy = src.cols/2;
	int depth = CV_32F;
	Rx.create(src.size(),depth);
	Ry.create(src.size(),depth);
	int X,Y;
	float *ang,*rx,*ry,theta;
	for(int i=0;i<src.rows;++i){
		ang = angle.ptr<float>(i);
		rx = Rx.ptr<float>(i);
		ry = Ry.ptr<float>(i);
		for(int j=0;j<src.cols;++j){
			X = i-cx;
			Y = j-cy;
			theta = ang[j];
			rx[j]  = cos(theta)*X - sin(theta)*Y;
			ry[j]  = sin(theta)*X + cos(theta)*Y; 
		}
	}

	
}

void minMAx(Mat &X, float &Xstart, float &Xlen,int unit){
	double minVal,maxVal;
	Point minLoc,maxLoc;
	
	minMaxLoc( X, &minVal, &maxVal, &minLoc, &maxLoc );
	Xstart = minVal;
	Xlen = (maxVal - minVal)/(float)unit;
}

void createHist(Mat &src,Mat &angle,Mat &X,Mat &Y, Mat &hist){
	int nx,ny,n_o;
	nx = 18;	// x bins
	ny = 8;		// y bins
	n_o = 4; 	// theta bins
	
	//hist.create(nx,ny,CV_32FC(n_o));
	
	float *data = (float *)(hist.data);
	for(int i=0;i<nx;++i)
		for(int j=0;j<ny;++j)
			for(int ch=0;ch<n_o;++ch)
				hist.ptr<float>(i)[n_o*j+ch] = 0.0;
			//	data[hist.step * i + j + ch] = 0.0;


	float Xlen,Ylen,Tlen;  // unit lengths
	float Xstart,Ystart,Tstart; 

	minMAx(X,Xstart,Xlen,nx);
	minMAx(Y,Ystart,Ylen,ny);
	minMAx(angle,Tstart,Tlen,n_o);

	int store = src.type();
	src.convertTo(src,CV_32F);

	float *dsrc,*xdata,*ydata,*tdata;
	int hx,hy,ht; 
	data = (float *)(hist.data);

	for(int i=0;i<src.rows;++i){
		dsrc = src.ptr<float>(i);
		xdata = X.ptr<float>(i);
		ydata = Y.ptr<float>(i);
		tdata = angle.ptr<float>(i);

		for(int j=0;j<src.cols;++j){
			if(Xlen<1e-9)
				hx = 0;
			else
				hx = (xdata[j] - Xstart) / Xlen;
			if(Ylen<1e-9)
				hy = 0;
			else
				hy = (ydata[j] - Ystart) / Ylen;
			if(Tlen<1e-9)
				ht = 0;
			else
				ht = (tdata[j] - Tstart) / Tlen;

			hist.ptr<float>(hx)[n_o*hy + ht] += dsrc[j];
		}
	}

}

inline bool check(int x,int y,int z,int a, int b,int c){
	return (x>=0 && y>=0 && z>=0 && x<a && y<b && z<c);
}
void histBlur(Mat &src){
	Mat temp1,temp2;
	double c1[] = {0.0545, 0.2442, 0.4026, 0.2442, 0.0545};
	double c2[] = {0.0545, 0.2442, 0.4026, 0.2442, 0.0545};
	temp1 = src.clone();
	temp2 = src.clone();
	register double sum;
	int x1,y1,z1,a,b,c;
	a = src.rows;
	b = src.cols;
	c = src.channels();

	//cout<<temp.size()<<' '<<temp.channels();
	
	for(int k = 0; k<c; ++k){
		for(int i = 0; i < a; ++i){
            for(int j = 0; j < b; ++j){
                sum = 0.0;
                for(int m = -2; m <= 2; ++m){
                    y1 = j + m;
                    if(check(i,y1,k,a,b,c))
                    	sum = sum + c2[m + 2] * src.ptr<float>(i)[c*y1 + k];
                }
                temp1.ptr<float>(i)[c*j + k] = sum;
            }
	    }
    }

    for(int k = 0; k<c; ++k){
		for(int j = 0; j < b; ++j){
            for(int i = 0; i < a; ++i){
                sum = 0.0;
                for(int m = -2; m <= 2; ++m){
                    x1 = i + m;
                    if(check(x1,j,k,a,b,c))
                    	sum = sum + c1[m + 2] * temp1.ptr<float>(x1)[c*j + k];
                }
                temp2.ptr<float>(i)[c*j + k] = sum;
            }
	    }
    }

	
	for(int i = 0; i < a; ++i){
        for(int j = 0; j < b; ++j){
			for(int k = 0; k<c; ++k){
                sum = 0.0;
                for(int m = -2; m <= 2; ++m){
                    z1 = k + m;
                    if(check(i,j,z1,a,b,c))
                    	sum = sum + c1[m + 2] * temp2.ptr<float>(i)[c*j + z1];
                }
                src.ptr<float>(i)[c*j + k] = sum;
            }
	    }
    }    

}
int compare (const void * a, const void * b)
{
  return ( *(float*)a - *(float*)b );
}

void binarize(Mat &src, Mat &bin){
	int t=20;
	int tot,rest,st=0;
	tot = src.rows*src.cols*src.channels();
	rest = tot*t/100;
	float arr[tot];

	for(int k=0;k<src.channels();++k)
		for(int i=0;i<src.rows;++i)
			for(int j=0;j<src.cols;++j)
				arr[st++] = src.ptr<float>(i)[src.channels()*j+k];
	
	//qsort (arr,st+1, sizeof(float), compare);		
	sort(arr,arr+st);
	//cout<<arr[0]<<' '<<arr[st-1]<<endl;
	float val = arr[st-rest];
	bin.create(src.size(),CV_32SC(src.channels()));

	for(int k=0;k<src.channels();++k)
		for(int i=0;i<src.rows;++i)
			for(int j=0;j<src.cols;++j)
				if (src.ptr<float>(i)[src.channels()*j+k] < val)
					bin.ptr<int>(i)[bin.channels()*j+k] = 0;
				else
					bin.ptr<int>(i)[bin.channels()*j+k] = 1;
			
}	

void printH(std::vector<bool> m){
	cout<<m.size()<<endl;
	for(int i=0;i<m.size();++i)
		cout<<m[i];
	cout<<endl;
	//cout<<minVal<<' '<<maxVal<<endl;
}

void linear(Mat &m,std::vector<bool> &v){
	for(int k=0;k<m.channels();++k){
		for(int i=0;i<m.rows;++i){
			for(int j=0;j<m.cols;++j){
				v.push_back(m.ptr<int>(i)[m.channels()*j+k]);
				//cout<<m.ptr<int>(i)[m.channels()*j+k]<<' ';
				//maxVal = max(maxVal,m.ptr<float>(i)[m.channels()*j+k]);
				//minVal = min(minVal,m.ptr<float>(i)[m.channels()*j+k]);
			}
			//cout<<endl;
		}
		//cout<<endl;
	}
}

void getDescriptor(Mat &src, vector<bool> &descriptor, Mat &Hist)
{
	Mat norm_grad,dst_norm,lenE;
	Mat theta;
  	
  	/// edge making -- we will get this as input 
    //src = imread("img.jpg");
	__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 3.1");
	if(src.channels()!=1)
		cvtColor( src, src, CV_RGB2GRAY );
  	__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 3.2");
  	GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
  	edgeMake(src,theta,src,norm_grad,1);
  	convertScaleAbs( norm_grad, norm_grad );
   	GetOrientations(norm_grad,theta);	
 
   	// normalizing 
   	GaussianBlur( norm_grad, dst_norm, Size(3,3), 3, 3, BORDER_DEFAULT );
  	norm_grad.convertTo(norm_grad,CV_32F);
  	dst_norm.convertTo(dst_norm,CV_32F);
  	for(int i=0;i<norm_grad.rows;++i)
  		for(int j=0;j<norm_grad.cols;++j)
  			norm_grad.ptr<float>(i)[j] =  (double)norm_grad.ptr<float>(i)[j] / max((float)4.0 , dst_norm.ptr<float>(i)[j]);
  	/////////////

	//Mat theta,norm_grad;
	Mat Rx,Ry;
	Mat bin;

	Rotate(norm_grad,theta,Rx,Ry);
	createHist(norm_grad,theta,Rx,Ry,Hist);	
	histBlur(Hist);
	binarize(Hist,bin);
	linear(bin,descriptor);
	bin.release();
	
	return;
}
