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

#include "evdev_device.hpp"

#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "bits.hpp"

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)

std::unique_ptr<EvdevDevice>
EvdevDevice::open(const std::string& filename)
{
  int fd = ::open(filename.c_str(), O_RDWR | O_NONBLOCK);
  if (fd < 0)
  {
    std::ostringstream out;
    out << filename << ": " << strerror(errno);
    throw std::runtime_error(out.str());
  }
  else
  {
    auto controller = std::make_unique<EvdevDevice>(fd, filename);
    controller->read_evdev_info();
    //controller->start_qsocket_notifier();
    return controller;
  }
}

EvdevDevice::EvdevDevice(int fd, const std::string& filename) :
  m_fd(fd),
  m_filename(filename)
{
}

EvdevDevice::~EvdevDevice()
{
  close(m_fd);
}

EvdevInfo
EvdevDevice::read_evdev_info()
{
  std::string name;

  { // Get the human readable name
    char c_name[1024] = "unknown";
    if (ioctl(m_fd, EVIOCGNAME(sizeof(c_name)), c_name) < 0)
    {
      std::ostringstream out;
      out << m_filename << ": " << strerror(errno);
      throw std::runtime_error(out.str());
    }
    else
    {
      name = c_name;
    }
  }

  std::array<unsigned long, bits::nbits(EV_MAX)> bit;
  std::array<unsigned long, bits::nbits(ABS_MAX)> abs_bit;
  std::array<unsigned long, bits::nbits(REL_MAX)> rel_bit;
  std::array<unsigned long, bits::nbits(KEY_MAX)> key_bit;

  { // Read in how many btn/abs/rel the device has
    ioctl(m_fd, EVIOCGBIT(0, EV_MAX), bit.data());
    ioctl(m_fd, EVIOCGBIT(EV_ABS, ABS_MAX), abs_bit.data());
    ioctl(m_fd, EVIOCGBIT(EV_REL, REL_MAX), rel_bit.data());
    ioctl(m_fd, EVIOCGBIT(EV_KEY, KEY_MAX), key_bit.data());
  }

  for(int i = 0; i < ABS_MAX; ++i)
  {
    if (bits::test_bit(i, abs_bit.data()))
    {
      struct input_absinfo absinfo;
      if (ioctl(m_fd, EVIOCGABS(i), &absinfo) < 0)
      {
        std::cout << "error\n";
      }
      else
      {
        std::cout << "absinfo: " << i
                  << " value:" << absinfo.value
                  << " min:" << absinfo.minimum
                  << " max:" <<  absinfo.maximum
                  << " fuzz:" <<  absinfo.fuzz
                  << " flat:" << absinfo.flat
                  << " res:" << absinfo.resolution << "\n";

      }
    }
  }

  return EvdevInfo(std::move(name),
                   std::move(bit),
                   std::move(abs_bit),
                   std::move(rel_bit),
                   std::move(key_bit));
}

ssize_t
EvdevDevice::read_events(struct input_event* ev, size_t count)
{
  // read data
  int rd = ::read(m_fd, ev, sizeof(struct input_event) * count);
  if (rd < 0)
  {
    return rd;
  }
  else
  {
    return rd / sizeof(struct input_event);
  }
}

/* EOF */
