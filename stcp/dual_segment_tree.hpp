#ifndef STCP_DUAL_SEGMENT_TREE_HPP
#define STCP_DUAL_SEGMENT_TREE_HPP

#include <utility>
#include <vector>
#include <cassert>
#include <cstddef>

namespace stcp {
    template <typename S, typename F, S (*Mapping)(F, S), F (*Composition)(F, F), F (*Id)()>
    struct dual_segment_tree {
        using value_type = S;

        // O(1)
        dual_segment_tree():
            dual_segment_tree(0) {
        }

        // O(n)
        explicit dual_segment_tree(std::size_t n):
            n_(n) {
            log_ = 0;
            while ((std::size_t(1) << log_) < n_) {
                ++log_;
            }
            size_ = (1 << log_);

            data_ = std::vector<S>(n_);
            lazy_ = std::vector<F>(size_, Id());
        }

        // O(size(v))
        explicit dual_segment_tree(std::vector<S> v):
            n_(v.size()) {
            log_ = 0;
            while ((std::size_t(1) << log_) < n_) {
                ++log_;
            }
            size_ = (1 << log_);

            data_ = std::move(v);
            lazy_ = std::vector<F>(size_, Id());
        }

    public:
        // O(log size(dual_segment_tree))
        // 0 <= i < size(dual_segment_tree)
        void set(std::size_t i, S x) {
            assert(0 <= i && i < n_);

            i += size_;
            for (auto j = log_; 1 <= j; --j) {
                apply_lazy(i >> j);
            }

            data_[i - size_] = x;
        }

        // O(log size(dual_segment_tree))
        // 0 <= i < size(dual_segment_tree)
        const S &get(std::size_t i) const {
            assert(0 <= i && i < n_);

            i += size_;
            for (auto j = log_; 1 <= j; --j) {
                apply_lazy(i >> j);
            }

            return data_[i - size_];
        }

        // O(log size(dual_segment_tree))
        // 0 <= l <= r <= size(dual_segment_tree)
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

            while (l < r) {
                if (l & 1) {
                    push_lazy(l++, f);
                }
                if (r & 1) {
                    push_lazy(--r, f);
                }
                l >>= 1; r >>= 1;
            }
        }

    private:
        void apply_lazy(std::size_t i) const {
            push_lazy(i + i, lazy_[i]);
            push_lazy(i + i + 1, lazy_[i]);
            lazy_[i] = Id();
        }
        void push_lazy(std::size_t i, F f) const {
            if (i < size_) {
                lazy_[i] = Composition(f, lazy_[i]);
            }
            else if (i - size_ < n_) {
                data_[i - size_] = Mapping(f, data_[i - size_]);
            }
        }

    private:
        mutable std::vector<S> data_;
        mutable std::vector<F> lazy_;
        std::size_t n_, size_, log_;
    };
}

#endif // STCP_DUAL_SEGMENT_TREE_HPP

