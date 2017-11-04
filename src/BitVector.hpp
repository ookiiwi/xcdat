#ifndef XCDAT_BIT_VECTOR_HPP_
#define XCDAT_BIT_VECTOR_HPP_

#include "BitVectorBuilder.hpp"
#include "Vector.hpp"

namespace xcdat {

// Bit vector supporting Rank/Select operations.
class BitVector {
public:
  BitVector() = default;
  ~BitVector() = default;

  explicit BitVector(std::istream &is);
  explicit BitVector(BitVectorBuilder& builder,
                     bool rank_flag, bool select_flag);

  bool operator[](size_t i) const {
    return (bits_[i / 32] & (1U << (i % 32))) != 0;
  }

  // the number of 1s in B[0,i).
  id_type rank(size_t i) const;
  // the position of the i+1 th occurrence.
  id_type select(size_t i) const;

  size_t num_1s() const {
    return num_1s_;
  }
  size_t num_0s() const {
    return size_ - num_1s_;
  }

  // the number of bits
  size_t size() const {
    return size_;
  }

  size_t size_in_bytes() const;

  void write(std::ostream &os) const;

  BitVector(const BitVector&) = delete;
  BitVector& operator=(const BitVector&) = delete;

  BitVector(BitVector&&) noexcept = default;
  BitVector& operator=(BitVector&&) noexcept = default;

private:
  static constexpr id_type kBitsInR1 {256};
  static constexpr id_type kBitsInR2 {32};
  static constexpr id_type kR1PerR2 {kBitsInR1 / kBitsInR2}; // 8
  static constexpr id_type kNum1sPerTip {512};

  struct RankTip {
    id_type L1;
    uint8_t L2[kR1PerR2];
  };

  Vector<uint32_t> bits_ {};
  Vector<RankTip> rank_tips_ {};
  Vector<id_type> select_tips_ {};
  size_t size_ {};
  size_t num_1s_ {};
};

} //namespace - xcdat

#endif //XCDAT_BIT_VECTOR_HPP_
