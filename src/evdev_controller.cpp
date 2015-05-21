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

#include "evdev_controller.hpp"

#include <boost/format.hpp>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>

void
EvdevController::start_qsocket_notifier()
{
  /*
  m_notifier = std::make_unique<QSocketNotifier>(m_fd, QSocketNotifier::Read);
  connect(&notifier, &QSocketNotifier::activated,
          [](int fd){
            on_data();
          });
  */
}

EvdevController::EvdevController(int fd,
                                 const std::string& filename,
                                 bool grab,
                                 bool debug) :
  m_fd(fd),
  m_filename(filename),
  m_name(),
  m_grab(grab),
  m_debug(debug),
  m_absinfo(ABS_MAX)
{
}

EvdevController::~EvdevController()
{
}

#if 0
bool
EvdevController::parse(const struct input_event& ev, ControllerMessage& msg_inout) const
{
  // struct input_event {
  //   struct timeval time;
  //   __u16 type;
  //   __u16 code;
  //   __s32 value;
  // };

  if (m_debug)
  {
    switch(ev.type)
    {
      case EV_KEY:
        std::cout << "EV_KEY " << key2str(ev.code) << " " << ev.value << std::endl;
        break;

      case EV_REL:
        std::cout << "EV_REL " << rel2str(ev.code) << " " << ev.value << std::endl;
        break;

      case EV_ABS:
        std::cout << "EV_ABS " << abs2str(ev.code) << " " << ev.value << std::endl;
        break;

      case EV_SYN:
        std::cout << "------------------- sync -------------------" << std::endl;
        break;

      case EV_MSC:
        // FIXME: no idea what those are good for, but they pop up
        // after key presses (something with scancodes maybe?!)
        break;

      default:
        log_info("unknown: " << ev.type << " " << ev.code << " " << ev.value);
        break;
    }
  }

  switch(ev.type)
  {
    case EV_KEY:
      {
        EvMap::const_iterator it = m_keymap.find(ev.code);
        if (it != m_keymap.end())
        {
          msg_inout.set_key(it->second, static_cast<bool>(ev.value));
          return true;
        }
        else
        {
          return false;
        }
      }
      break;

    case EV_ABS:
      {
        EvMap::const_iterator it = m_absmap.find(ev.code);
        if (it != m_absmap.end())
        {
          const struct input_absinfo& absinfo = m_absinfo[ev.code];
          msg_inout.set_abs(it->second, ev.value, absinfo.minimum, absinfo.maximum);
          return true;
        }
        else
        {
          return false;
        }
        break;
      }

    case EV_REL:
      {
        EvMap::const_iterator it = m_relmap.find(ev.code);
        if (it != m_relmap.end())
        {
          msg_inout.set_rel(it->second, ev.value);
          return true;
        }
        else
        {
          return false;
        }
      }
      break;

    default:
      // not supported event
      return false;
      break;
  }
}
#endif

void
EvdevController::on_data()
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
}

/* EOF */
