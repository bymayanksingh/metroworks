#include<iostream>
#include<vector>
#include<queue>
using namespace std;
vector <int> v[1005];
bool visited[1005];
int from[1005];
int n,e,a,b,x,y,c=0,src,dest;
void addEdges(int u,int w){
v[u].push_back(w);
v[w].push_back(u);
}
void bfs(int src,int dest){
    queue <int> q;
    q.push(src);
    visited[src]=true;
    while(!q.empty()){
        int u=q.front();
        q.pop();
        for(auto x:v[u]){
            if(!visited[x]){
                from[x]=u;
                visited[x]=true;
                if(x==dest) return ;
                q.push(x);
            }
        }
}
}
int main(){
cout<<"Enter the number of node station:-";
cin>>n;
cout<<"Enter the station connected:-";
cin>>e;
cout<<"Enter the stations connected:-";
for(int i=1;i<=e;i++){
cin>>a>>b;
addEdges(a,b);
}
cout<<"Enter your source :-";
cin>>src;
cout<<"Enter your destination :-";
cin>>dest;
bfs(src,dest);
vector <int> ans;
ans.push_back(dest);
int s=from[dest],c=1;
while(s!=src){
ans.push_back(s);
s=from[s];
c++;
}
ans.push_back(src);c++;cout<<c<<"\n";
for(int i=ans.size()-1;i>=0;i--)cout<<ans[i]<<" ";
return 0;
}
