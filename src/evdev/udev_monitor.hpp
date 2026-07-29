// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_UDEV_MONITOR_HPP
#define HEADER_EVTEST_QT_UDEV_MONITOR_HPP

#include <memory>
#include <string>

#include <QObject>
#include <QSocketNotifier>

struct udev;
struct udev_monitor;

namespace evtest_qt {

/** Watches the udev input subsystem for event-device add/remove.
    udev is the kernel userspace device manager; its netlink monitor is
    the standard way to learn about input hotplug (not a high-level
    D-Bus device API). */
class UdevInputMonitor : public QObject
{
  Q_OBJECT

public:
  explicit UdevInputMonitor(QObject* parent = nullptr);
  ~UdevInputMonitor() override;

  bool is_valid() const { return m_udev != nullptr && m_monitor != nullptr; }

signals:
  /** Emitted for /dev/input/event* nodes only. */
  void device_added(QString const& devnode, QString const& name);
  void device_removed(QString const& devnode, QString const& name);
  void monitor_error(QString const& message);

private slots:
  void on_udev_readable(int fd);

private:
  struct udev* m_udev;
  struct udev_monitor* m_monitor;
  std::unique_ptr<QSocketNotifier> m_notifier;

  UdevInputMonitor(UdevInputMonitor const&) = delete;
  UdevInputMonitor& operator=(UdevInputMonitor const&) = delete;
};

} // namespace evtest_qt

#endif

/* EOF */
