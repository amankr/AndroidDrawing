#include "ugp.h"
#include <time.h>

int perm[61][577];

map < vector < int > , vector < pair < int , int >  > > db;


void loading(jobject &Amgr,JNIEnv *env)
{
	/*
	AAssetManager *mgr = AAssetManager_fromJava(env, Amgr);;

	AAssetDir* assetDir = AAssetManager_openDir(mgr, "");

	const char* f1 = (const char*)"permutations.txt";
	const char* f2 = (const char*)"db.txt";
	/*while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
		__android_log_print(ANDROID_LOG_DEBUG, "NDK_asset_file ", "%s",filename);
	}

	AAsset* asset1 = AAssetManager_open(mgr, (const char *)f1, AASSET_MODE_UNKNOWN);
	AAsset* asset2 = AAssetManager_open(mgr, (const char *)f2, AASSET_MODE_UNKNOWN);
	*/

	FILE *fp;
	__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 1.1");
	fp = fopen("/mnt/sdcard/Download/ugp/permutations.txt","r");
	//__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 1.2");
	if(!fp)
		__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "file not found");
	else
		__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "file found");
	//while(true)
	//{
		for(int i = 0 ; i < 60 ; ++i)
			for(int j = 0 ; j < 576 ; ++j)	
				fscanf(fp,"%d",&perm[i][j]);
	//}
	fclose(fp);
	loadFile();

	//AAsset_close(asset1);
	//AAsset_close(asset2);
}


void loadFile()
{
		if(db.size() != 0){
			__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "map exits");
			return;
		}
		__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 1.2");
        FILE *fp;
        fp = fopen("mnt/sdcard/Download/ugp/db.txt","r");
        if(!fp)
			__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "file not found");
		else
			__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "file found");
        vector < int > ktuple;
        int idx;
        int k;
        pair < int , int > p;
        while(true)
        {
                ktuple.clear();
                if(fscanf(fp,"%d",&k) == EOF) break;
                for(int i = 0 ; i < k ; ++i)    
                {
                        fscanf(fp,"%d",&idx);
                        ktuple.push_back(idx);
                }
                fscanf(fp,"%d",&p.first);
                fscanf(fp,"%d",&p.second);
                db[ktuple].push_back(p);
        }
        fclose(fp);
        return ;
}

void store(vector < int > &ktuple , int img , int patch)
{
        FILE * fp;
        fp = fopen("db.txt","a");
        fprintf(fp,"%d ",(int)ktuple.size());
        for(int i = 0 ; i < ktuple.size() ; ++i)        fprintf(fp,"%d ",ktuple[i]);
        fprintf(fp,"%d ",img);
        fprintf(fp,"%d\n",patch); 
        fclose(fp);
        return;
}

void inserting(vector < bool > & descriptor , int img , int patch) 
{
	int n=20,k=3;
	vector < int > temp;
	for(int i = 0 ; i < n ; ++i)
	{
		temp.clear();
		for(int j = 0 ; j < k ; ++j)
		{
			int l;
			for(l = 0 ; l < descriptor.size() ; ++l)	if(descriptor[perm[i*k + j][l]])	break;
			temp.push_back(l);
		}
		store(temp,img,patch);
	}

}

void match(vector < bool > & descriptor,int &a,int &b,int &c){
	__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 3.1");
	int k=3;
	vector < int > temp;
	//srand(time(NULL));
	int p=0;//rand()%20;
	__android_log_print(ANDROID_LOG_DEBUG, "LOG_Ndk", "checkpoint 3.1 random %d",p);
	for(int j = 3*p ; j < 3*p+k ; ++j)
	{
		int l;
		for(l = 0 ; l < descriptor.size() ; ++l)	if(descriptor[perm[j][l]])	break;
		temp.push_back(l);
	}
	for(int i=0;i<db[temp].size();++i){
		if(db[temp][i].first == 0)	a++;
		else if(db[temp][i].first == 1)	b++;
		else	c++;
	}
}
