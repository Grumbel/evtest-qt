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

#include <QTimer>
#include <QMenuBar>

#include "util.hpp"
#include "widgets/evdev_widget.hpp"

namespace evtest_qt {

EvtestApp::EvtestApp(QApplication& app) :
  m_app(app),
  m_window(),
  m_widget(),
  m_action_exit("Exit"),
  m_action_verification_mode("Verification Mode"),
  m_vbox_layout(&m_widget),
  m_evdev_list_box(),
  m_ev_widget(),
  m_ev_widget_placeholder(),
  m_device(),
  m_state(),
  m_notifier()
{
  // setup actions
  connect(&m_action_exit, &QAction::triggered, [this](){
    std::cout << "Exit" << std::endl;
    m_app.quit();
  });

  m_action_verification_mode.setCheckable(true);
  m_action_verification_mode.setToolTip("Highlight buttons have been used");
  connect(&m_action_verification_mode, &QAction::toggled, [this](bool value){
    if (m_ev_widget) {
      m_ev_widget->set_verification_mode(value);
    }
  });

  // build GUI
  QMenuBar* menubar = m_window.menuBar();
  QMenu* file_menu = menubar->addMenu("&File");
  file_menu->addAction(&m_action_exit);


  QMenu* view_menu = menubar->addMenu("&View");
  view_menu->addAction(&m_action_verification_mode);

  //m_widget.setMinimumSize(400, 300);
  m_window.setCentralWidget(&m_widget);

  m_vbox_layout.addWidget(&m_evdev_list_box);

  m_ev_widget_placeholder = util::make_unique<QLabel>("nothing selected");
  m_ev_widget_placeholder->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  m_vbox_layout.addWidget(m_ev_widget_placeholder.get());

  QObject::connect(&m_evdev_list_box, SIGNAL(activated(int)),
                   this, SLOT(on_item_change(int)));

  m_window.show();
}

EvtestApp::~EvtestApp()
{
}

void
EvtestApp::refresh_device_list()
{
  m_evdev_list_box.clear();
  for(auto dev : EvdevList::scan("/dev/input"))
  {
    try
    {
      auto dev_fp = EvdevDevice::open(dev);
      auto info = dev_fp->read_evdev_info();

      std::ostringstream str;
      str << dev.substr(11) << ": " << info.name;
      m_evdev_list_box.addItem(str.str().c_str(), QString::fromStdString(dev));
    }
    catch(std::exception const& err)
    {
      std::cout << dev << ": " << err.what() << std::endl;

      std::ostringstream str;
      str << dev.substr(11) << ": " << "[error: " << err.what() << "]";
      m_evdev_list_box.addItem(str.str().c_str(), QString::fromStdString(dev));
    }
  }
}

void
EvtestApp::select_device(QString const& device)
{
  on_device_change(device.toStdString());
}

void
EvtestApp::on_item_change(int index)
{
  std::string dev = m_evdev_list_box.itemData(index).toString().toUtf8().data();
  std::cout << "activated: " << dev << std::endl;
  on_device_change(dev);
}

void
EvtestApp::on_data(EvdevDevice& device, EvdevState& state)
{
  std::array<struct input_event, 128> ev;
  while(true)
  {
    ssize_t num_events = device.read_events(ev.data(), ev.size());
    if (num_events < 0)
    {
      std::cout << "error: " << num_events << ": " << strerror(errno) << std::endl;
      return;
    }
    else if (num_events == 0)
    {
      return;
    }
    else
    {
      for(ssize_t i = 0; i < num_events; ++i)
      {
        state.update(ev[static_cast<size_t>(i)]);
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
EvtestApp::on_notification(int fd)
{
  on_data(*m_device, *m_state);
}

void
EvtestApp::on_device_change(std::string const& filename)
{
  m_notifier.reset();
  m_state.reset();
  //m_vbox_layout.removeWidget(m_ev_widget.get());
  m_ev_widget.reset();

  try
  {
    m_device = EvdevDevice::open(filename);
    auto info = m_device->read_evdev_info();

    m_state = util::make_unique<EvdevState>(info);

    m_ev_widget_placeholder.reset();
    m_ev_widget = util::make_unique<EvdevWidget>(*m_state, info);
    m_ev_widget->set_verification_mode(m_action_verification_mode.isChecked());
    m_vbox_layout.addWidget(m_ev_widget.get());

    m_notifier = util::make_unique<QSocketNotifier>(m_device->get_fd(), QSocketNotifier::Read);

    QObject::connect(m_notifier.get(), SIGNAL(activated(int)),
                     this, SLOT(on_notification(int)));

    QTimer::singleShot(0, this, SIGNAL(on_shrink_action()));
  }
  catch(std::exception const& err)
  {
    std::cout << filename << ": " << err.what() << std::endl;
    m_ev_widget.reset();
    m_ev_widget_placeholder = util::make_unique<QLabel>(err.what());
    m_vbox_layout.addWidget(m_ev_widget.get());
  }
}

} // namespace evtest_qt

/* EOF */
