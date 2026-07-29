// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "about_dialog.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace evtest_qt {

AboutDialog::AboutDialog(QWidget* parent) :
  QDialog(parent)
{
  setWindowTitle(QStringLiteral("About evtest-qt"));
  setAttribute(Qt::WA_DeleteOnClose);

  auto icon_label = std::make_unique<QLabel>();
  icon_label->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
  icon_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto text_label = std::make_unique<QLabel>();
  text_label->setText(
    QStringLiteral(
      "<center>"
      "<h1>evtest-qt</h1>"
      "<p>Version %1</p>"
      "<p>A graphical input device tester for Linux.</p>"
      "<p><a href=\"https://github.com/Grumbel/evtest-qt\">https://github.com/Grumbel/evtest-qt</a></p>"
      "<p>Copyright (C) 2015–2026 Ingo Ruhnke "
      "&lt;<a href=\"mailto:grumbel@gmail.com\">grumbel@gmail.com</a>&gt;</p>"
      "<p>Licensed under the GPLv3+</p>"
      "</center>"
      ).arg(QStringLiteral(EVTEST_QT_VERSION)));
  text_label->setTextFormat(Qt::RichText);
  text_label->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  text_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  text_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
  text_label->setOpenExternalLinks(true);

  auto button_box = std::make_unique<QDialogButtonBox>(QDialogButtonBox::Close);
  connect(button_box.get(), &QDialogButtonBox::rejected,
          this, &AboutDialog::reject);

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
