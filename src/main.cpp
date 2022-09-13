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
#include <string>

#include <QIcon>

#include "evtest_app.hpp"

namespace evtest_qt {

void print_help()
{
  std::cout << "Usage: evtest-qt [DEVICE]\n"
            << "A graphical joystick tester\n"
            << "\n"
            << "   DEVICE  event device file to start with\n"
            << "\n"
            << "   -v, --version   Print version number\n"
            << "   -h, --help      Print help\n"
            << "\n"
            << "Options:\n"
            << "   --verification-mode\n"
            << "                   Start with verification mode enabled\n"
            << std::endl;
}

struct Options
{
  bool verification_mode = false;
  std::vector<std::string> rest = {};
};

Options parse_args(int argc, char** argv)
{
  Options opts;

  for(int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "-h") == 0 ||
        strcmp(argv[i], "--help") == 0)
    {
      print_help();
      exit(EXIT_SUCCESS);
    }
    else if (strcmp(argv[i], "-v") == 0 ||
             strcmp(argv[i], "--version") == 0)
    {
      std::cout << "evtest-qt " << EVTEST_QT_VERSION << std::endl;
      exit(EXIT_SUCCESS);
    }
    else if (strcmp(argv[i], "--verification-mode") == 0)
    {
      opts.verification_mode = true;
    }
    else
    {
      if (!opts.rest.empty())
      {
        print_help();
        exit(EXIT_FAILURE);
      }
      else
      {
        opts.rest.emplace_back(argv[i]);
      }
    }
  }

  return opts;
}

} // namespace evtest_qt

int main(int argc, char** argv)
{
  using namespace evtest_qt;

  Options const opts = parse_args(argc, argv);

  QApplication app(argc, argv);

  app.setApplicationName("evtest-qt");
  app.setApplicationVersion(EVTEST_QT_VERSION);
  app.setWindowIcon(QIcon::fromTheme("evtest-qt"));

  evtest_qt::EvtestApp evtest(app);
  evtest.set_verification_mode(opts.verification_mode);
  evtest.refresh_device_list();

  if (!opts.rest.empty()) {
    evtest.select_device(QString::fromStdString(opts.rest[0]));
  }

  return app.exec();
}

/* EOF */
