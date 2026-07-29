// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "evdev_enum.hpp"

#include <format>

#include <libevdev/libevdev.h>
#include <linux/input.h>

namespace evtest_qt {

namespace {

std::string code_name_or_fallback(unsigned int type, uint16_t code,
                                  char const* prefix)
{
  if (char const* name = libevdev_event_code_get_name(type, code)) {
    return name;
  }
  return std::format("{}#{}", prefix, code);
}

} // namespace

std::string evdev_abs_name(uint16_t code)
{
  return code_name_or_fallback(EV_ABS, code, "ABS_");
}

std::string evdev_key_name(uint16_t code)
{
  return code_name_or_fallback(EV_KEY, code, "KEY_");
}

std::string evdev_rel_name(uint16_t code)
{
  return code_name_or_fallback(EV_REL, code, "REL_");
}

} // namespace evtest_qt

/* EOF */
