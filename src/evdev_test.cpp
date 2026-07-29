// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <array>
#include <cerrno>
#include <cstring>
#include <print>

#include "evdev/evdev_device.hpp"
#include "evdev/evdev_enum.hpp"

namespace evtest_qt {

void print_evdev_info(EvdevInfo const& info)
{
  std::println("name: '{}'", info.name);

  if (!info.abss.empty())
  {
    std::println("abs: {}", info.abss.size());
    for (size_t i = 0; i < info.abss.size(); ++i)
    {
      auto absinfo = info.get_absinfo(info.abss[i]);
      std::println("  {} value:{} min:{} max:{} fuzz:{} flat:{} res:{}",
                   evdev_abs_name(info.abss[i]),
                   absinfo.value,
                   absinfo.minimum,
                   absinfo.maximum,
                   absinfo.fuzz,
                   absinfo.flat,
                   absinfo.resolution);
    }
    std::println("");
  }

  if (!info.rels.empty())
  {
    std::println("rel: {}", info.rels.size());
    for (size_t i = 0; i < info.rels.size(); ++i)
    {
      std::println("  {}", evdev_rel_name(info.rels[i]));
    }
    std::println("");
  }

  if (!info.keys.empty())
  {
    std::println("key: {}", info.keys.size());
    for (size_t i = 0; i < info.keys.size(); ++i)
    {
      std::println("  {}", evdev_key_name(info.keys[i]));
    }
    std::println("");
  }
}

void print_events(EvdevDevice& device)
{
  std::println("reading events...");
  std::array<struct input_event, 1> ev;
  while (true)
  {
    ssize_t num_events = device.read_events(ev.data(), ev.size());
    if (num_events < 0)
    {
      std::println("error: {}: {}", num_events, strerror(errno));
      break;
    }

    for (size_t i = 0; i < static_cast<size_t>(num_events); ++i)
    {
      if (ev[i].type == EV_SYN)
      {
        std::println("--------- sync ---------");
      }
      else
      {
        switch (ev[i].type)
        {
          case EV_ABS:
            std::println("{:>8} {:>8} {:>8}",
                         ev[i].type, evdev_abs_name(ev[i].code), ev[i].value);
            break;

          case EV_REL:
            std::println("{:>8} {:>8} {:>8}",
                         ev[i].type, evdev_rel_name(ev[i].code), ev[i].value);
            break;

          case EV_KEY:
            std::println("{:>8} {:>8} {:>8}",
                         ev[i].type, evdev_key_name(ev[i].code), ev[i].value);
            break;

          default:
            std::println("{:>8} {:>8} {:>8}",
                         ev[i].type, ev[i].code, ev[i].value);
            break;
        }
      }
    }
  }
}

} // namespace evtest_qt

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::println("Usage: {} DEVICE", argv[0]);
    return 1;
  }

  try
  {
    auto device = evtest_qt::EvdevDevice::open(argv[1]);
    auto info = device->read_evdev_info();

    print_evdev_info(info);
    print_events(*device);

    return 0;
  }
  catch (std::exception const& err)
  {
    std::println(stderr, "error: {}", err.what());
    return 1;
  }
}

/* EOF */
