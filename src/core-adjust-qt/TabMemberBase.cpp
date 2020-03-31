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

/** @file src/core-adjust-qt/TabMemberBase.hpp
  * @brief Base classes for members of a QTabWidget in the Core Adjust application.
  *
  * @file src/core-adjust-qt/TabMemberBase.cpp
  * @brief Base classes for members of a QTabWidget in the Core Adjust application (implementation).
  *
  *
  * @class TabMemberWidget
  * @brief A pure virtual base class (widget) that must be implemented by every
  * widget that is a member of a QTabWidget.
  *
  * @fn virtual void TabMemberWidget::load()
  * @brief Load the values for the UI items from the Settings instance.
  *
  * @fn virtual void TabMemberWidget::store()
  * @brief Store the values for the UI items in the Settings instance.
  *
  * @fn virtual void TabMemberWidget::refresh()
  * @brief Refresh the current (processor) values displayed on this tab.
  *
  * @fn virtual bool TabMemberWidget::read(std::stringstream& err, bool haveEventLoop = true)
  * @brief (Re)Read the TabValues for this tab from the processor/system/...
  * @return False if the new TabValues differ from the current settings.
  *
  * @fn virtual bool TabMemberWidget::apply()
  * @brief Apply the stored (processor) values displayed on this tab.
  *
  * @fn virtual bool TabMemberWidget::compare()
  * @brief Compare the current widget values against the current TabValues
  * @return True if the comparison is equal
  *
  * @fn virtual void TabMemberWidget::timed(bool is_current_tab)
  * @brief Timer callback function, called every 500ms after construction.
  *
  * @fn virtual void TabMemberWidget::valueChanged(TabMemberWidget*)
  * @brief The signal emitted when the user modified an item on this tab.
  *
  *
  * @class TabMemberWidget::Settings
  * @brief An abstract base class that may optionaly be implemented by a
  * TabMemberWidget and manages (access to) its configuration data.
  * @note This class must use virtual inheritance.
  *
  * @fn virtual void TabMemberWidget::Settings::load(QSettings&, const SingleCpuInfo&, const TabMemberValues&)
  * @brief Load the settings for this TabMemberWidget from the INI file.
  *
  * @fn virtual void TabMemberWidget::Settings::save(QSettings&)
  * @brief Save the settings for this TabMemberWidget to the INI file.
  *
  * @fn virtual TabMemberSettings& TabMemberWidget::Settings::tabMemberSettings()
  * @brief Accessor for the TabMemberSettings in the main TabSettings instance.
  *
  * @fn virtual CommonSettings& TabMemberWidget::Settings::commonSettings()
  * @brief Accessor for the CommonSettings in the main TabSettings instance.
  *
  *
  *
  * @class TabMemberTemplate
  * @brief Template for a TabMemberWidget implementation.
  *
  * @fn template<class T> explicit TabMemberTemplate::TabMemberTemplate(const SingleCpuInfo& cpuInfo, const SingleCpuId& cpuId, TabMemberValues& tabValues, TabMemberSettings& tabSettings, QWidget* parent, T* scroll_layout)
  * @param cpuInfo
  * Reference to the global CpuInfo instance.
  * @param cpuId
  * Reference to the global CpuId instance.
  * @param tabValues
  * Reference to the TabMemberValues instance.
  * @param tabSettings
  * Reference to the TabMemberSettings instance.
  * @param parent
  * Pointer to the parent widget.
  * @param scroll_layout
  * The layout that is added to the QScrollArea.
  *
  * @fn inline const SingleCpuInfo& TabMemberTemplate::cpuInfo()
  * @brief Return a reference to the global CpuInfo instance.
  *
  * @fn inline const SingleCpuId& TabMemberTemplate::cpuId()
  * @brief Return a reference to the global CpuId instance.
  *
  * @fn inline TabMemberValues& TabMemberTemplate::tabValues()
  * @brief Return a reference to the TabMemberValues instance.
  *
  * @fn inline TabMemberSettings& TabMemberTemplate::tabSettings()
  * @brief Return a reference to the TabMemberSettings instance.
  *
  * @fn QSize	TabMemberTemplate::sizeHint() const override
  * @brief Reimplements QWidget::sizeHint()
  *
  *
  *
  * @class TabMemberTemplateAllCpus
  * @brief Template for a TabMemberWidget implementation.
  *
  * @fn template<class T> explicit TabMemberTemplateAllCpus::TabMemberTemplateAllCpus(const CpuInfo& cpuInfo, const CpuId& cpuId, TabValues& tabValues, TabSettings& tabSettings, QWidget* parent, T* scroll_layout)
  * @param cpuInfo
  * Reference to the global CpuInfo instance.
  * @param cpuId
  * Reference to the global CpuId instance.
  * @param tabValues
  * Reference to the global TabValues instance.
  * @param tabSettings
  * Reference to the global TabSettings instance.
  * @param parent
  * Pointer to the parent widget.
  * @param scroll_layout
  * The layout that is added to the QScrollArea.
  *
  * @fn inline const SingleCpuInfo& TabMemberTemplateAllCpus::cpuInfo()
  * @brief Return a reference to the global CpuInfo instance.
  *
  * @fn inline const SingleCpuId& TabMemberTemplateAllCpus::cpuId()
  * @brief Return a reference to the global CpuId instance.
  *
  * @fn inline TabMemberValues& TabMemberTemplateAllCpus::tabValues()
  * @brief Return a reference to the global TabValues instance.
  *
  * @fn inline TabMemberSettings& TabMemberTemplateAllCpus::tabSettings()
  * @brief Return a reference to the global TabSettings instance.
  *
  * @fn QSize	TabMemberTemplateAllCpus::sizeHint() const override
  * @brief Reimplements QWidget::sizeHint()
  */
#include <stdexcept>
#include "TabMemberBase.hpp"

TabMemberSettings& TabMemberWidget::Settings::tabMemberSettings() {
  throw std::runtime_error("Missing override for TabMemberWidget::Settings::data()");
}

CommonSettings& TabMemberWidget::Settings::commonSettings() {
  throw std::runtime_error("Missing override for TabMemberWidget::Settings::common()");
}

const SingleCpuInfo& TabMemberWidget::Settings::cpuInfo() const {
  throw std::runtime_error("Missing override for TabMemberWidget::Settings::cpuInfo()");
}

TabMemberTemplate::TabMemberTemplate(
  const SingleCpuInfo& cpuInfo, const SingleCpuId& cpuId,
  TabMemberValues& tabValues, TabMemberSettings& tabSettings,
  QWidget* parent, bool have_scroll_widget)
  : TabMemberWidget(parent), have_scroll_widget_(have_scroll_widget),
    cpuInfo_(cpuInfo), cpuId_(cpuId),
    tabValues_(tabValues), tabSettings_(tabSettings) {

  if (have_scroll_widget_) {
    /* Inheritor must add the inner widgets to scroll_layout_ */
    scroll_layout_ = new QVBoxLayout();

    /* Setup scroll widget/area */
    scroll_widget_ = new QWidget();
    scroll_widget_->setLayout(scroll_layout_);
    scroll_area_ = new QScrollArea();
    scroll_area_->setWidget(scroll_widget_);
    scroll_area_->setWidgetResizable(true);

    /* Widget main layout */
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(scroll_area_);
  }
}

QSize	TabMemberTemplate::sizeHint() const {
  if (have_scroll_widget_) {
    auto&& size = scroll_widget_->sizeHint();
    return QSize(size.width() + 50, size.height() + 50);
  }
  return QWidget::sizeHint();
}

TabMemberTemplateAllCpus::TabMemberTemplateAllCpus(
  const CpuInfo& cpuInfo, const CpuId& cpuId,
  TabValues& tabValues, TabSettings& tabSettings,
  QWidget* parent, bool have_scroll_widget)
  : TabMemberWidget(parent), have_scroll_widget_(have_scroll_widget),
    cpuInfo_(cpuInfo), cpuId_(cpuId),
    tabValues_(tabValues), tabSettings_(tabSettings) {

  if (have_scroll_widget_) {
    /* Inheritor must add the inner widgets to scroll_layout_ */
    scroll_layout_ = new QVBoxLayout();

    /* Setup scroll widget/area */
    scroll_widget_ = new QWidget();
    scroll_widget_->setLayout(scroll_layout_);
    scroll_area_ = new QScrollArea();
    scroll_area_->setWidget(scroll_widget_);
    scroll_area_->setWidgetResizable(true);

    /* Widget main layout */
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(scroll_area_);
  }
}

QSize	TabMemberTemplateAllCpus::sizeHint() const {
  if (have_scroll_widget_) {
    auto size = scroll_widget_->sizeHint();
    return { size.width() + 50, size.height() + 50 };
  }
  return QWidget::sizeHint();
}

