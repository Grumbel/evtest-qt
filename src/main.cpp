// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstdlib>
#include <cstring>
#include <print>
#include <string>
#include <vector>

#include <QIcon>

#include "evtest_app.hpp"

namespace evtest_qt {

void print_help()
{
  std::print(
    "Usage: evtest-qt [DEVICE]\n"
    "A graphical joystick tester\n"
    "\n"
    "   DEVICE  event device file to start with\n"
    "\n"
    "   -v, --version   Print version number\n"
    "   -h, --help      Print help\n"
    "\n"
    "Options:\n"
    "   --verification-mode\n"
    "                   Start with verification mode enabled\n"
  );
}

struct Options
{
  bool verification_mode = false;
  std::vector<std::string> rest = {};
};

Options parse_args(int argc, char** argv)
{
  Options opts;

  for (int i = 1; i < argc; ++i)
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
      std::println("evtest-qt {}", EVTEST_QT_VERSION);
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
