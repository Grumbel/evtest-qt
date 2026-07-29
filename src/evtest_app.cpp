// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "evtest_app.hpp"

#include <QDateTime>
#include <QFileInfo>
#include <QMenuBar>
#include <QTimer>

#include <cerrno>
#include <cstring>
#include <format>
#include <array>

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
  m_action_hide_event_log("Hide Event Log"),
  m_action_event_graph("Event Graph..."),
  m_vbox_layout(&m_widget),
  m_evdev_list_box(),
  m_device_log(),
  m_ev_widget(),
  m_ev_widget_placeholder(),
  m_device(),
  m_state(),
  m_notifier(),
  m_udev_monitor(),
  m_graph_window(),
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

  m_action_hide_event_log.setCheckable(true);
  m_action_hide_event_log.setChecked(false);
  m_action_hide_event_log.setToolTip("Hide the input device event log");
  connect(&m_action_hide_event_log, &QAction::toggled, [this](bool hide){
    m_device_log.setVisible(!hide);
  });

  m_action_event_graph.setToolTip("Open a time-series plot of axes and buttons");
  connect(&m_action_event_graph, &QAction::triggered, this, &EvtestApp::on_show_event_graph);

  QMenuBar* menubar = m_window.menuBar();
  QMenu* file_menu = menubar->addMenu("&File");
  file_menu->addAction(&m_action_exit);

  QMenu* view_menu = menubar->addMenu("&View");
  view_menu->addAction(&m_action_verification_mode);
  view_menu->addAction(&m_action_hide_event_log);
  view_menu->addSeparator();
  view_menu->addAction(&m_action_event_graph);

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
EvtestApp::on_show_event_graph()
{
  if (!m_graph_window) {
    m_graph_window = std::make_unique<EventGraphWindow>();
  }
  if (m_device && m_state) {
    m_graph_window->set_device(m_state->get_info());
    m_graph_window->set_state_source(m_state.get());
  } else {
    m_graph_window->clear_device();
    m_graph_window->set_state_source(nullptr);
  }
  m_graph_window->show();
  m_graph_window->raise();
  m_graph_window->activateWindow();
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
  if (m_graph_window) {
    m_graph_window->set_state_source(nullptr);
    m_graph_window->clear_device();
  }

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

      m_evdev_list_box.addItem(
        QString::fromStdString(std::format("{}: {}", dev.substr(11), info.name)),
        QString::fromStdString(dev));
    }
    catch (std::exception const& err)
    {
      m_evdev_list_box.addItem(
        QString::fromStdString(std::format("{}: [error: {}]", dev.substr(11), err.what())),
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

QString
EvtestApp::normalize_device_path(QString const& arg)
{
  QString path = arg.trimmed();
  if (path.isEmpty()) {
    return path;
  }

  // Accept "event12" or "input/event12" as shorthand for /dev/input/event12
  if (!path.startsWith(QLatin1Char('/'))) {
    if (path.startsWith(QStringLiteral("input/"))) {
      path = QStringLiteral("/dev/") + path;
    } else if (path.startsWith(QStringLiteral("event"))) {
      path = QStringLiteral("/dev/input/") + path;
    }
  }

  QFileInfo fi(path);
  if (fi.exists()) {
    // Prefer absolute path; canonical may be empty for some special nodes
    path = fi.absoluteFilePath();
  }
  return path;
}

void
EvtestApp::select_device(QString const& device)
{
  QString path = normalize_device_path(device);
  on_device_change(path.toStdString());

  for (int i = 0; i < m_evdev_list_box.count(); ++i) {
    QString item_path = m_evdev_list_box.itemData(i).toString();
    if (item_path == path
        || QFileInfo(item_path).fileName() == QFileInfo(path).fileName()) {
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
  QString dev = m_evdev_list_box.itemData(index).toString();
  if (!dev.isEmpty()) {
    on_device_change(dev.toStdString());
  }
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
          if (m_graph_window) {
            m_graph_window->push_state(state);
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QObject::connect(m_notifier.get(), &QSocketNotifier::activated,
                     this, [this](QSocketDescriptor) { on_notification(0); });
#elif QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    QObject::connect(m_notifier.get(),
                     QOverload<QSocketDescriptor>::of(&QSocketNotifier::activated),
                     this, [this](QSocketDescriptor) { on_notification(0); });
#else
    QObject::connect(m_notifier.get(),
                     static_cast<void (QSocketNotifier::*)(int)>(&QSocketNotifier::activated),
                     this, SLOT(on_notification(int)));
#endif

    m_current_device = QString::fromStdString(filename);
    if (m_graph_window) {
      m_graph_window->set_device(info);
      m_graph_window->set_state_source(m_state.get());
    }
    append_log(QStringLiteral("open   %1  \"%2\"")
               .arg(device_href(m_current_device,
                                m_current_device.section(QLatin1Char('/'), -1)),
                    QString::fromStdString(info.name).toHtmlEscaped()));

    QTimer::singleShot(0, this, &EvtestApp::on_shrink_action);
  }
  catch (std::exception const& err)
  {
    QString path = QString::fromStdString(filename);
    append_log(QStringLiteral("open failed %1: %2")
               .arg(device_href(path, path.section(QLatin1Char('/'), -1)),
                    QString::fromUtf8(err.what()).toHtmlEscaped()));
    m_ev_widget_placeholder = std::make_unique<QLabel>(QString::fromUtf8(err.what()));
    m_ev_widget_placeholder->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    m_vbox_layout.insertWidget(1, m_ev_widget_placeholder.get(), /*stretch=*/1);
  }
}

} // namespace evtest_qt

/* EOF */
