// https://yukicoder.me/problems/no/674

#include <iostream>
#include <algorithm>
#include <cstdint>
#include <stcp/segment_set.hpp>
using namespace std;

int main() {
    int64_t d; cin >> d;

    stcp::segment_set<int64_t> set;

    int64_t ans = 0;

    int q; cin >> q;
    while (q--) {
        int64_t a, b; cin >> a >> b; ++b;

        set.insert(a, b);
        set.connect(a);
        set.connect(b);

        ans = max<int64_t>(
            ans,
            set.wrapped_size(a)
        );

        cout << ans << '\n';
    }

    cout << flush;
}

