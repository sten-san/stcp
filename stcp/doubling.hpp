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

        size_t log2() const noexcept {
            return log2_;
        }

        const T &operator [](size_t i) {
            return dub_[i];
        }

        T operator ()(int_type d) {
            auto acc = dub_[0];

            auto iter = std::begin(dub_);
            while (0 < d) {
                if (d & 1) {
                    acc = op_(acc, *iter);
                }
                ++iter; d >>= 1;
            }

            return acc;
        }

        template <typename Apply>
        auto operator ()(Apply &&apply) {
            return [this, apply = std::forward<Apply>(apply)](int_type d, auto ...args) {
                auto t = make_tuple(args...);
                auto r = d;

                auto iter = std::begin(dub_);
                while (0 < r) {
                    if (r & 1) {
                        t = apply(*iter, t);
                    }
                    ++iter; r >>= 1;
                }

                return t;
            };
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

