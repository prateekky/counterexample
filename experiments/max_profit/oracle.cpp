#include <bits/stdc++.h>

using namespace std;

class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int mx = 0;

        for (int i = 0; i < (int)prices.size(); ++i) {
            for (int j = i + 1; j < (int)prices.size(); ++j) {
                mx = max(mx, prices[j] - prices[i]);
            }
        }

        return mx;
    }
};

int main(){
    int n;
    cin>>n;

    vector<int>prices(n);
    for(auto&x: prices)cin>>x;

    cout<<Solution().maxProfit(prices)<<'\n';
}