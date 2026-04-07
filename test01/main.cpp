#include <iostream>
#include <array>

using namespace std;
std::array<int, 4> coins = { 64, 16, 4, 1 };
int coinChange(int rest, int idx) {
    if (rest > coins[idx])
    {
        rest -= coins[idx];
        return 1 + coinChange(rest, idx);
    }
    else if (rest > 0)
    {
        return coinChange(rest, ++idx);
    }
    return 0;
}
int main() {
    int n;
    cin >> n;
    int ans = coinChange(1024 - n, 0);
    cout << ans << endl;
    return 0;
}