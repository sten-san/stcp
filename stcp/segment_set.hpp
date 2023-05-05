#ifndef STCP_SEGMENT_SET_HPP
#define STCP_SEGMENT_SET_HPP

#include <utility>
#include <iterator>
#include <optional>
#include <map>
#include <cassert>

namespace stcp {
    // 互いに重ならない区間の集合 { [l1, r1), [l2, r2), ... } を管理する (li < ri)
    template <typename Int>
    struct segment_set {
        using int_type = Int;

        segment_set(): seg_() {
        }

    public:
        // 区間 [l, r) を追加する
        // 既存要素と重なる部分はマージする (境界の接合は行わない)
        void insert(int_type l, int_type r) {
            assert(l <= r);

            if (l == r) {
                return;
            }

            auto iter1 = seg_.upper_bound(l);
            auto iter2 = seg_.lower_bound(r);

            if (iter1 != std::begin(seg_)) {
                std::advance(iter1, -1);
                if (l < iter1->second) {
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

        // 区間 [l, r) を追加する
        // clear(l, r) -> insert(l, r)
        void overwrite(int_type l, int_type r) {
            assert(l <= r);

            if (l == r) {
                return;
            }

            clear(l, r);

            seg_[l] = r;
        }

        // 要素 [x, k), [k, y) を [x, y) へマージする
        void connect(int_type k) {
            auto iter1 = seg_.lower_bound(k);

            if (iter1 == std::begin(seg_) || iter1 == std::end(seg_)) {
                return;
            }

            auto iter2 = std::prev(iter1);

            auto [l1, r1] = *iter2;
            auto [l2, r2] = *iter1;

            if (r1 == k && l2 == k) {
                seg_.erase(iter1);
                seg_.erase(iter2);
                seg_[l1] = r2;
            }
        }

        // 要素 [x, y) を [x, k), [k, y) に分割
        void split(int_type k) {
            auto _ = wrapped(k);

            if (!_.has_value()) {
                return;
            }

            auto [l, r] = *_;

            seg_.erase(l);
            seg_[l] = k;
            seg_[k] = r;
        }

        // 集合から [l, r) に重なる部分を *切り取り* 削除する
        // 部分的に重なるような区間要素に対しては適切な分割が行われる
        void clear(int_type l, int_type r) {
            assert(l <= r);

            split(l);
            split(r);

            remove_covered(l, r);
        }

        // [l, r) に重なる区間を集合からすべて取り除く
        void remove_covered(int_type l, int_type r) {
            assert(l <= r);

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

        // 存在するのなら l <= k < r な区間 [l, r) を返す
        std::optional<std::pair<int_type, int_type>> wrapped(int_type k) const {
            auto iter = seg_.upper_bound(k);

            if (iter == std::begin(seg_)) {
                return std::nullopt;
            }
            std::advance(iter, -1);

            if (iter->second <= k) {
                return std::nullopt;
            }

            return *iter;
        }

        // k を含む区間のサイズを取得する
        std::size_t wrapped_size(int_type k) const {
            auto _ = wrapped(k);

            if (!_.has_value()) {
                return 0;
            }

            auto [l, r] = *_;

            return r - l;
        }

        // k を含む区間が存在するか
        bool is_wrapped(int_type k) const {
            return wrapped(k).has_value();
        }

        // x, y が同じ区間に属するか
        bool same(int_type x, int_type y) const {
            auto _ = wrapped(x);

            if (!_.has_value()) {
                return false;
            }

            auto [l, r] = *_;

            return l <= y && y < r;
        }

    private:
        std::map<int_type, int_type> seg_;
    };
}

#endif // STCP_SEGMENT_SET_HPP

