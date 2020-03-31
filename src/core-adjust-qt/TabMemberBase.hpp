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

#ifndef CoreAdjust_TabMemberBase
#define CoreAdjust_TabMemberBase

#include <functional>
#include <sstream>
#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <QSize>
#include <QSettings>
#include <QVBoxLayout>
#include "CpuInfo.hpp"
#include "CpuId.hpp"

class CommonSettings;
class TabMemberSettings;
class TabMemberValues;
class TabSettings;
class TabValues;

class TabMemberWidget : public QWidget {
  Q_OBJECT
  protected:
    explicit TabMemberWidget(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual ~TabMemberWidget() = default;
  public:
    class Settings;
    virtual void load() = 0;
    virtual void store() = 0;
    virtual void refresh() = 0;
    virtual bool read(std::ostringstream& err) = 0;
    virtual bool apply() = 0;
    virtual bool compare() = 0;
    virtual void timed(bool is_current_tab) = 0;
  signals:
    void valueChanged(TabMemberWidget*);
};

class TabMemberWidget::Settings {
  public:
    virtual void load(QSettings&, const TabMemberValues&) = 0;
    virtual void save(QSettings&) = 0;

  protected:
    Settings() = default;
    virtual ~Settings() = default;
    /* The actual methods are (re)implemented by class TabMemberSettings (virtual inheritance) */
    virtual TabMemberSettings& tabMemberSettings();
    virtual CommonSettings& commonSettings();
    virtual const SingleCpuInfo& cpuInfo() const;
};

/* Template for TabMemberWidget implementations that are meant for a single processor. */
class TabMemberTemplate : public TabMemberWidget {
  Q_OBJECT
  protected:
    explicit TabMemberTemplate(
      const SingleCpuInfo& cpuInfo, const SingleCpuId& cpuId,
      TabMemberValues& tabValues, TabMemberSettings& tabSettings,
      QWidget* parent, bool have_scroll_widget = true);

    inline const SingleCpuInfo& cpuInfo() { return cpuInfo_; }
    inline const SingleCpuId& cpuId() { return cpuId_; }
    inline TabMemberValues& tabValues() { return tabValues_; }
    inline TabMemberSettings& tabSettings() { return tabSettings_; }

    ~TabMemberTemplate() override = default;

    QVBoxLayout* scroll_layout_;
    QScrollArea* scroll_area_;
    QWidget* scroll_widget_;
    bool have_scroll_widget_;

  private:
    std::reference_wrapper<const SingleCpuInfo> cpuInfo_;
    std::reference_wrapper<const SingleCpuId> cpuId_;
    std::reference_wrapper<TabMemberValues> tabValues_;
    std::reference_wrapper<TabMemberSettings> tabSettings_;

  public:
    QSize	sizeHint() const override;
};

/* Template for TabMemberWidget implementations that are meant for all processor. */
class TabMemberTemplateAllCpus : public TabMemberWidget {
  Q_OBJECT
  protected:
    explicit TabMemberTemplateAllCpus(
      const CpuInfo& cpuInfo, const CpuId& cpuId,
      TabValues& tabValues, TabSettings& tabSettings,
      QWidget* parent, bool have_scroll_widget = true);

    ~TabMemberTemplateAllCpus() override = default;

    inline const CpuInfo& cpuInfo() { return cpuInfo_; }
    inline const CpuId& cpuId() { return cpuId_; }
    inline TabValues& tabValues() { return tabValues_; }
    inline TabSettings& tabSettings() { return tabSettings_; }

    QVBoxLayout* scroll_layout_;
    QScrollArea* scroll_area_;
    QWidget* scroll_widget_;
    bool have_scroll_widget_;

  private:
    std::reference_wrapper<const CpuInfo> cpuInfo_;
    std::reference_wrapper<const CpuId> cpuId_;
    std::reference_wrapper<TabValues> tabValues_;
    std::reference_wrapper<TabSettings> tabSettings_;

  public:
    QSize	sizeHint() const override;
};

#endif

