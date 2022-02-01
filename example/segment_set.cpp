// https://yukicoder.me/problems/no/674

#include <iostream>
#include <cstdint>
#include <stcp/segment_set.hpp>
using namespace std;

int main() {
    int64_t d; cin >> d;

    stcp::segment_set<int64_t> seg;

    int64_t ans = 0;

    int q; cin >> q;
    while (q--) {
        int64_t a, b; cin >> a >> b; ++b;

        seg.insert(a, b, true);
        ans = max(
            ans,
            seg.covered_size(a)
        );

        cout << ans << endl;
    }
}

