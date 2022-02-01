// https://atcoder.jp/contests/abc167/tasks/abc167_d

#include <iostream>
#include <vector>
#include <tuple>
#include <cstdint>
#include <stcp/doubling.hpp>
using namespace std;

int main() {
    int n; cin >> n;
    int64_t k; cin >> k;

    vector<int> a(n);
    for (auto &e : a) {
        cin >> e; --e;
    }

    int log2 = 0;
    while ((int64_t(1) << (log2 + 1)) <= k) {
        ++log2;
    }

    auto dub = stcp::make_doubling<int64_t>(log2, a, [&](auto &x, auto &y) {
        vector<int> z(n);
        for (int i = 0; i < n; ++i) {
            z[i] = x[y[i]];
        }
        return z;
    });

    auto f = dub([](auto &g, auto &args) {
        return make_tuple(g[get<0>(args)]);
    });

    auto [ans] = f(k, 0);

    cout << ans + 1 << endl;
}

