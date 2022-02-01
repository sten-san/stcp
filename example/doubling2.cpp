// https://atcoder.jp/contests/typical90/tasks/typical90_bf

#include <iostream>
#include <vector>
#include <tuple>
#include <cstddef>
#include <stcp/doubling.hpp>
using namespace std;

int main() {
    constexpr auto mod = 100000;

    int n; cin >> n;
    int64_t k; cin >> k;

    int log2 = 0;
    while ((int64_t(1) << (log2 + 1)) <= k) {
        ++log2;
    }

    array<int, mod> next = {};
    for (int i = 0; i < mod; ++i) {
        next[i] = i;
        for (auto c : to_string(i)) {
            next[i] += c - '0';
        }
        next[i] %= mod;
    }

    auto dub = stcp::make_doubling<int64_t>(log2, next, [&](auto &x, auto &y) {
        array<int, mod> z = {};
        for (int i = 0; i < mod; ++i) {
            z[i] = x[y[i]];
        }
        return z;
    });

    auto f = dub.connect([](auto &g, auto &args) {
        return make_tuple(g[get<0>(args)]);
    });

    auto [ans] = f(k, n);

    cout << ans << endl;
}

