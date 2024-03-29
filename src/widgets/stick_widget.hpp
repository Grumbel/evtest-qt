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

#ifndef HEADER_EVTEST_QT_STICK_WIDGET_HPP
#define HEADER_EVTEST_QT_STICK_WIDGET_HPP

namespace evtest_qt {

class StickWidget
{
private:
public:
  StickWidget();
  virtual ~StickWidget();

private:
  StickWidget(StickWidget const&) = delete;
  StickWidget& operator=(StickWidget const&) = delete;
};

} // namespace evtest_qt

#endif

/* EOF */
