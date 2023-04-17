#ifndef STCP_SEGMENT_TREE_HPP
#define STCP_SEGMENT_TREE_HPP

#include <type_traits>
#include <utility>
#include <vector>
#include <cassert>
#include <cstddef>

namespace stcp {
    template <typename S, S (*Op)(S, S), S (*E)()>
    struct segment_tree {
        using value_type = S;

        // O(1)
        segment_tree():
            segment_tree(0) {
        }

        // O(n)
        explicit segment_tree(std::size_t n):
            n_(n) {
            log_ = 0;
            while ((std::size_t(1) << log_) < n_) {
                ++log_;
            }
            size_ = (1 << log_);

            data_ = std::vector<S>(size_ + size_, E());
        }

        // O(size(v))
        explicit segment_tree(const std::vector<S> &v):
            n_(v.size()) {
            log_ = 0;
            while ((std::size_t(1) << log_) < n_) {
                ++log_;
            }
            size_ = (1 << log_);

            data_ = std::vector<S>(size_ + size_, E());
            for (std::size_t i = 0; i < n_; ++i) {
                data_[i + size_] = v[i];
            }

            for (std::size_t i = size_ - 1; 1 <= i; --i) {
                data_[i] = Op(data_[i + i], data_[i + i + 1]);
            }
        }

    public:
        // O(log size(segment_tree))
        // 0 <= i < size(segment_tree)
        void set(std::size_t i, S x) {
            assert(0 <= i && i < n_);

            i += size_;

            data_[i] = x; i >>= 1;
            while (1 <= i) {
                data_[i] = Op(data_[i + i], data_[i + i + 1]);
                i >>= 1;
            }
        }
        // O(1)
        // 0 <= i < size(segment_tree)
        const S &get(std::size_t i) const {
            assert(0 <= i && i < n_);

            return data_[i + size_];
        }

        // O(log size(segment_tree))
        // 0 <= l <= r <= size(segment_tree)
        S prod(std::size_t l, std::size_t r) const {
            assert(0 <= l && l <= r && r <= n_);

            l += size_; r += size_;

            S accl = E(), accr = E();
            while (l < r) {
                if (l & 1) {
                    accl = Op(accl, data_[l++]);
                }
                if (r & 1) {
                    accr = Op(data_[--r], accr);
                }
                l >>= 1; r >>= 1;
            }

            return Op(accl, accr);
        }

        // O(1)
        const S &all_prod() const {
            return data_[1];
        }

        // O(log size(segment_tree))
        // 0 <= l && l <= r && r <= n_
        template <typename F>
        void acc(std::size_t l, std::size_t r, F &&f) const {
            static_assert(std::is_invocable_v<F, S>);

            assert(0 <= l && l <= r && r <= n_);

            l += size_; r += size_;

            std::size_t log_ = 0, pr = r;
            while (l < pr) {
                if (l & 1) {
                    std::forward<F>(f)(data_[l++]);
                }
                l >>= 1; pr >>= 1; ++log_;
            }

            while (pr < size_) {
                pr <<= 1;
                if (pr != (r >> (log_ - 1))) {
                    std::forward<F>(f)(data_[pr]);
                    ++pr;
                }
                --log_;
            }
        }

        // O(log size(segment_tree))
        // 0 <= l <= size(segment_tree)
        template <typename F>
        std::size_t max_right(std::size_t l, F &&f) const {
            static_assert(std::is_invocable_r_v<bool, F, S>);

            assert(0 <= l && l <= n_);
            assert(std::forward<F>(f)(E()));

            if (l == n_) {
                return n_;
            }
            l += size_;

            S acc = E();
            while (((l & (l << 1)) | 1) != l) {
                if (S con = Op(acc, data_[l]); std::forward<F>(f)(con)) {
                    if (l & 1) {
                        acc = con; ++l;
                    }
                    l >>= 1;
                    continue;
                }
                break;
            }

            if (std::forward<F>(f)(Op(acc, data_[l]))) {
                return n_;
            }

            while (l < size_) {
                l <<= 1;
                if (S con = Op(acc, data_[l]); std::forward<F>(f)(con)) {
                    acc = con; ++l;
                }
            }

            return l - size_;
        }

        // O(log size(segment_tree))
        // 0 <= r <= size(segment_tree)
        template <typename F>
        std::size_t min_left(std::size_t r, F &&f) const {
            static_assert(std::is_invocable_r_v<bool, F, S>);

            assert(0 <= r && r <= n_);
            assert(std::forward<F>(f)(E()));

            if (r == 0) {
                return 0;
            }
            r += size_; --r;

            S acc = E();
            while ((r & -r) != r) {
                if (S con = Op(data_[r], acc); std::forward<F>(f)(con)) {
                    if ((r & 1) == 0) {
                        acc = con; --r;
                    }
                    r >>= 1;
                    continue;
                }
                break;
            }

            if (std::forward<F>(f)(Op(data_[r], acc))) {
                return 0;
            }

            while (r < size_) {
                r <<= 1; ++r;
                if (S con = Op(data_[r], acc); std::forward<F>(f)(con)) {
                    acc = con; --r;
                }
            }

            return r + 1 - size_;
        }

    private:
        std::vector<S> data_;
        std::size_t n_, size_, log_;
    };
}

#endif // STCP_SEGMENT_TREE_HPP

