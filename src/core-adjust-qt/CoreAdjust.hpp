/*
 * This file is part of 'Core Adjust'.
 *
 * Core Adjust - Adjust various settings of Intel Processors.
 * Copyright (C) 2020, Alexander Bruines <alexander.bruines@gmail.com>
 *
 * Core Adjust is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Core Adjust is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Core Adjust. If not, see <https://www.gnu.org/licenses/>.
 */

/**
  * @file src/core-adjust-qt/CoreAdjust.hpp
  * @brief Central QWidget of the Core Adjust GUI application.
  *
  * @file src/core-adjust-qt/CoreAdjust.cpp
  * @brief Central QWidget of the Core Adjust GUI application (implementation).
  */
#ifndef CoreAdjust_CentralWidget
#define CoreAdjust_CentralWidget

// STL
#include <utility>
// Qt
#include <QCheckBox>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
// App
#include "CpuNumber.hpp"
#include "Monitor.hpp"
#include "ShellCommand.hpp"
#include "TabMember.hpp"

/**
  * @class CoreAdjust
  * @brief Central QWidget of the application.
  *
  * This widget is the application central widget.
  * It holds all the TabMemberWidget instances that form the user-interface and
  * coordinates applying and saving the new processor settings.
  */
class CoreAdjust : public QWidget {
  Q_OBJECT
  public:

    explicit CoreAdjust(
        CpuId&,
        CpuInfo&,
        TabValues&,
        TabSettings&,
        QWidget* parent = nullptr
    );
    ~CoreAdjust() override;

    /** @brief Load the values for all TabMemberWidget instances from the TabSettings instance. */
    void load();
    /** @brief Store the values for all TabMemberWidget instances to the TabSettings instance. */
    void store();
    /** @brief Captures QEvent::Close events.
      *
      * This class is set as eventFilter by the MainWindow instance.
      * This method is used to intercept the event triggered when the user
      * clicks on the 'close' button in the application titlebar. */
    bool eventFilter(QObject*, QEvent*) override;

  private:
    struct TabMemberDescription;
    static const std::vector<TabMemberDescription> TabMemberFactory;

    struct TabMemberAllCpusDescription;
    static const std::vector<TabMemberAllCpusDescription> TabMemberAllCpusFactory;

    std::reference_wrapper<CpuId> cpuId_;
    std::reference_wrapper<CpuInfo> cpuInfo_;
    std::reference_wrapper<TabValues> tabValues_;
    std::reference_wrapper<TabSettings> tabSettings_;

    inline CpuId& cpuId() { return cpuId_; }
    inline CpuInfo& cpuInfo() { return cpuInfo_; }
    inline TabValues& tabValues() { return tabValues_; }
    inline TabSettings& tabSettings() { return tabSettings_; }

    /** @brief This vector contains information about all TabMemberWidget in the application.
      *
      * Each entry of the vector is a std::tuple<...> where:
      * | element | type |
      * | :-: | :- |
      * |  0  | The QTabWidget* containing the TabMemberWidget* in tuple element 1 or NULL if the TabMemberWidget* in tuple element 1 belongs to tabWidget_. |
      * |  1  | Vector of TabMemberWidget* assigned to tuple element 0. If tuple element 0 is NULL then this vector always has a size of 1. |
      * |  2  | The currently active tab of tuple element 0. |
      */
    std::vector<
        std::tuple<
            QTabWidget*,
            std::vector<TabMemberWidget*>,
            unsigned short>> vTabMemberWidget_;

    /** @brief The currently active tab of tabWidget_ */
    unsigned short tabMemberIdx_ { 0 };

    /** @brief This QTabWidget contains all the processor tabs +
      * all tabs that are not for a specific processor. */
    QTabWidget* tabwidget_;

    QCheckBox* chkboxSaveOnExit_;
    QCheckBox* chkboxApplyOnEvent_;
    QPushButton* btnApply_;
    QPushButton* btnExit_;
    QVBoxLayout* monitorLayout_;
    Monitor* monitor_;
    MonitorTab* monitor_tab_;
    QTimer *timer_;

    bool ctor_apply_;

    ShellCommand shell_;

    void handleCloseEvent();
    void addProcessorTab(const PhysCpuNr&);

  private slots:
    void cbApplyOnSysEvent(int);
    void apply();
    void btnExit();
    void valueChanged(TabMemberWidget*);
    void tabSwitch(int);
    void processorTabSwitch(int);
    void timerCallback();
    void applyAfterShowEvent();
};

struct CoreAdjust::TabMemberDescription {
  /** @brief Name of the TabMemberWidget. */
  const char* name;
  /** @brief Pointer to function that creates the TabMemberWidget. */
  TabMemberWidget* (*construct) (
      const SingleCpuInfo&,
      const SingleCpuId&,
      TabMemberValues&,
      TabMemberSettings&,
      QWidget*
  );
  /** @brief Create a new TabMemberWidget.
    * @returns The TabMemberWidget. */
  template<typename T>
  static TabMemberWidget* Construct(
      const SingleCpuInfo& cpu_info,
      const SingleCpuId& cpu_id,
      TabMemberValues& cpu_values,
      TabMemberSettings& cpu_settings,
      QWidget* parent) {
    return new T(cpu_info, cpu_id, cpu_values, cpu_settings, parent);
  }
};

struct CoreAdjust::TabMemberAllCpusDescription {
  /** @brief Name of the TabMemberWidget. */
  const char* name;
  /** @brief Pointer to function that creates the TabMemberWidget. */
  TabMemberWidget* (*construct) (
      const CpuInfo&,
      const CpuId&,
      TabValues&,
      TabSettings&,
      QWidget*
  );
  /** @brief Create a new TabMemberWidget.
    * @returns The TabMemberWidget. */
  template<typename T>
  static TabMemberWidget* Construct(
      const CpuInfo& cpu_info,
      const CpuId& cpu_id,
      TabValues& cpu_values,
      TabSettings& cpu_settings,
      QWidget* parent) {
    return new T(cpu_info, cpu_id, cpu_values, cpu_settings, parent);
  }
};

#endif

