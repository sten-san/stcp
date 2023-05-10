// https://yukicoder.me/problems/no/2292

#include <iostream>
#include <algorithm>
#include <cstdint>
#include <stcp/segment_set.hpp>
using namespace std;

int main() {
    int n, q; cin >> n >> q;

    stcp::segment_set<int64_t> set;

    while (q--) {
        int t; cin >> t;

        if (t == 1) {
            int l, r; cin >> l >> r; ++r;

            set.insert(l, r);
        }

        if (t == 2) {
            int l, r; cin >> l >> r;

            set.clear(l + 1, r);
        }

        if (t == 3) {
            int u, v; cin >> u >> v;

            if (u != v) {
                cout << set.same(u, v) << '\n';
            }
            else {
                cout << 1 << '\n';
            }
        }

        if (t == 4) {
            int u; cin >> u;

            cout << max<int64_t>(1, set.wrapped_size(u)) << '\n';
        }
    }

    cout << flush;
}

