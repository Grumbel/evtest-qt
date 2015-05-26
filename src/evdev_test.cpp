// evtest-qt - A graphical joystick tester
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <iomanip>
#include <string.h>

#include "evdev_device.hpp"
#include "evdev_enum.hpp"

void print_evdev_info(const EvdevInfo& info)
{
  std::cout << "name: '" << info.name << "'" << std::endl;

  if (!info.abss.empty())
  {
    std::cout << "abs: " << info.abss.size() << "\n";
    for(size_t i = 0; i < info.abss.size(); ++i)
    {
      auto absinfo = info.get_absinfo(info.abss[i]);
      std::cout << "  " << evdev_abs_name(info.abss[i])
                << " value:" << absinfo.value
                << " min:" << absinfo.minimum
                << " max:" <<  absinfo.maximum
                << " fuzz:" <<  absinfo.fuzz
                << " flat:" << absinfo.flat
                << " res:" << absinfo.resolution << "\n";
    }
    std::cout << "\n";
  }

  if (!info.rels.empty())
  {
    std::cout << "rel: " << info.rels.size() << "\n";
    for(size_t i = 0; i < info.rels.size(); ++i)
    {
      std::cout << "  " << evdev_rel_name(info.rels[i]) << "\n";
    }
    std::cout << "\n";
  }

  if (!info.keys.empty())
  {
    std::cout << "key: " << info.keys.size() << "\n";
    for(size_t i = 0; i < info.keys.size(); ++i)
    {
      std::cout << "  " << evdev_key_name(info.keys[i]) << "\n";
    }
    std::cout << "\n";
  }
}

void print_events(EvdevDevice& device)
{
  std::cout << "reading events..." << std::endl;
  std::array<struct input_event, 1> ev;
  while(true)
  {
    ssize_t num_events = device.read_events(ev.data(), ev.size());
    if (num_events < 0)
    {
      std::cout << "error: " << num_events << ": " << strerror(errno) << std::endl;
      break;
    }
    else
    {
      for(ssize_t i = 0; i < num_events; ++i)
      {
        if (ev[i].type == EV_SYN)
        {
          std::cout << "--------- sync ---------" << std::endl;
        }
        else
        {
          switch(ev[i].type)
          {
            case EV_ABS:
              std::cout << std::setw(8) << ev[i].type << " "
                        << std::setw(8) << evdev_abs_name(ev[i].code) << " "
                        << std::setw(8) << ev[i].value << std::endl;
              break;

            case EV_REL:
              std::cout << std::setw(8) << ev[i].type << " "
                        << std::setw(8) << evdev_rel_name(ev[i].code) << " "
                        << std::setw(8) << ev[i].value << std::endl;
              break;

            case EV_KEY:
              std::cout << std::setw(8) << ev[i].type << " "
                        << std::setw(8) << evdev_key_name(ev[i].code) << " "
                        << std::setw(8) << ev[i].value << std::endl;
              break;

            default:
              std::cout << std::setw(8) << ev[i].type << " "
                        << std::setw(8) << ev[i].code << " "
                        << std::setw(8) << ev[i].value << std::endl;
              break;
          }
        }
      }
    }
  }
}

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " DEVICE\n";
    return 1;
  }
  else
  {
    auto device = EvdevDevice::open(argv[1]);
    auto info = device->read_evdev_info();

    print_evdev_info(info);
    print_events(*device);

    return 0;
  }
}

/* EOF */
