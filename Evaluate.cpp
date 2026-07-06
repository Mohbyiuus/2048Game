//此文件仅用于测试评估函数的效果，请不要用于项目制作 

#include "Evaluate.h"
using namespace std;


int main() {
    int a[5][5] = {0};
    while (true) {
        for (int i=1;i<=4;i++)
            for (int j=1;j<=4;j++) cin>>a[i][j];
        vector<Move> res = Reaction(1, 2, a);
        cout<<evaluate(a)<<endl;
        for(int i=0;i<res.size();i++)
        	cout<<res[i].x<<" "<<res[i].y<<" "<<res[i].z<<" "<<res[i].score<<endl;
    }
    
}
