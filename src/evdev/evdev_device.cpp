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

#include "evdev_device.hpp"

#include <map>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "util.hpp"
#include "util/bits.hpp"

namespace evtest_qt {

std::unique_ptr<EvdevDevice>
EvdevDevice::open(std::string const& filename)
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
    auto controller = util::make_unique<EvdevDevice>(fd, filename);
    //controller->read_evdev_info();
    //controller->start_qsocket_notifier();
    return controller;
  }
}

EvdevDevice::EvdevDevice(int fd, std::string const& filename) :
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

  int version = 0;
  if (ioctl(m_fd, EVIOCGVERSION, &version) < 0)
  {
    std::cout << m_filename << ": " << strerror(errno) << '\n';
  }

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

  std::string phys;
  {
    char c_name[1024] = "unknown";
    if (ioctl(m_fd, EVIOCGPHYS(sizeof(c_name)), c_name) < 0) {
      // only worth a warning, as uinput devices might not have this set
      std::cerr << m_filename << ": EVIOCGPHYS failed: " << strerror(errno) << std::endl;
    }
    phys = c_name;
  }

  if (false)
  { // this just fails a lot and doesn't seem to be of much use
    char c_name[256] = "unknown";
    if (ioctl(m_fd, EVIOCGUNIQ(sizeof(c_name)), c_name) < 0)
    {
      std::cout << m_filename << ": " << strerror(errno) << '\n';
    }
    else
    {
      std::cout << "uniq: '" << c_name << "'" << std::endl;
    }
  }

  {
    unsigned long propbits[INPUT_PROP_MAX];
    if (ioctl(m_fd, EVIOCGPROP(sizeof(propbits)), propbits) < 0)
    {
      std::ostringstream out;
      out << m_filename << ": " << strerror(errno);
      throw std::runtime_error(out.str());
    }
    else
    {
      //for (prop = 0; prop < INPUT_PROP_MAX; prop++) {
      //if (test_bit(prop, propbits))
      //                printf("  Property type %d (%s)\n", prop, propname(prop));
    }
  }

  struct input_id id;
  {
    if (ioctl(m_fd, EVIOCGID, &id) < 0)
    {
      std::ostringstream out;
      out << m_filename << ": " << strerror(errno);
      throw std::runtime_error(out.str());
    }
    else
    {
      // nothing to do
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

  std::map<uint16_t, AbsInfo> absinfos;
  for(uint16_t i = 0; i < ABS_MAX; ++i)
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
        absinfos[i] = AbsInfo(absinfo);
      }
    }
  }

  return EvdevInfo(version,
                   std::move(name),
                   std::move(phys),
                   id,
                   std::move(bit),
                   std::move(abs_bit),
                   std::move(rel_bit),
                   std::move(key_bit),
                   std::move(absinfos));
}

ssize_t
EvdevDevice::read_events(struct input_event* ev, size_t count)
{
  ssize_t rd = ::read(m_fd, ev, sizeof(struct input_event) * count);
  if (rd < EAGAIN)
  {
    return 0;
  }
  else if (rd < 0)
  {
    return rd;
  }
  else
  {
    return rd / static_cast<ssize_t>(sizeof(struct input_event));
  }
}

} // namespace evtest_qt

/* EOF */
