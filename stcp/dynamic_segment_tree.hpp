#ifndef DYNAMIC_SEGMENT_TREE_HPP
#define DYNAMIC_SEGMENT_TREE_HPP

#include <type_traits>
#include <utility>
#include <cassert>
#include <cstddef>

namespace stcp {
    template <typename S, S (*Op)(S, S), S (*E)()>
    struct dynamic_segment_tree {
        using value_type = S;

        // O(1)
        dynamic_segment_tree():
            dynamic_segment_tree(0) {
        }

        // O(1)
        dynamic_segment_tree(std::size_t n):
            n_(n), root_(nullptr) {
        }

        ~dynamic_segment_tree() {
            if (root_ != nullptr) delete root_;
        }

    public:
        // O(log size(dynamic_segment_tree))
        // 0 <= i < size(dynamic_segment_tree)
        void set(std::size_t i, S x) {
            assert(0 <= i && i < n_);

            update_tree(root_, 0, n_, i, x);
        }

        // O(log size(dynamic_segment_tree))
        // 0 <= i < size(dynamic_segment_tree)
        const S &get(std::size_t i) const {
            assert(0 <= i && i < n_);

            return get(root_, 0, n_, i);
        }

        // O(log size(dynamic_segment_tree))
        // 0 <= l <= r <= size(dynamic_segment_tree)
        S prod(std::size_t l, std::size_t r) const {
            assert(0 <= l && l <= r && r <= n_);

            return prod(root_, 0, n_, l, r);
        }

        // O(1)
        const S &all_prod() const {
            if (root_ != nullptr) {
                return root_->prod;
            }
            return E();
        }

        // O(log size(dynamic_segment_tree))
        // 0 <= l <= size(dynamic_segment_tree)
        template <typename F>
        std::size_t max_right(std::size_t l, F &&f) const {
            static_assert(std::is_invocable_r_v<bool, F, S>);

            assert(0 <= l && l <= n_);
            assert(std::forward<F>(f)(E()));

            S acc = E();
            return max_right(root_, 0, n_, l, [&](auto &&x) mutable {
                return std::forward<F>(f)(std::forward<decltype(x)>(x));
            }, acc);
        }

        // O(log size(dynamic_segment_tree))
        // 0 <= r <= size(dynamic_segment_tree)
        template <typename F>
        std::size_t min_left(std::size_t r, F &&f) const {
            static_assert(std::is_invocable_r_v<bool, F, S>);

            assert(0 <= r && r <= n_);
            assert(std::forward<F>(f)(E()));

            S acc = E();
            return min_left(root_, 0, n_, r, [&](auto &&x) mutable {
                return std::forward<F>(f)(std::forward<decltype(x)>(x));
            }, acc);
        }

    private:
        struct node {
            node(std::size_t i, S value):
                i(i), value(value), prod(value), l(nullptr), r(nullptr) {
            }

            ~node() {
                if (l != nullptr) delete l;
                if (r != nullptr) delete r;
            }

            void update() {
                prod = Op(Op(
                    l != nullptr ? l->prod : E(),
                    value),
                    r != nullptr ? r->prod : E()
                );
            }

            std::size_t i;
            S value, prod;
            node *l, *r;
        };

        static void update_tree(node* &range, std::size_t l, std::size_t r, std::size_t i, S x) {
            using std::swap;

            if (range == nullptr) {
                range = new node(i, x);
                return;
            }

            if (range->i == i) {
                range->value = x;
                range->update();
                return;
            }

            auto m = l + (r - l) / 2;
            if (i < m) {
                if (range->i < i) {
                    swap(range->i, i);
                    swap(range->value, x);
                }
                update_tree(range->l, l, m, i, x);
            }
            else {
                if (i < range->i) {
                    swap(range->i, i);
                    swap(range->value, x);
                }
                update_tree(range->r, m, r, i, x);
            }

            range->update();
        }

        static const S &get(const node *range, std::size_t l, std::size_t r, std::size_t i) {
            if (range == nullptr) {
                return E();
            }

            if (range->i == i) {
                return range->value;
            }

            auto m = l + (r - l) / 2;
            if (i < m) {
                return get(range->l, l, m, i);
            }
            return get(range->r, m, r, i);
        }

        static S prod(const node *range, std::size_t l, std::size_t r, std::size_t query_l, std::size_t query_r) {
            if (range == nullptr || r <= query_l || query_r <= l) {
                return E();
            }

            if (query_l <= l && r <= query_r) {
                return range->prod;
            }

            auto m = l + (r - l) / 2;

            S acc = prod(range->l, l, m, query_l, query_r);
            if (query_l <= range->i && range->i < query_r) {
                acc = Op(acc, range->value);
            }
            return Op(acc, prod(range->r, m, r, query_l, query_r));
        }

        template <typename F>
        static std::size_t max_right(const node *range, std::size_t l, std::size_t r, std::size_t query_l, F f, S &acc) {
            if (range == nullptr || r <= query_l) {
                return r;
            }

            auto m = l + (r - l) / 2;

            auto max_right_l = max_right(range->l, l, m, query_l, f, acc);
            if (max_right_l < m) {
                return max_right_l;
            }

            if (query_l <= range->i) {
                if (S con = Op(acc, range->value); f(con)) {
                    acc = con;
                }
                else {
                    return range->i;
                }
            }

            if (query_l <= m) {
                if (range->r == nullptr) {
                    return r;
                }

                if (S con = Op(acc, range->r->prod); f(con)) {
                    acc = con;
                    return r;
                }
            }

            return max_right(range->r, m, r, query_l, f, acc);
        }

        template <typename F>
        static std::size_t min_left(const node *range, std::size_t l, std::size_t r, std::size_t query_r, F f, S &acc) {
            if (range == nullptr || query_r <= l) {
                return l;
            }

            auto m = l + (r - l) / 2;

            auto min_left_r = min_left(range->r, m, r, query_r, f, acc);
            if (m < min_left_r) {
                return min_left_r;
            }

            if (range->i < query_r) {
                if (S con = Op(range->value, acc); f(con)) {
                    acc = con;
                }
                else {
                    return range->i + 1;
                }
            }

            if (m <= query_r) {
                if (range->l == nullptr) {
                    return l;
                }

                if (S con = Op(range->l->prod, acc); f(con)) {
                    acc = con;
                    return l;
                }
            }

            return min_left(range->l, l, m, query_r, f, acc);
        }

    private:
        std::size_t n_;
        node *root_;
    };
}

#endif // DYNAMIC_SEGMENT_TREE_HPP

