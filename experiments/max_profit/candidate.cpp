#include <bits/stdc++.h>

using namespace std;

class Solution {
public:
    int maxProfit(vector<int>& prices) {
        size_t i = 0;
        size_t j = 1;
        int profit = 0;
        int mx = 0;

        while (j < prices.size()) {
            if (prices[i] < prices[j]) {
                profit = prices[j] - prices[i];
            } else {
                i++;
            }

            mx = max(mx, profit);
            j++;
        }

        return mx ? mx : 0;
    }
};

int main(){
    int n;
    cin>>n;

    vector<int>prices(n);
    for(auto&x: prices)cin>>x;

    cout<<Solution().maxProfit(prices)<<'\n';
}