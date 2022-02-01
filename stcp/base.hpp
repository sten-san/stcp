#ifndef STCP_BASE_HPP
#define STCP_BASE_HPP

#include <utility>
#include <algorithm>
#include <numeric>
#include <iterator>

#include <optional>
#include <tuple>

#include <limits>
#include <cstdint>
#include <cstddef>
#include <cassert>

namespace stcp {
    using std::size_t;
    using std::int8_t;
    using std::uint8_t;
    using std::int16_t;
    using std::uint16_t;
    using std::int32_t;
    using std::uint32_t;
    using std::int64_t;
    using std::uint64_t;

    using std::pair;
    using std::tuple;
    using std::optional;
    using std::nullopt;
    using std::make_pair;
    using std::make_tuple;

    template <typename T>
    const inline auto inf = std::numeric_limits<T>::max();

    template <typename T>
    T &chmin(T &l, const T &r) {
        return l = std::min(l, r);
    }
    template <typename T, typename Compare>
    T &chmin(T &l, const T &r, Compare c) {
        return l = std::min(l, r, std::move(c));
    }

    template <typename T>
    T &chmax(T &l, const T &r) {
        return l = std::max(l, r);
    }
    template <typename T, typename Compare>
    T &chmax(T &l, const T &r, Compare c) {
        return l = std::max(l, r, std::move(c));
    }
}

#ifdef STCP_ENABLE_MULTIPRECISION

#include <boost/multiprecision/cpp_int.hpp>

namespace stcp {
    using bint = boost::multiprecision::cpp_int;

    template <>
    const inline bint inf<bint> = (bint(1) << 1023) - 1;
}

#endif // STCP_ENABLE_MULTIPRECISION

#endif // STCP_BASE_HPP

