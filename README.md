# Fixed-Point Numerical Representation

Super simple implementation of fixed-point numbers in C++.

We use a `struct` with no constructor (instead of a `class`) for a few reasons:
  1. C++ allows custom data structures in constant expressions if and only if they meet a number of esoteric constraints, one of which is to have no private members. The internal integral representation of the number is stored as the member `internal`.
  2. It's surprisingly easy to confuse (a.) making a fixed-point number to represent a certain integer and (b.) making a fixed-point number whose internal representation is a certain integer. The former can be accomplished with the non-member expression `fp::from_int<...>`, while the latter can be accomplished with `fp::t<...>{ .internal=... }`.

Astronomically high and low radix points are allowed and theoretically bounded only by your hardware. You can make, for example, an 8-bit fixed-point number that represents the range from 2^48 to 2^56, or from 2^-729 to 2^-721. The range-precision tradeoff is up to your discretion.

The struct also comes ready to convert to `float`/`double` and to stringify/streamify to print or write as text. `fp<...> x; std::cout << x << std::endl;` works out of the box.

## Syntax

Fixed-point numbers take three template arguments:
  1. the number of bits in the internal representation;
  2. the number of integral bits (left of the radix point); and
  3. whether the representation is signed or unsigned.

For example, `fp::t<8, 5, unsigned>` takes up exactly 8 bits of memory and can represent any value from zero to 31.875, spaced by 0.125. If we were to inrease 5 to 6, we'd be able to represent up to 63.75, but our representable numbers would be spaced by 0.25: hence the range-precision tradeoff.
