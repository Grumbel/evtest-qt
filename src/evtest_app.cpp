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

#include "evtest_app.hpp"

#include <QDateTime>
#include <QMenuBar>
#include <QTimer>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>

#include "widgets/about_dialog.hpp"
#include "widgets/evdev_widget.hpp"

namespace evtest_qt {

EvtestApp::EvtestApp(QApplication& app) :
  m_app(app),
  m_window(),
  m_widget(),
  m_action_exit("Exit"),
  m_action_show_about("About"),
  m_action_verification_mode("Verification Mode"),
  m_vbox_layout(&m_widget),
  m_evdev_list_box(),
  m_device_log(),
  m_ev_widget(),
  m_ev_widget_placeholder(),
  m_device(),
  m_state(),
  m_notifier(),
  m_udev_monitor(),
  m_refresh_timer(),
  m_current_device()
{
  connect(&m_action_exit, &QAction::triggered, [this](){
    m_app.quit();
  });

  m_action_show_about.setToolTip("Show About dialog");
  connect(&m_action_show_about, &QAction::triggered, [this](bool){
    auto about_dialog = std::make_unique<AboutDialog>(&m_window);
    about_dialog->show();
    about_dialog.release();
  });

  m_action_verification_mode.setCheckable(true);
  m_action_verification_mode.setToolTip("Highlight buttons have been used");
  connect(&m_action_verification_mode, &QAction::toggled, [this](bool value){
    if (m_ev_widget) {
      m_ev_widget->set_verification_mode(value);
    }
  });

  QMenuBar* menubar = m_window.menuBar();
  QMenu* file_menu = menubar->addMenu("&File");
  file_menu->addAction(&m_action_exit);

  QMenu* view_menu = menubar->addMenu("&View");
  view_menu->addAction(&m_action_verification_mode);

  QMenu* help_menu = menubar->addMenu("&Help");
  help_menu->addAction(&m_action_show_about);

  m_window.setCentralWidget(&m_widget);

  m_vbox_layout.addWidget(&m_evdev_list_box);

  m_ev_widget_placeholder = std::make_unique<QLabel>("nothing selected");
  m_ev_widget_placeholder->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  m_vbox_layout.addWidget(m_ev_widget_placeholder.get(), /*stretch=*/1);

  m_device_log.setReadOnly(true);
  m_device_log.setOpenExternalLinks(false);
  m_device_log.setOpenLinks(false);
  m_device_log.document()->setMaximumBlockCount(500);
  m_device_log.setFixedHeight(120);
  m_device_log.setStyleSheet(QStringLiteral(
    "QTextBrowser { font-family: monospace; }"
    "QTextBrowser a { text-decoration: none; }"
  ));
  m_vbox_layout.addWidget(&m_device_log);
  connect(&m_device_log, SIGNAL(anchorClicked(QUrl)),
          this, SLOT(on_log_anchor_clicked(QUrl)));

  QObject::connect(&m_evdev_list_box, SIGNAL(activated(int)),
                   this, SLOT(on_item_change(int)));

  m_refresh_timer.setSingleShot(true);
  m_refresh_timer.setInterval(150);
  connect(&m_refresh_timer, SIGNAL(timeout()), this, SLOT(on_refresh_timeout()));

  m_udev_monitor = std::make_unique<UdevInputMonitor>(this);
  connect(m_udev_monitor.get(), SIGNAL(device_added(QString,QString)),
          this, SLOT(on_udev_device_added(QString,QString)));
  connect(m_udev_monitor.get(), SIGNAL(device_removed(QString,QString)),
          this, SLOT(on_udev_device_removed(QString,QString)));
  connect(m_udev_monitor.get(), SIGNAL(monitor_error(QString)),
          this, SLOT(on_udev_monitor_error(QString)));

  append_log(QStringLiteral("evtest-qt %1")
             .arg(QStringLiteral(EVTEST_QT_VERSION)));
  if (m_udev_monitor->is_valid()) {
    append_log(QStringLiteral("listening for input device add/remove (udev)"));
  } else {
    append_log(QStringLiteral("warning: udev monitor unavailable; list will not auto-update"));
  }

  m_window.show();
}

EvtestApp::~EvtestApp()
{
}

QString
EvtestApp::device_href(QString const& devnode, QString const& label)
{
  // device:/dev/input/eventN — handled in on_log_anchor_clicked
  return QStringLiteral("<a href=\"device:%1\">%2</a>")
    .arg(devnode.toHtmlEscaped(), label.toHtmlEscaped());
}

void
EvtestApp::append_log(QString const& html_line)
{
  QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss"));
  m_device_log.append(QStringLiteral("%1 %2").arg(stamp, html_line));
}

void
EvtestApp::on_log_anchor_clicked(QUrl const& link)
{
  if (link.scheme() != QLatin1String("device")) {
    return;
  }
  // QUrl("device:/dev/input/event0") -> path() == "/dev/input/event0"
  QString path = link.path();
  if (path.isEmpty()) {
    return;
  }
  select_device(path);
}

void
EvtestApp::schedule_device_list_refresh()
{
  // Coalesce bursts of udev events (e.g. one stick creates several nodes)
  m_refresh_timer.start();
}

void
EvtestApp::on_refresh_timeout()
{
  refresh_device_list();
}

void
EvtestApp::on_udev_device_added(QString const& devnode, QString const& name)
{
  QString base = devnode.section(QLatin1Char('/'), -1);
  QString link = device_href(devnode, base);
  if (name.isEmpty()) {
    append_log(QStringLiteral("add    %1").arg(link));
  } else {
    append_log(QStringLiteral("add    %1  \"%2\"").arg(link, name.toHtmlEscaped()));
  }
  schedule_device_list_refresh();
}

void
EvtestApp::on_udev_device_removed(QString const& devnode, QString const& name)
{
  QString base = devnode.section(QLatin1Char('/'), -1);
  QString link = device_href(devnode, base);
  if (name.isEmpty()) {
    append_log(QStringLiteral("remove %1").arg(link));
  } else {
    append_log(QStringLiteral("remove %1  \"%2\"").arg(link, name.toHtmlEscaped()));
  }

  if (m_current_device == devnode) {
    append_log(QStringLiteral("current device disconnected; closing view"));
    clear_device_view(QStringLiteral("device disconnected: %1").arg(base));
  }

  schedule_device_list_refresh();
}

void
EvtestApp::on_udev_monitor_error(QString const& message)
{
  append_log(QStringLiteral("error: %1").arg(message.toHtmlEscaped()));
}

void
EvtestApp::clear_device_view(QString const& message)
{
  m_notifier.reset();
  m_state.reset();
  m_device.reset();
  m_ev_widget.reset();
  m_current_device.clear();

  m_ev_widget_placeholder = std::make_unique<QLabel>(message);
  m_ev_widget_placeholder->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  m_vbox_layout.insertWidget(1, m_ev_widget_placeholder.get(), /*stretch=*/1);
}

void
EvtestApp::refresh_device_list()
{
  QString previous = m_current_device;
  if (previous.isEmpty() && m_evdev_list_box.currentIndex() >= 0) {
    previous = m_evdev_list_box.currentData().toString();
  }

  m_evdev_list_box.blockSignals(true);
  m_evdev_list_box.clear();

  int select_index = -1;
  for (auto const& dev : EvdevList::scan("/dev/input"))
  {
    try
    {
      auto dev_fp = EvdevDevice::open(dev);
      auto info = dev_fp->read_evdev_info();

      std::ostringstream str;
      str << dev.substr(11) << ": " << info.name;
      m_evdev_list_box.addItem(QString::fromStdString(str.str()),
                               QString::fromStdString(dev));
    }
    catch (std::exception const& err)
    {
      std::ostringstream str;
      str << dev.substr(11) << ": " << "[error: " << err.what() << "]";
      m_evdev_list_box.addItem(QString::fromStdString(str.str()),
                               QString::fromStdString(dev));
    }

    if (!previous.isEmpty() && previous == QString::fromStdString(dev)) {
      select_index = m_evdev_list_box.count() - 1;
    }
  }

  if (select_index >= 0) {
    m_evdev_list_box.setCurrentIndex(select_index);
  } else if (!previous.isEmpty() && previous == m_current_device) {
    clear_device_view(QStringLiteral("device disconnected"));
  }

  m_evdev_list_box.blockSignals(false);
}

void
EvtestApp::select_device(QString const& device)
{
  on_device_change(device.toStdString());

  for (int i = 0; i < m_evdev_list_box.count(); ++i) {
    if (m_evdev_list_box.itemData(i).toString() == device) {
      m_evdev_list_box.setCurrentIndex(i);
      break;
    }
  }
}

void
EvtestApp::set_verification_mode(bool value)
{
  m_action_verification_mode.setChecked(value);
}

void
EvtestApp::on_item_change(int index)
{
  std::string dev = m_evdev_list_box.itemData(index).toString().toUtf8().data();
  on_device_change(dev);
}

void
EvtestApp::on_data(EvdevDevice& device, EvdevState& state)
{
  std::array<struct input_event, 128> ev;
  while (true)
  {
    ssize_t num_events = device.read_events(ev.data(), ev.size());
    if (num_events < 0)
    {
      append_log(QStringLiteral("read error on %1: %2")
                 .arg(device_href(m_current_device,
                                  m_current_device.section(QLatin1Char('/'), -1)),
                      QString::fromLocal8Bit(strerror(errno)).toHtmlEscaped()));
      m_notifier.reset();
      return;
    }
    else if (num_events == 0)
    {
      return;
    }
    else
    {
      for (ssize_t i = 0; i < num_events; ++i)
      {
        if (state.update(ev[static_cast<size_t>(i)])) {
          if (m_ev_widget) {
            m_ev_widget->set_state(state);
          }
        }
      }
    }
  }
}

void
EvtestApp::on_shrink_action()
{
  m_window.resize(0, 0);
}

void
EvtestApp::on_notification(int)
{
  if (m_device && m_state) {
    on_data(*m_device, *m_state);
  }
}

void
EvtestApp::on_device_change(std::string const& filename)
{
  m_notifier.reset();
  m_state.reset();
  m_device.reset();
  m_ev_widget.reset();
  m_ev_widget_placeholder.reset();
  m_current_device.clear();

  try
  {
    m_device = EvdevDevice::open(filename);
    auto info = m_device->read_evdev_info();

    m_state = std::make_unique<EvdevState>(info);

    m_ev_widget = std::make_unique<EvdevWidget>(info);
    m_ev_widget->set_verification_mode(m_action_verification_mode.isChecked());
    m_vbox_layout.insertWidget(1, m_ev_widget.get(), /*stretch=*/1);

    m_notifier = std::make_unique<QSocketNotifier>(m_device->get_fd(), QSocketNotifier::Read);

    QObject::connect(m_notifier.get(), SIGNAL(activated(int)),
                     this, SLOT(on_notification(int)));

    m_current_device = QString::fromStdString(filename);
    append_log(QStringLiteral("open   %1  \"%2\"")
               .arg(device_href(m_current_device,
                                m_current_device.section(QLatin1Char('/'), -1)),
                    QString::fromStdString(info.name).toHtmlEscaped()));

    QTimer::singleShot(0, this, SLOT(on_shrink_action()));
  }
  catch (std::exception const& err)
  {
    QString path = QString::fromStdString(filename);
    append_log(QStringLiteral("open failed %1: %2")
               .arg(device_href(path, path.section(QLatin1Char('/'), -1)),
                    QString::fromLocal8Bit(err.what()).toHtmlEscaped()));
    m_ev_widget_placeholder = std::make_unique<QLabel>(QString::fromLocal8Bit(err.what()));
    m_ev_widget_placeholder->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    m_vbox_layout.insertWidget(1, m_ev_widget_placeholder.get(), /*stretch=*/1);
  }
}

} // namespace evtest_qt

/* EOF */
