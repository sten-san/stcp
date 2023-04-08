#ifndef STCP_DOUBLING_HPP
#define STCP_DOUBLING_HPP

#include <vector>
#include <type_traits>
#include "stcp/base.hpp"

namespace stcp {
    template <typename Int, typename T, typename Op>
    struct doubling {
        using int_type = Int;

        static_assert(
            std::is_invocable_r_v<T, Op, const T, const T>,
            "stcp::doubling Cannot call with 'T Op(T, T)'"
        );

        doubling(size_t log2, T x, Op op):
            log2_(log2), dub_(), op_(std::move(op)) {

            dub_.push_back(x);
            for (size_t i = 0; i < log2; ++i) {
                x = op_(x, x);
                dub_.push_back(x);
            }
        }

        // 0 <= a1 < a2 < ... < ak <= log2
        // 2^a1 + 2^a2 + ... + 2^ak = x
        // f(x^(2^a1)), f(x^(2^a2)), ..., f(x^(2^ak))
        template <typename F>
        void parse(int_type x, F f) {
            auto iter = std::cbegin(dub_);

            while (0 < x) {
                if (x & 1) {
                    f(*iter);
                }
                ++iter; x >>= 1;
            }
        }

        // x * x * ... * x
        // |--- 0 < n ---|
        auto repeat(int_type n) {
            assert(0 < n);

            auto acc = dub_[0];
            parse(n - 1, [&](auto &f) {
                acc = op_(acc, f);
            });

            return acc;
        }

        template <typename Apply>
        auto connect(Apply &&apply) {
            return [this, apply = std::forward<Apply>(apply)](int_type x, auto ...args) {
                auto t = make_tuple(args...);

                parse(x, [&](auto &f) {
                    t = apply(f, t);
                });

                return t;
            };
        }

        size_t log2() const noexcept {
            return log2_;
        }

        const T &operator [](size_t i) const {
            return dub_[i];
        }

    private:
        const size_t log2_;
        std::vector<T> dub_;
        Op op_;
    };

    template <typename Int, typename T, typename Op>
    auto make_doubling(size_t log2, T x, Op op) {
        return doubling<Int, T, Op>(log2, std::move(x), std::move(op));
    }
}

#endif

