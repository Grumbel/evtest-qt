// evtest-qt - A graphical joystick tester
// Copyright (C) 2022 Ingo Ruhnke <grumbel@gmail.com>
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

#include "about_dialog.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace evtest_qt {

AboutDialog::AboutDialog(QWidget* parent) :
  QDialog(parent)
{
  // widgets
  auto icon_label = std::make_unique<QLabel>();
  icon_label->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
  icon_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  // m_icon_label.setPixmap(QPixmap("icons/evtest-qt.svg")));

  auto text_label = std::make_unique<QLabel>();
  text_label->setText(
    "<center>"
    "<h1>evtest-qt</h1>"
    "<p>A graphical input device tester.</p>"
    "<p><a href=\"https://github.com/Grumbel/evtest-qt\">https://github.com/Grumbel/evtest-qt</a></p>"
    "<p>Copyright (C) 2015-2022\nIngo Ruhnke &lt;<a href=\"mail:grumbel@gmail.com\">grumbel@gmail.com</a>&gt;</p>"
    "<p>Licensed under the GPLv3+</p>"
    "</center>"
    );
  text_label->setTextFormat(Qt::RichText);
  text_label->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  text_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  text_label->setTextInteractionFlags(Qt::TextSelectableByMouse);

  auto button_box = std::make_unique<QDialogButtonBox>(QDialogButtonBox::Close);
  connect(button_box.get(), &QDialogButtonBox::rejected,
          this, &AboutDialog::reject);

  // layout
  auto vbox = std::make_unique<QVBoxLayout>();
  vbox->addStretch();
  vbox->addWidget(icon_label.release());
  vbox->addWidget(text_label.release());
  vbox->addStretch();
  vbox->addWidget(button_box.release());

  setLayout(vbox.release());
}

} // namespace evtest_qt

/* EOF */
