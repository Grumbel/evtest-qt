// jstest-qt - A graphical joystick tester
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

    std::cout << "name: " << info.name << std::endl;
    std::cout << "num_abs: " << info.num_abs << std::endl;
    std::cout << "num_rel: " << info.num_rel << std::endl;
    std::cout << "num_key: " << info.num_key << std::endl;

    std::cout << "abs:\n";
    for(size_t i = 0; i < info.abss.size(); ++i)
    {
      std::cout << "  " << info.abss[i] << "\n";
    }
    std::cout << "\n";

    std::cout << "rel:\n";
    for(size_t i = 0; i < info.rels.size(); ++i)
    {
      std::cout << "  " << info.rels[i] << "\n";
    }
    std::cout << "\n";

    std::cout << "key:\n";
    for(size_t i = 0; i < info.keys.size(); ++i)
    {
      std::cout << "  " << info.keys[i] << "\n";
    }
    std::cout << "\n";

    std::cout << "reading events..." << std::endl;
    std::array<struct input_event, 128> ev;
    while(true)
    {
      ssize_t num_events = device->read_events(ev.data(), ev.size());
      if (num_events < 0)
      {
        std::cout << "error: " << num_events << ": " << strerror(errno) << std::endl;
        break;
      }
      else
      {
        for(ssize_t i = 0; i < num_events; ++i)
        {
          std::cout << std::setw(8) << ev[i].type << " "
                    << std::setw(8) << ev[i].code << " "
                    << std::setw(8) << ev[i].value << std::endl;
        }
      }
    }

    return 0;
  }
}

/* EOF */
