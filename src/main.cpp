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

#include "evtest_app.hpp"

void print_help()
{
  std::cout << "Usage: evtest-qt [DEVICE]\n"
            << "A graphical joystick tester\n"
            << "\n"
            << "   DEVICE  event device file to start with\n"
            << "\n"
            << "   -h, --help   Print help\n";
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  QApplication::setApplicationName("evtest-qt");
  QApplication::setApplicationVersion("0.1");

  std::vector<QString> args;

  for(int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "-h") == 0 ||
        strcmp(argv[i], "--help") == 0)
    {
      print_help();
      return 0;
    }
    else
    {
      if (!args.empty())
      {
        print_help();
        return 1;
      }
      else
      {
        args.push_back(argv[i]);
      }
    }
  }

  EvtestApp evtest;
  evtest.refresh_device_list();

  if (!args.empty())
  {
    evtest.select_device(args[0]);
  }

  return app.exec();
}

/* EOF */
