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

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

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
  EvdevInfo info;

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
      info.name = c_name;
    }
  }

  { // Read in how many btn/abs/rel the device has
    ioctl(m_fd, EVIOCGBIT(0, EV_MAX), info.bit.data());
    ioctl(m_fd, EVIOCGBIT(EV_ABS, ABS_MAX), info.abs_bit.data());
    ioctl(m_fd, EVIOCGBIT(EV_REL, REL_MAX), info.rel_bit.data());
    ioctl(m_fd, EVIOCGBIT(EV_KEY, KEY_MAX), info.key_bit.data());

    for(int i = 0; i < ABS_MAX; ++i)
    {
      if (test_bit(i, info.abs_bit))
      {
      }
    }

    for(int i = 0; i < REL_MAX; ++i)
    {
      if (test_bit(i, info.rel_bit))
      {
      }
    }

    for(int i = 0; i < KEY_MAX; ++i)
    {
      if (test_bit(i, info.key_bit))
      {
      }
    }
  }
  return info;
}

int
EvdevDevice::read_event(struct input_event& event)
{
  // read data
  struct input_event ev[128];
  int rd = 0;
  while((rd = ::read(m_fd, ev, sizeof(struct input_event) * 128)) > 0)
  {
    for (size_t i = 0; i < rd / sizeof(struct input_event); ++i)
    {
      if (ev[i].type == EV_SYN)
      {
        //submit_msg(m_msg, m_message_descriptor);
      }
      else
      {
        //parse(ev[i], m_msg);
      }
    }
  }

  return true;
}

/* EOF */
