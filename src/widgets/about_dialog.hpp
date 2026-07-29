// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_ABOUT_DIALOG_HPP
#define HEADER_EVTEST_QT_ABOUT_DIALOG_HPP

#include <QDialog>

namespace evtest_qt {

class AboutDialog : public QDialog
{
public:
  AboutDialog(QWidget* parent);

public:
  AboutDialog(AboutDialog const&) = delete;
  AboutDialog& operator=(AboutDialog const&) = delete;
};

} // namespace evtest_qt

#endif

/* EOF */
