#include <cstdint>
#include <peekpoke.h>
#include <climits>

template<uint8_t ISize, uint8_t FSize>
class FixedPoint {
private:
    template <typename R>
    static constexpr R BitMask(unsigned int const onecount) {
        return static_cast<R>(-(onecount != 0)) & (static_cast<R>(-1) >> ((sizeof(R) * CHAR_BIT) - onecount));
    }

    static constexpr uint8_t Size = (ISize + FSize);
    using IntType = _BitInt(((ISize + 7) / 8) * 8);
    using FracType = _BitInt(((FSize + 7) / 8) * 8);
    using StorageType = _BitInt(((Size + 7) / 8) * 8);
    StorageType val;
    static constexpr unsigned long INT_MASK = BitMask<unsigned long>(ISize) << FSize;
    static constexpr unsigned long FRAC_MASK = BitMask<unsigned long>(FSize);
public:
    constexpr explicit FixedPoint(IntType i) { SetI(i); }

    template<uint8_t OI, uint8_t OF>
    constexpr FixedPoint(const FixedPoint<OI, OF>& o) {
        val = FixedPoint<ISize, FSize>(o.AsI(), o.AsF()).Get();
    }
    constexpr FixedPoint(const FixedPoint& o) : val(o.val) {}

    constexpr explicit FixedPoint(IntType i, FracType f) {
        SetI(i);
        SetF(f);
    }

    constexpr FixedPoint(long double f) {
        Set(f * (1 << FSize));
    }

    constexpr FixedPoint& operator=(FixedPoint o)
    {
        val = o.val;
        return *this;
    }

    constexpr FixedPoint operator +(const FixedPoint& o) const {
        FixedPoint<ISize, FSize> n = val;
        n += o;
        return n;
    }
    constexpr FixedPoint& operator +=(const FixedPoint& o) {
        val += o.val;
        return *this;
    }

    constexpr FixedPoint operator -(const FixedPoint& o) const {
        FixedPoint<ISize, FSize> n = val;
        n -= o;
        return n;
    }
    constexpr FixedPoint& operator -=(const FixedPoint& o) {
        val -= o.val;
        return *this;
    }

    constexpr FixedPoint operator/(uint8_t i) {
        FixedPoint<ISize, FSize> n{val};
        n /= i;
        return n;
    }
    constexpr FixedPoint& operator/=(uint8_t i) {
        val /= i;
        return *this;
    }

    constexpr FixedPoint operator*(uint8_t i) {
        FixedPoint<ISize, FSize> n{val};
        n *= i;
        return n;
    }
    constexpr FixedPoint& operator*=(uint8_t i) {
        val *= i;
        return *this;
    }

    constexpr FixedPoint operator >>(uint8_t val) const {
        FixedPoint<ISize, FSize> n = *this;
        n >>= val;
        return n;
    }
    constexpr FixedPoint& operator >>=(uint8_t v) {
        val >>= v;
        return *this;
    }
    
    constexpr FixedPoint operator <<(uint8_t val) const {
        FixedPoint<ISize, FSize> n = *this;
        n <<= val;
        return n;
    }
    constexpr FixedPoint& operator <<=(uint8_t v) {
        val <<= v;
        return *this;
    }

    template<uint8_t OI, uint8_t OF>
    constexpr FixedPoint<OI, OF> As() const {
        FixedPoint<OI, OF> o(AsI(), AsF());
        return o;
    }
    constexpr IntType AsI() const { return (val & INT_MASK) >> FSize; }

    constexpr FracType AsF() const { return (val & FRAC_MASK); }

    constexpr void SetI(IntType v) { val &= ~INT_MASK; val |= static_cast<StorageType>((v) & (INT_MASK >> FSize)) << FSize; }
    constexpr void SetF(FracType v) { val &= ~FRAC_MASK; val |= (v & FRAC_MASK); }
    constexpr void Set(StorageType v) { val = v; }
    constexpr StorageType Get() const { return val; }


    constexpr friend bool operator==(const FixedPoint& a, const FixedPoint& b) noexcept {
        return a.val == b.val;
    }
    constexpr friend bool operator< (const FixedPoint& a, const FixedPoint& b) noexcept {
        return a.val < b.val;
    }
    constexpr friend bool operator!=(const FixedPoint& a, const FixedPoint& b) noexcept {
        return !(a == b);
    }
    constexpr friend bool operator> (const FixedPoint& a, const FixedPoint& b) noexcept {
        return b < a;
    }
    constexpr friend bool operator>=(const FixedPoint& a, const FixedPoint& b) noexcept {
        return !(a < b);
    }
    constexpr friend bool operator<=(const FixedPoint& a, const FixedPoint& b) noexcept {
        return !(b < a);
    }

    constexpr friend bool operator==(const FixedPoint& a, IntType b) noexcept {
        return a.val == b;
    }
    constexpr friend bool operator< (const FixedPoint& a, IntType b) noexcept {
        return a.val < b;
    }
    constexpr friend bool operator!=(const FixedPoint& a, IntType b) noexcept {
        return !(a == b);
    }
    constexpr friend bool operator> (const FixedPoint& a, IntType b) noexcept {
        return b < a;
    }
    constexpr friend bool operator>=(const FixedPoint& a, IntType b) noexcept {
        return !(a < b);
    }
    constexpr friend bool operator<=(const FixedPoint& a, IntType b) noexcept {
        return !(b < a);
    }

    constexpr friend bool operator==(IntType a, const FixedPoint& b) noexcept {
        return a == b.val;
    }
    constexpr friend bool operator< (IntType a, const FixedPoint& b) noexcept {
        return a < b.val;
    }
    constexpr friend bool operator!=(IntType a, const FixedPoint& b) noexcept {
        return !(a == b);
    }
    constexpr friend bool operator> (IntType a, const FixedPoint& b) noexcept {
        return b < a;
    }
    constexpr friend bool operator>=(IntType a, const FixedPoint& b) noexcept {
        return !(a < b);
    }
    constexpr friend bool operator<=(IntType a, const FixedPoint& b) noexcept {
        return !(b < a);
    }
};
namespace FixedPointLiterals {

constexpr FixedPoint<8, 8> operator ""_8_8(long double fixed) {
    return {fixed};
}
constexpr FixedPoint<12, 4> operator ""_12_4(long double fixed) {
    return {fixed};
}

using FP_8_8 = FixedPoint<8, 8>;
using FP_12_4 = FixedPoint<12, 4>;
}