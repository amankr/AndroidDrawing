#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <cmath>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
//#include <tr1/unordered_map>

using namespace cv;
using namespace std;

extern map < vector < int > , vector < pair < int , int >  > > db;

void getDescriptor(Mat &src, vector<bool> &descriptor,Mat &Hist);
void loading(jobject &, JNIEnv *);
void loadFile();
void match(vector < bool > & descriptor,int &a,int &b,int &c);
void inserting(vector < bool > & descriptor , int img , int patch);
