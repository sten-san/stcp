#ifndef STCP_BINARY_TRIE_ARRAY_HPP
#define STCP_BINARY_TRIE_ARRAY_HPP

#include <type_traits>
#include <utility>
#include <stdexcept>
#include <cassert>
#include <cstddef>

namespace stcp {
    template <std::size_t Bits>
    struct binary_trie_array {
        static_assert(0 < Bits);

        constexpr static std::size_t bits = Bits;

        // O(1)
        binary_trie_array(std::size_t max_range_) noexcept:
            xor_all{ 0 }, max_range(max_range_), root(max_range_) {
        }

        // O(1)
        // 0 <= v < 2^Bits
        void apply_xor(std::size_t v) noexcept {
            xor_all ^= v;
        }

        // O(log max_range)
        std::size_t size(std::size_t l, std::size_t r) const {
            assert(l <= r && r <= max_range);

            return root.count(l, r);
        }

        // O(log max_range)
        std::size_t size(std::size_t k) const {
            assert(k < max_range);

            return size(k, k + 1);
        }

        // O(1)
        std::size_t size() const {
            return root.count();
        }

        // O(Bits log max_range)
        // 0 <= v < 2^Bits
        std::size_t count(std::size_t l, std::size_t r, std::size_t v) const noexcept {
            assert(l <= r && r <= max_range);

            return count_(l, r, v ^ xor_all);
        }
        // O(Bits log max_range)
        // 0 <= v < 2^Bits
        bool exist(std::size_t l, std::size_t r, std::size_t v) const noexcept {
            assert(l <= r && r <= max_range);

            return exist_(l, r, v ^ xor_all);
        }

        // O(Bits log max_range)
        // 0 <= v < 2^Bits
        void insert(std::size_t k, std::size_t v) {
            assert(k < max_range);

            insert_(k, v ^ xor_all);
        }
        // O(Bits log max_range)
        // 0 <= v < 2^Bits
        void erase(std::size_t k, std::size_t v) noexcept {
            assert(k < max_range);

            erase_(k, v ^ xor_all);
        }

        // O(Bits log max_range)
        // 0 <= n < size(binary_trie)
        std::size_t nth_element(std::size_t l, std::size_t r, std::size_t n) const {
            assert(l <= r && r <= max_range);

            if (size(l, r) <= n) {
                throw std::out_of_range("binary_trie");
            }

            std::size_t path = 0;

            auto b = (std::size_t(1) << (Bits - 1));

            const node *iter = &root; 
            while (0 < b) {
                auto m = !!(xor_all & b);
                auto c = iter->c[m];
                b >>= 1; path <<= 1;

                if (c != nullptr) {
                    if (n < c->count(l, r)) {
                        iter = c;
                        path |= 0;
                        continue;
                    }
                    n -= c->count(l, r);
                }

                iter = iter->c[!m];
                path |= 1;
            }

            return path;
        }

        // O(Bits log max_range)
        // 0 <= v < 2^Bits
        std::size_t lower_bound(std::size_t l, std::size_t r, std::size_t v) const noexcept {
            assert(l <= r && r <= max_range);

            if (v == 0) {
                return 0;
            }
            return upper_bound(l, r, v - 1);
        }

        // O(Bits log max_range)
        // 0 <= v < 2^Bits
        std::size_t upper_bound(std::size_t l, std::size_t r, std::size_t v) const noexcept {
            assert(l <= r && r <= max_range);

            auto b = (std::size_t(1) << (Bits - 1));

            std::size_t sum = 0;

            const node *iter = &root;
            while (0 < b) {
                auto m = !!(xor_all & b);
                auto f = !!(v & b);
                auto c = iter->c[m];

                if (c != nullptr && f) {
                    sum += c->count(l, r);
                }

                auto d = iter->c[m ^ f];
                if (d == nullptr) {
                    return sum;
                }
                iter = d;

                b >>= 1;
            }
            sum += iter->count(l, r);

            return sum;
        }

    private:
        template <typename S, S (*Op)(S, S), S (*E)()>
        struct dynamic_segment_tree {
            using value_type = S;

            // O(1)
            dynamic_segment_tree() noexcept:
                dynamic_segment_tree(0) {
            }

            // O(1)
            dynamic_segment_tree(std::size_t n) noexcept:
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
            S get(std::size_t i) const {
                assert(0 <= i && i < n_);

                return get(root_, 0, n_, i);
            }

            // O(1)
            std::size_t size() const noexcept {
                return n_;
            }

            // O(log size(dynamic_segment_tree))
            // 0 <= l <= r <= size(dynamic_segment_tree)
            S prod(std::size_t l, std::size_t r) const {
                assert(0 <= l && l <= r && r <= n_);

                return prod(root_, 0, n_, l, r);
            }

            // O(1)
            S all_prod() const {
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

            static S get(const node *range, std::size_t l, std::size_t r, std::size_t i) noexcept {
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

        static constexpr int op(int x, int y) noexcept {
            return x + y;
        }
        static constexpr int elem() noexcept {
            return 0;
        }

        using segtree = dynamic_segment_tree<int, op, elem>;

    private:
        struct node {
            node(std::size_t max_range) noexcept:
                iset(max_range), c{ nullptr, nullptr } {
            }

            ~node() {
                if (c[0] != nullptr) delete c[0];
                if (c[1] != nullptr) delete c[1];
            }

            node *advance(bool f) {
                if (c[f] == nullptr) {
                    c[f] = new node(iset.size());
                }
                return c[f];
            }

            std::size_t count(std::size_t l, std::size_t r) const {
                return iset.prod(l, r);
            }
            std::size_t count() const {
                return iset.all_prod();
            }

            void insert(int index) {
                iset.set(index, iset.get(index) + 1);
            }
            void erase(int index) {
                iset.set(index, iset.get(index) - 1);
            }

            segtree iset;
            node *c[2];
        };

        std::size_t count_(std::size_t l, std::size_t r, std::size_t n) const noexcept {
            auto b = (std::size_t(1) << (Bits - 1));

            const node *iter = &root;
            while (0 < b) {
                iter = iter->c[!!(n & b)];
                if (iter == nullptr || iter->count(l, r) == 0) {
                    return 0;
                }
                b >>= 1;
            }

            return iter->count(l, r);
        }
        bool exist_(std::size_t l, std::size_t r, std::size_t n) const noexcept {
            return 0 < count_(l, r, n);
        }

        void insert_(std::size_t k, std::size_t n) {
            auto b = (std::size_t(1) << (Bits - 1));

            node *iter = &root; root.insert(k);
            while (0 < b) {
                iter = iter->advance(!!(n & b)); iter->insert(k);
                b >>= 1;
            }
        }
        void erase_(std::size_t k, std::size_t n) noexcept {
            if (exist_(k, k + 1, n)) {
                auto b = (std::size_t(1) << (Bits - 1));

                node *iter = &root; root.erase(k);
                while (0 < b) {
                    iter = iter->c[!!(n & b)]; iter->erase(k);
                    b >>= 1;
                }
            }
        }

        std::size_t xor_all;
        std::size_t max_range;
        node root;
    };
}

#endif // STCP_BINARY_TRIE_ARRAY_HPP

