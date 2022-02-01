// https://atcoder.jp/contests/abc179/tasks/abc179_e

#include <iostream>
#include <vector>
#include <array>
#include <tuple>
#include <cstdint>
#include <stcp/doubling.hpp>
using namespace std;

int main() {
    int64_t n; cin >> n;
    int x, m; cin >> x >> m;

    int log2 = 0;
    while ((int64_t(1) << (log2 + 1)) <= n) {
        ++log2;
    }

    vector<array<int64_t, 2>> next(m);
    for (int64_t i = 0; i < m; ++i) {
        next[i] = { (i * i) % m, (i * i) % m };
    }

    auto dub = stcp::make_doubling<int64_t>(log2, next, [&](auto &x, auto &y) {
        vector<array<int64_t, 2>> z(m);
        for (int i = 0; i < m; ++i) {
            auto [s, p] = x[i];
            auto [t, q] = y[s];
            z[i] = { t, p + q };
        }
        return z;
    });

    auto f = dub.connect([](auto &g, auto &args) {
        auto [s, p] = args;
        auto [t, q] = g[s];
        return make_tuple(t, p + q);
    });

    auto [_, ans] = f(n - 1, x, int64_t(x));

    cout << ans << endl;
}

