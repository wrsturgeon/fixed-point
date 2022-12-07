#ifndef FIXED_POINT_HPP
#define FIXED_POINT_HPP

#include <bit>          // for std::bit_width
#include <cmath>        // for ldexp, ldexpf, std::ldexp, std::ldexpf
#include <concepts>     // for std::integral, std::same_as, std::unsigned_integral
#include <cstdint>      // for int16_t, int32_t, int64_t, int8_t, std::int16_t, std::int32_t, std::int64_t, std::int8_t, std::uint16_t, std::uint32_t, std::uint64_t, std::uint8_t, uint16_t, uint32_t, uint64_t, uint8_t
#include <cstdlib>      // for abs, size_t, std::size_t
#include <type_traits>  // for std::conditional_t

extern "C" {
#include <limits.h>     // for CHAR_BIT
}

#ifndef FP_NOSTR
#include <iostream>     // for std::string, std::__1::operator<<, std::ostream
#include <string>       // for std::allocator, std::operator+, std::char_traits, std::basic_string, std::to_string
#else // FP_NOSTR
#if FP_NOSTR != 1
#error "FP_NOSTR must be either not defined or defined to 1"
#endif // FP_NOSTR != 1
#endif // FP_NOSTR

#define FP_INLINE [[gnu::always_inline]] inline
#define FP_IMPURE [[nodiscard]] FP_INLINE
#define FP_PURE FP_IMPURE constexpr

namespace fp {

namespace util {

// cstdint types with templated bit width
template <std::unsigned_integral auto B> struct s_int_struct {};
template <std::unsigned_integral auto I> struct u_int_struct {};
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAKEINT(B) template <> struct s_int_struct<B##U> { using type = std::int##B##_t; }; template <> struct u_int_struct<B##U> { using type = std::uint##B##_t; };
MAKEINT(8)
MAKEINT(16)
MAKEINT(32)
MAKEINT(64)
#undef MAKEINT
template <typename S> concept valid_sign = std::same_as<S, signed> or std::same_as<S, unsigned>;
template <std::unsigned_integral auto B> using s_int = typename s_int_struct<B>::type;
template <std::unsigned_integral auto B> using u_int = typename u_int_struct<B>::type;
template <std::unsigned_integral auto B, valid_sign S> using int_b = std::conditional_t<std::is_signed_v<S>, s_int<B>, u_int<B>>;

// Bit-shifting operators that sidestep undefined behavior with negative shifts
template <std::integral auto shift_amt, std::integral T>
FP_PURE T rshift(T&& v) noexcept { if constexpr (shift_amt < 0) { return v << -shift_amt; } else { return v >> shift_amt; } }
template <std::integral auto shift_amt, std::integral T>
FP_PURE T lshift(T&& v) noexcept { if constexpr (shift_amt < 0) { return v >> -shift_amt; } else { return v << shift_amt; } }

FP_PURE std::size_t byte_pow2_ceil(std::unsigned_integral auto x) noexcept {
  std::size_t c = CHAR_BIT;
  while (x > c) { c <<= 1U; }
  return c;
}

static_assert(byte_pow2_ceil(8U) == 8U, "As of now, this fixed-point library works only with a byte size of 8.");

template <typename T> using signedness = std::conditional_t<std::is_signed_v<T>, signed, unsigned>;

} // namespace util

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
struct t {
  util::int_b<B, S> internal; // NOLINT(misc-non-private-member-variables-in-classes)
  // no constructor
  FP_IMPURE explicit operator float() const noexcept requires (static_cast<int>(I) == I) { return std::ldexpf(static_cast<float>(internal), static_cast<int>(I)); }
  FP_IMPURE explicit operator double() const noexcept requires (static_cast<int>(I) == I) { return std::ldexp(static_cast<double>(internal), static_cast<int>(I)); }
#ifndef FP_NOSTR
  FP_IMPURE explicit operator std::string() const noexcept { return std::to_string(operator double()); }
#endif // FP_NOSTR
};

template <std::integral auto I>
inline constexpr decltype(auto) from_int = t<
  util::byte_pow2_ceil(std::bit_width(std::abs(I))),
  util::byte_pow2_ceil(std::bit_width(std::abs(I))),
  std::conditional_t<(I < 0), signed, unsigned>>{ .internal = I };

#ifndef FP_NOSTR // String and stream operators

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::ostream& operator<<(std::ostream& os, t<B, I, S>&& v) {
  return os << static_cast<std::string>(v);
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::ostream& operator<<(std::ostream& os, t<B, I, S> const& v) {
  return os << static_cast<std::string>(v);
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(std::string&& s, t<B, I, S>&& v) {
  return s + static_cast<std::string>(v);
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(std::string&& s, t<B, I, S> const& v) {
  return s + static_cast<std::string>(v);
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(std::string const& s, t<B, I, S>&& v) {
  return s + static_cast<std::string>(v);
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(std::string const& s, t<B, I, S> const& v) {
  return s + static_cast<std::string>(v);
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(t<B, I, S>&& v, std::string&& s) {
  return static_cast<std::string>(v) + s;
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(t<B, I, S>&& v, std::string const& s) {
  return static_cast<std::string>(v) + s;
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(t<B, I, S> const& v, std::string&& s) {
  return static_cast<std::string>(v) + s;
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(t<B, I, S> const& v, std::string const& s) {
  return static_cast<std::string>(v) + s;
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(char const* const s, t<B, I, S>&& v) {
  return s + static_cast<std::string>(v);
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(char const* const s, t<B, I, S> const& v) {
  return s + static_cast<std::string>(v);
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(t<B, I, S>&& v, char const* const s) {
  return static_cast<std::string>(v) + s;
}

template <std::unsigned_integral auto B, std::signed_integral auto I, util::valid_sign S>
std::string operator+(t<B, I, S> const& v, char const* const s) {
  return static_cast<std::string>(v) + s;
}

#endif // FP_NOSTR

} // namespace fp

#undef FP_PURE
#undef FP_IMPURE
#undef FP_INLINE

#endif // FIXED_POINT_HPP
