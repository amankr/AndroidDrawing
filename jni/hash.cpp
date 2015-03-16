/* 
	Use this to create n*k permanent permutations 
*/
#include "ugp.h"

vector < int > generatePermutation(int n)
{
	//srand(unsigned(time(0)));
	vector < int > re;
	for(int i = 0 ; i < n ; ++i)	re.push_back(i);
	random_shuffle(re.begin(),re.end());
	return re;
}

inline int minIndex(vector < int > &permutation , vector < int > & descriptor)
{
	int re;
	for(re = 0 ; re < descriptor.size() ; ++re)	if(descriptor[permutation[re]])	return re;
	return re;
}

int main(){
	int n;
	n = 576;
	std::vector<int> v;
	srand(unsigned(time(0)));
	for(int i=0;i<60;++i){
		v = generatePermutation(n);
		for(int j=0;j<v.size();++j)
			std::cout<<v[j]<<' ';
		putchar('\n');
	}
}
