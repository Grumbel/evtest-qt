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

//#include <QCommandLineParser>

#include "evtest_app.hpp"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  QApplication::setApplicationName("evtest-qt");
  QApplication::setApplicationVersion("0.1");

#if 0
  QCommandLineParser parser;
  parser.setApplicationDescription("A graphical joystick tester");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("DEVICE", "event device file to start with");

  parser.process(app);

  const QStringList args = parser.positionalArguments();

  EvtestApp evtest;
  evtest.refresh_device_list();

  if (!args.empty())
  {
    evtest.select_device(args[0]);
  }
#endif

  return app.exec();
}

/* EOF */
