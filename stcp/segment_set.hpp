#ifndef STCP_SEGMENT_SET_HPP
#define STCP_SEGMENT_SET_HPP

#include <map>
#include "stcp/base.hpp"

namespace stcp {
    template <typename Int = int64_t>
    struct segment_set {
        using int_type = Int;

        segment_set(): seg_() {
        }

    public:
        void insert(int_type l, int_type r, const bool connect_adjacent_segment = false) {
            if (r <= l) {
                return;
            }

            auto iter1 = seg_.upper_bound(l);
            auto iter2 = seg_.lower_bound(r + connect_adjacent_segment);

            if (iter1 != std::begin(seg_)) {
                std::advance(iter1, -1);
                if (l < iter1->second + connect_adjacent_segment) {
                    l = iter1->first;
                }
                std::advance(iter1, +1);
            }

            if (iter2 != std::begin(seg_)) {
                std::advance(iter2, -1);
                if (r <= iter2->second) {
                    r = iter2->second;
                }
                std::advance(iter2, +1);
            }

            seg_.erase(iter1, iter2);

            seg_[l] = r;
        }

        void remove_covered(int_type l, int_type r) {
            if (r <= l) {
                return;
            }

            auto iter = seg_.upper_bound(l);

            if (iter != std::begin(seg_)) {
                std::advance(iter, -1);
            }

            while (iter != std::end(seg_) && iter->first < r) {
                if (l < iter->second) {
                    iter = seg_.erase(iter);
                }
                else {
                    std::advance(iter, +1);
                }
            }
        }

        void remove_covered(int_type x) {
            remove_covered(x, x + 1);
        }

        pair<int_type, int_type> covered(int_type x) const {
            auto iter = seg_.upper_bound(x);

            if (iter == std::begin(seg_)) {
                return { x, x };
            }
            std::advance(iter, -1);

            if (iter->second <= x) {
                return { x, x };
            }

            return *iter;
        }

        int_type covered_size(int_type x) const {
            auto [l, r] = covered(x);
            return r - l;
        }

        bool is_covered(int_type x) const {
            return 0 < covered_size(x);
        }

        bool same(int_type x, int_type y) const {
            auto [l, r] = covered(x);
            return l <= y && y < r;
        }

    private:
        std::map<int_type, int_type> seg_;
    };
}

#endif // STCP_SEGMENT_SET_HPP

