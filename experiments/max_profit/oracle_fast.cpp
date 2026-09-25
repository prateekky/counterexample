#include <bits/stdc++.h>

using namespace std;

vector<vector<int>>dp;

//recursion + memoization

// int profit(vector<int>&arr, int i, int n, int buy){
//     //no more days left to buy and sell
//     if(i==n){
//         return 0;
//     }
//     //if already calculated for that day at buy
//     if(dp[i][buy]!=-1){
//         return dp[i][buy];
//     }
//     int pft=0;
//     //if buy phase then buy the stock or skip to the next day
//     if(buy==1){
//         pft= max(-arr[i]+profit(arr,i+1,n,0),profit(arr,i+1,n,1));
//     }else{
//         //sold state then either sell the stock on current day or keep holding onto it
//         pft= max(arr[i]+0, profit(arr,i+1,n,0));//arr[i]+ 0 because we have to buy-sell only once then end
//     }

//     //return the max profit
//     return dp[i][buy]=pft;
// }

//more optimized

int profit_mo(vector<int>&arr){
    //base
    if(arr.size()<2){
        return 0;
    }
    //minimum price/cost till now
    int mn_p=arr[0];
    //max profit till now
    int mx_pft=0;

    size_t i=0;
    while(i<arr.size()){
        mx_pft=max(mx_pft, arr[i]-mn_p);
        mn_p=min(mn_p, arr[i]);
        i++;
    }
    return mx_pft;
}

int main(){
    int n;
    cin>>n;
    vector<int>arr(n);
    dp.assign(n,vector<int>(2,-1));
    for(auto&x:arr)cin>>x;

    int res=profit_mo(arr);

    cout<<res<<'\n';
    return 0;
}