#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <vector>

using namespace cv;
using namespace std;



inline bool check(int i , int j , int n , int m)
{
        return (i >= 0 && i < n && j >= 0 && j < m);
}

float average(int x , int y , int n , int m , Mat & Img)
{
        int xk,yk;
        register float sum = 0.0;
        register int cnt = 0;
        for(int i = -2 ; i <= 2 ; ++i)
        {
                for(int j = -2 ; j <= 2 ; ++j)
                {
                        xk = x + i;
                        yk = y + j;
                        if(check(xk,yk,n,m))
                        {
                                cnt++;
                                sum += Img.at<float>(xk,yk);
                        }
                }
        }
        return sum /(cnt);
}

float devn(int x , int y , int n , int m , float avg , Mat & Img)
{
        int xk,yk;
        register int cnt = 0;
        register float sum = 0.0;
        for(int i = -2 ; i <= 2 ; ++i)
        {
                for(int j = -2 ; j <= 2 ; ++j)
                {
                        xk = x + i;
                        yk = y + j;
                        if(check(xk,yk,n,m))
                        {
                                cnt++;
                                sum += ((Img.at<float>(xk,yk) - avg) * (Img.at<float>(xk,yk) - avg));
                        }
                }
        }
        sum = sum/(cnt);
        return sqrt(sum);
}

void converter(Mat & src , Mat & dst)
{       
        int n = src.rows;
        int m = src.cols;
        float av,val,e=0.01;
        float dn;
        dst.create(src.size(),CV_32F);
        float *dt;
        int store = src.type();
        src.convertTo(src,CV_32F);
        float *sc;
        for(int i = 0 ; i < n ; ++i)
        {
        		dt = dst.ptr<float>(i);
        		sc = src.ptr<float>(i);
                for(int j = 0 ; j < m ; ++j)
                {
                       av = average(i,j,n,m,src);
                       dn = devn(i,j,n,m,av,src);
                      
                       dt[j] = (sc[j] -av) ;
                       dt[j] = (dt[j] > 0) ? dt[j]:0;
                       dt[j] /= (dn+e);
                       
                	
                }
        }
        src.convertTo(src,store);
}


//edgeDetector
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
inline float dist(float x1,float y1,float x2, float y2){
	return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
}

void getFour(vector<vector<float> > &v,int i,int j,float theta){ // theta in 4 quad
	float x,y,rt;
	rt = sqrt(2);
	x = i + rt*cos(theta);
	y = j + rt*sin(theta);
	
	int x1,y1,x2,y2;
	x1 = x;
	y1 = y;
	x2 = x1+1;
	y2 = y1+1;
	if(i==100 && j==100)
		cout<<i<<' '<<j<<' '<<x1<<' '<<y1<<' '<<theta*180/M_PI<<endl;
	v[0][0] = x1;
	v[0][1] = y1;
	v[1][0] = x1;
	v[1][1] = y2;
	v[2][0] = x2;
	v[2][1] = y1;
	v[3][0] = x2;
	v[3][1] = y2;
	
	v[0][2] = (x2-x)*(y2-y);
	v[1][2] = (x2-x)*(y-y1);
	v[2][2] = (x-x1)*(y2-y);
	v[3][2] = (x-x1)*(y-y1);
	/*
	// bilinear weights 
	v[0][2] = dist(v[0][0],v[0][1],x,y);
	v[1][2] = dist(v[1][0],v[1][1],x,y);
	v[2][2] = dist(v[2][0],v[2][1],x,y);
	v[3][2] = dist(v[3][0],v[3][1],x,y);
	float sum = v[0][2]+v[1][2]+v[2][2]+v[3][2];
	
	
	v[0][2] = v[0][2]/sum;
	v[1][2] = v[1][2]/sum;
	v[2][2] = v[2][2]/sum;
	v[3][2] = v[3][2]/sum;
	*/
	
	
}

void msgInit(Mat fourX1[],Mat fourY1[],Mat fourX2[],Mat fourY2[],Mat w0[], Mat w1[], Mat a0[], Mat a1[], Mat &dir){
	int n = dir.rows;
    int m = dir.cols;
    
	for(int i=0;i<4;++i){
		fourX1[i].create(dir.size(),CV_32F);
		fourY1[i].create(dir.size(),CV_32F);
		fourX2[i].create(dir.size(),CV_32F);
		fourY2[i].create(dir.size(),CV_32F);
		w0[i].create(dir.size(),CV_32F);
		w1[i].create(dir.size(),CV_32F);
		a0[i].create(dir.size(),CV_32F);
		a1[i].create(dir.size(),CV_32F);
	}
	
	
	vector<float> b(3,0);
	vector<vector<float> > v(4,b);
	//for(int i=0;i<4;++i) v.push_back(b);

	float var=2*M_PI/5;
	int X,Y;

	float *fx1[4],*fx2[4],*fy1[4],*fy2[4],*W0[4],*W1[4],*A0[4],*A1[4],*mg;

	for(int i=0;i<dir.rows;++i){

		for(int k=0;k<4;++k){
			fx1[k] = fourX1[k].ptr<float>(i);
			fx2[k] = fourX2[k].ptr<float>(i);
			fy1[k] = fourY1[k].ptr<float>(i);
			fy2[k] = fourY2[k].ptr<float>(i);
			W0[k] = w0[k].ptr<float>(i);
			W1[k] = w1[k].ptr<float>(i);
			A0[k] = a0[k].ptr<float>(i);
			A1[k] = a1[k].ptr<float>(i); 
		}
		mg = dir.ptr<float>(i);

		for(int j=0;j<dir.cols;++j){
			getFour(v,i,j,mg[j]);//dir.at<float>(i,j));
			for(int k=0;k<4;++k){
				fx1[k][j] = v[k][0];
				fy1[k][j] = v[k][1];
				W0[k][j] = v[k][2];
				X=v[k][0];
				Y=v[k][1];
				if(check(X,Y,n,m)){
					W1[k][j] = exp(-((mg[j]-dir.ptr<float>(X)[Y])*(mg[j]-dir.ptr<float>(X)[Y]))/var);
				}	
			}

			getFour(v,i,j,mg[j]+M_PI);//dir.at<float>(i,j)+M_PI);
			for(int k=0;k<4;++k){
				fx2[k][j] = v[k][0];
				fy2[k][j] = v[k][1];
				A0[k][j] = v[k][2];
				X=v[k][0];
				Y=v[k][1];
				if(check(X,Y,n,m)){
					A1[k][j] = exp(-((mg[j]-dir.ptr<float>(X)[Y])*(mg[j]-dir.ptr<float>(X)[Y]))/var);
				}	
			}

		}
	}
}
void MessagePass(Mat & mag,Mat & dir, Mat & len){
	//cout<<"here";
	int store = mag.type();
	mag.convertTo(mag,CV_32F);

	Mat M0,M1,W0,W1;
	int n = mag.rows;
    int m = mag.cols;
    M0.create(mag.size(),CV_32F);
    M1.create(mag.size(),CV_32F);
	M0.zeros(mag.size(),CV_32F);
	M1.zeros(mag.size(),CV_32F);
	W0.create(mag.size(),CV_32F);
    W1.create(mag.size(),CV_32F);
	
	vector<vector<float> > v;
//	float var=2*M_PI/5;
	int X,Y;
	
	float *x1[4],*y1[4],*x2[4],*y2[4],*m0, *m1, *w0, *w1;
	float *m0W0[4], *m0W1[4],*m1W0[4], *m1W1[4]; 

	Mat fourX1[4],fourY1[4],fourX2[4],fourY2[4],m0w0[4],m0w1[4],m1w0[4],m1w1[4];
	
	msgInit(fourX1,fourY1,fourX2,fourY2,m0w0,m0w1,m1w0,m1w1,dir);  // time taking 
	

	for(int iter=1;iter<=10;++iter){ // must b even
	//	cout<<iter<<endl;
		for(int i=0;i<n;++i){
			for(int k=0;k<4;++k){
				x1[k] = fourX1[k].ptr<float>(i);
				y1[k] = fourY1[k].ptr<float>(i);
				x2[k] = fourX2[k].ptr<float>(i);
				y2[k] = fourY2[k].ptr<float>(i);
			//	m0W0[k] = m0w0[k].ptr<float>(i);
			//	m0W1[k] = m0w1[k].ptr<float>(i);
			//	m1W0[k] = m1w0[k].ptr<float>(i);
			//	m1W1[k] = m1w1[k].ptr<float>(i);
			}

			for(int j=0;j<m;++j){
				
				if(iter &1){
					W0.ptr<float>(i)[j] = 0;
					W1.ptr<float>(i)[j] = 0;
				}
				else{
					M0.ptr<float>(i)[j] = 0;
					M1.ptr<float>(i)[j] = 0;
				}
				
				for(int k=0;k<4;++k){
					X = x1[k][j];
					Y = y1[k][j];
					if(check(X,Y,n,m)){
						if(iter&1)
							W0.ptr<float>(i)[j] = m0w0[k].ptr<float>(X)[Y] * m0w1[k].ptr<float>(X)[Y] * (mag.ptr<float>(X)[Y] + M0.ptr<float>(X)[Y]);
						else
							M0.ptr<float>(i)[j] = m0w0[k].ptr<float>(X)[Y] * m0w1[k].ptr<float>(X)[Y] * (mag.ptr<float>(X)[Y] + W0.ptr<float>(X)[Y]);
						
					}
				}
				for(int k=0;k<4;++k){
					X = x2[k][j];
					Y = y2[k][j];
					if(check(X,Y,n,m)){
						if(iter&1)
							W1.ptr<float>(i)[j] = m1w0[k].ptr<float>(X)[Y] * m1w1[k].ptr<float>(X)[Y] * (mag.ptr<float>(X)[Y] + M1.ptr<float>(X)[Y]);
						else
							M1.ptr<float>(i)[j] = m1w0[k].ptr<float>(X)[Y] * m1w1[k].ptr<float>(X)[Y] * (mag.ptr<float>(X)[Y] + W1.ptr<float>(X)[Y]);
	
					}
				}
			}
		}
	}
	float *ln;
	float *mg;
	len.create(mag.size(),CV_32F);
	for(int i=0;i<n;++i){
		ln = len.ptr<float>(i);
		mg = mag.ptr<float>(i); 
		m0 = M0.ptr<float>(i);
		m1 = M1.ptr<float>(i);
		for(int j=0;j<m;++j){
			ln[j] = mg[j] + m0[j] + m1[j] ;
		}
	}
	mag.convertTo(mag,store);
	/*
	for(int i=100;i<106;++i){
		for(int j=100;j<106;++j){
			cout<<m0w0[0].ptr<float>(i)[j]<<':'<< m0w1[0].ptr<float>(i)[j]<<' ';

		}
		cout<<endl;
	}	
	*/
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

int main(){
	Mat src,dst,dst_norm,lenE;
	Mat angle;
	//params
	int kernel_size = 3;
  	int scale = 1;
  	int delta = 0;
  	int ddepth = CV_32F;
  
    src = imread("img.jpg");
  	cvtColor( src, src, CV_RGB2GRAY );
  	GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
  //	Laplacian( src, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
  	edgeMake(src,angle,src,dst,1);
  	convertScaleAbs( dst, dst );
  	
  	converter(dst,dst_norm);
   	GetOrientations(dst,angle);	
    

	  	
  	convertScaleAbs( dst_norm, dst_norm );
 	imwrite("imd3.jpg",dst_norm);

  	
  	MessagePass(dst_norm,angle,lenE);
 	
 	Mat ndst;
 	//dst_norm.convertTo(dst_norm,CV_32F);
 	edgeMake(lenE,angle,src,ndst,0);
 	convertScaleAbs( ndst, ndst); 	
  	
  /// Create window
  	//namedWindow("wtf");//, CV_WINDOW_AUTOSIZE );


  
	
  /// Show what you got
	imwrite("img1.jpg", ndst);
	
	imwrite("imd2.jpg", dst);
	

	//waitKey(0);
	
	
	return 0;
  

}