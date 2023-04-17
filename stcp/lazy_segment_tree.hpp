#ifndef STCP_LAZY_SEGMENT_TREE_HPP
#define STCP_LAZY_SEGMENT_TREE_HPP

#include <type_traits>
#include <utility>
#include <vector>
#include <cassert>
#include <cstddef>

namespace stcp {
    template <typename S, S (*Op)(S, S), S (*E)(), typename F, S (*Mapping)(F, S), F (*Composition)(F, F), F (*Id)()>
    struct lazy_segment_tree {
        using value_type = S;

        // O(1)
        lazy_segment_tree():
            lazy_segment_tree(0) {
        }

        // O(n)
        explicit lazy_segment_tree(std::size_t n):
            n_(n) {
            log_ = 0;
            while ((std::size_t(1) << log_) < n_) {
                ++log_;
            }
            size_ = (1 << log_);

            data_ = std::vector<S>(size_ + size_, E());
            lazy_ = std::vector<F>(size_, Id());
        }

        // O(size(v))
        explicit lazy_segment_tree(const std::vector<S> &v):
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
                update_data(i);
            }

            lazy_ = std::vector<F>(size_, Id());
        }

    public:
        // O(log size(lazy_segment_tree))
        // 0 <= i < size(lazy_segment_tree)
        void set(std::size_t i, S x) {
            assert(0 <= i && i < n_);

            i += size_;
            for (auto j = log_; 1 <= j; --j) {
                apply_lazy(i >> j);
            }

            data_[i] = x; i >>= 1;
            while (1 <= i) {
                update_data(i); i >>= 1;
            }
        }

        // O(log size(lazy_segment_tree))
        // 0 <= i < size(lazy_segment_tree)
        const S &get(std::size_t i) const {
            assert(0 <= i && i < n_);

            i += size_;
            for (auto j = log_; 1 <= j; --j) {
                apply_lazy(i >> j);
            }

            return data_[i];
        }

        // O(log size(lazy_segment_tree))
        // 0 <= l <= r <= size(lazy_segment_tree)
        S prod(std::size_t l, std::size_t r) const {
            assert(0 <= l && l <= r && r <= n_);

            if (l == r) {
                return E();
            }
            l += size_; r += size_;

            for (auto i = log_; 1 <= i; --i) {
                if (((l >> i) << i) != l) {
                    apply_lazy(l >> i);
                }
                if (((r >> i) << i) != r) {
                    apply_lazy((r - 1) >> i);
                }
            }

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

        // O(log size(lazy_segment_tree))
        // 0 <= l <= r <= size(lazy_segment_tree)
        void apply(std::size_t l, std::size_t r, F f) {
            assert(0 <= l && l <= r && r <= n_);

            if (l == r) {
                return;
            }
            l += size_; r += size_;

            for (auto i = log_; 1 <= i; --i) {
                if (((l >> i) << i) != l) {
                    apply_lazy(l >> i);
                }
                if (((r >> i) << i) != r) {
                    apply_lazy((r - 1) >> i);
                }
            }

            auto pl = l, pr = r;
            while (l < r) {
                if (l & 1) {
                    push_lazy(l++, f);
                }
                if (r & 1) {
                    push_lazy(--r, f);
                }
                l >>= 1; r >>= 1;
            }

            for (std::size_t i = 1; i <= log_; ++i) {
                if (((pl >> i) << i) != pl) {
                    update_data(pl >> i);
                }
                if (((pr >> i) << i) != pr) {
                    update_data((pr - 1) >> i);
                }
            }
        }

        // O(log size(lazy_segment_tree))
        // 0 <= l <= size(lazy_segment_tree)
        template <typename G>
        std::size_t max_right(std::size_t l, G &&f) const {
            static_assert(std::is_invocable_r_v<bool, G, S>);

            assert(0 <= l && l <= n_);
            assert(std::forward<G>(f)(E()));

            if (l == n_) {
                return n_;
            }
            l += size_;

            for (auto i = log_; 1 <= i; --i) {
                apply_lazy(l >> i);
            }

            S acc = E();
            while (((l & (l << 1)) | 1) != l) {
                if (S con = Op(acc, data_[l]); std::forward<G>(f)(con)) {
                    if (l & 1) {
                        acc = con; ++l;
                    }
                    l >>= 1;
                    continue;
                }
                break;
            }

            if (std::forward<G>(f)(Op(acc, data_[l]))) {
                return n_;
            }

            while (l < size_) {
                apply_lazy(l); l <<= 1;
                if (S con = Op(acc, data_[l]); std::forward<G>(f)(con)) {
                    acc = con; ++l;
                }
            }

            return l - size_;
        }

        // O(log size(lazy_segment_tree))
        // 0 <= r <= size(lazy_segment_tree)
        template <typename G>
        std::size_t min_left(std::size_t r, G &&f) const {
            static_assert(std::is_invocable_r_v<bool, G, S>);

            assert(0 <= r && r <= n_);
            assert(std::forward<G>(f)(E()));

            if (r == 0) {
                return 0;
            }
            r += size_; --r;

            for (auto i = log_; 1 <= i; --i) {
                apply_lazy(r >> i);
            }

            S acc = E();
            while ((r & -r) != r) {
                if (S con = Op(data_[r], acc); std::forward<G>(f)(con)) {
                    if ((r & 1) == 0) {
                        acc = con; --r;
                    }
                    r >>= 1;
                    continue;
                }
                break;
            }

            if (std::forward<G>(f)(Op(data_[r], acc))) {
                return 0;
            }

            while (r < size_) {
                apply_lazy(r); r <<= 1; ++r;
                if (S con = Op(data_[r], acc); std::forward<G>(f)(con)) {
                    acc = con; --r;
                }
            }

            return r + 1 - size_;
        }

    private:
        void apply_lazy(std::size_t i) const {
            push_lazy(i + i, lazy_[i]);
            push_lazy(i + i + 1, lazy_[i]);
            lazy_[i] = Id();
        }
        void push_lazy(std::size_t i, F f) const {
            data_[i] = Mapping(f, data_[i]);
            if (i < size_) {
                lazy_[i] = Composition(f, lazy_[i]);
            }
        }

        void update_data(std::size_t i) const {
            data_[i] = Op(data_[i + i], data_[i + i + 1]);
        }

    private:
        mutable std::vector<S> data_;
        mutable std::vector<F> lazy_;
        std::size_t n_, size_, log_;
    };
}

#endif // STCP_LAZY_SEGMENT_TREE_HPP

