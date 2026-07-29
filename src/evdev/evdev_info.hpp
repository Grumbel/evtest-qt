// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_EVDEV_INFO_HPP
#define HEADER_EVTEST_QT_EVDEV_INFO_HPP

#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <map>
#include <vector>
#include <array>
#include <string>
#include <linux/input.h>
#include <stdexcept>

#include "util/bits.hpp"

namespace evtest_qt {

class AbsInfo
{
public:
  int32_t value;
  int32_t minimum;
  int32_t maximum;
  int32_t fuzz;
  int32_t flat;
  int32_t resolution;

  AbsInfo() :
    value(),
    minimum(),
    maximum(),
    fuzz(),
    flat(),
    resolution()
  {
  }

  AbsInfo(input_absinfo const& absinfo) :
    value(absinfo.value),
    minimum(absinfo.minimum),
    maximum(absinfo.maximum),
    fuzz(absinfo.fuzz),
    flat(absinfo.flat),
    resolution(absinfo.resolution)
  {
  }
};

class EvdevInfo
{
public:
  int version;
  std::string name;
  std::string phys;
  struct input_id id;
  std::array<unsigned long, bits::nbits(EV_MAX)> bit;
  std::array<unsigned long, bits::nbits(ABS_MAX)> abs_bit;
  std::array<unsigned long, bits::nbits(REL_MAX)> rel_bit;
  std::array<unsigned long, bits::nbits(KEY_MAX)> key_bit;

  std::map<uint16_t, AbsInfo> absinfos;

  std::vector<uint16_t> abss;
  std::vector<uint16_t> rels;
  std::vector<uint16_t> keys;

public:
  EvdevInfo() :
    version(),
    name(),
    phys(),
    id(),
    bit(),
    abs_bit(),
    rel_bit(),
    key_bit(),
    absinfos(),
    abss(),
    rels(),
    keys()
  {
  }

  EvdevInfo(int version_,
            std::string name_,
            std::string phys_,
            input_id id_,
            std::array<unsigned long, bits::nbits(EV_MAX)> bit_,
            std::array<unsigned long, bits::nbits(ABS_MAX)> abs_bit_,
            std::array<unsigned long, bits::nbits(REL_MAX)> rel_bit_,
            std::array<unsigned long, bits::nbits(KEY_MAX)> key_bit_,
            std::map<uint16_t, AbsInfo> absinfos_) :
    version(version_),
    name(std::move(name_)),
    phys(std::move(phys_)),
    id(id_),
    bit(std::move(bit_)),
    abs_bit(std::move(abs_bit_)),
    rel_bit(std::move(rel_bit_)),
    key_bit(std::move(key_bit_)),
    absinfos(std::move(absinfos_)),
    abss(),
    rels(),
    keys()
  {
    for(uint16_t i = 0; i < ABS_MAX; ++i)
    {
      if (bits::test_bit(i, abs_bit.data()))
      {
        abss.push_back(i);
      }
    }

    for(uint16_t i = 0; i < REL_MAX; ++i)
    {
      if (bits::test_bit(i, rel_bit.data()))
      {
        rels.push_back(i);
      }
    }

    for(uint16_t i = 0; i < KEY_MAX; ++i)
    {
      if (bits::test_bit(i, key_bit.data()))
      {
        keys.push_back(i);
      }
    }
  }

  bool has_key(uint16_t code) const
  {
    return bits::test_bit(code, key_bit.data());
  }

  bool has_abs(uint16_t code) const
  {
    return bits::test_bit(code, abs_bit.data());
  }

  bool has_rel(uint16_t code) const
  {
    return bits::test_bit(code, rel_bit.data());
  }

  size_t get_key_idx(uint16_t code) const
  {
    auto it = std::find(keys.begin(), keys.end(), code);
    assert(it != keys.end());
    return static_cast<size_t>(it - keys.begin());
  }

  size_t get_rel_idx(uint16_t code) const
  {
    auto it = std::find(rels.begin(), rels.end(), code);
    assert(it != rels.end());
    return static_cast<size_t>(it - rels.begin());
  }

  size_t get_abs_idx(uint16_t code) const
  {
    auto it = std::find(abss.begin(), abss.end(), code);
    assert(it != abss.end());
    return static_cast<size_t>(it - abss.begin());
  }

  AbsInfo get_absinfo(uint16_t code) const
  {
    auto it = absinfos.find(code);
    if (it == absinfos.end())
    {
      throw std::runtime_error("invalid code");
    }
    else
    {
      return it->second;
    }
  }
};

} // namespace evtest_qt

#endif

/* EOF */
