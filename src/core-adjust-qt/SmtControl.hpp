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

#ifndef CoreAdjust_SmtControl
#define CoreAdjust_SmtControl

// STL
#include <sstream>
#include <vector>
// Qt
#include <QButtonGroup>
#include <QCheckBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QTabWidget>
// App
#include "CpuNumber.hpp"
#include "CpuId.hpp"
#include "CpuInfo.hpp"
#include "ShellCommand.hpp"
#include "TabMemberBase.hpp"

class HttControl final : public QWidget {
  Q_OBJECT
  public:
    explicit HttControl(QWidget *parent = nullptr);
    virtual ~HttControl() = default;
    QRadioButton* dont_disable_smt_;
    QRadioButton* disable_smt_processor_;
    QButtonGroup* buttons_;

  private:
    QScrollArea* scroll_area_;
    QWidget* scroll_widget_;

  signals:
    void valueChanged(void);

  private slots:
    void button_toggled(int, bool);
};

class SmtControl final : public TabMemberTemplateAllCpus {
  Q_OBJECT
  public:
    class CommonValues;
    class CommonSettings;
    class Values;
    class Settings;

    explicit SmtControl(
        const CpuInfo&, const CpuId&, TabValues&, TabSettings&,
        QWidget *parent = nullptr);

    ~SmtControl() override = default;

    void load() override;
    void store() override;
    void refresh() override;
    bool read(std::ostringstream& err) override;
    bool apply() override;
    bool compare() override;
    void timed(bool) override {}

  private:
    ShellCommand shell_;

    /* This works because our parent calls the compare() method after this
     * class emits a valueChanged signal. */
    bool doApply { false };

    bool global_smt_support_ { false };
    std::vector<bool> cpu_htt_support_;

    QTabWidget* tabs_;

    QRadioButton* dont_adjust_smt_;
    QRadioButton* enable_smt_global_;
    QRadioButton* disable_smt_global_;
    QCheckBox* per_processor_;
    QButtonGroup* buttons_;

    void store(TabSettings&);

    int applySMT(bool state);
    int applyHTT(PhysCpuNr n, bool state);

  private slots:
    void button_toggled(int, bool);
    void httValueChanged();
    void perCpuChecked(int state);
};

class SmtControl::CommonValues {
  friend class SmtControl;
  private:
    bool smtGlobalDisable() const;
    void smtGlobalDisable(bool state);

    bool smt_global_disable_ { false };
};

class SmtControl::CommonSettings : private SmtControl::CommonValues {
  friend class SmtControl;
  private:
    bool smtGlobalDisableEnabled() const;
    void smtGlobalDisableEnabled(bool state);
    bool httPerCpuEnabled() const;
    void httPerCpuEnabled(bool state);

    bool enable_smt_global_disable_ { false };
    bool htt_per_cpu_enable_ { false };
};

class SmtControl::Values {
  friend class SmtControl;
  private:
    bool httDisable() const;
    void httDisable(bool state);

    bool htt_disable_ { false };
};

class SmtControl::Settings
  : private SmtControl::Values,
    virtual protected TabMemberWidget::Settings {

  friend class SmtControl;

  public:
    Settings() = default;
    ~Settings() override = default;

  protected:
    void load(QSettings&, const TabMemberValues&) override;
    void save(QSettings&) override;

  private:
    bool httDisableEnabled() const;
    void httDisableEnabled(bool state);

    static constexpr const char* INI_SMT_DISABLE { "SMT_Disable" };
    static constexpr const char* INI_SMT_DISABLE_ENABLE { "SMT_Disable_Enabled" };
    static constexpr const char* INI_HTT_PER_CPU_ENABLE { "CPU_HTT_Enable" };
    static constexpr const char* INI_HTT_DISABLE { "HTT_Disable" };
    static constexpr const char* INI_HTT_DISABLE_ENABLE { "HTT_Disable_Enabled" };

    bool enable_htt_disable_ { false };
};


inline bool SmtControl::CommonSettings::smtGlobalDisableEnabled() const {
  return enable_smt_global_disable_;
}

inline void SmtControl::CommonSettings::smtGlobalDisableEnabled(bool v) {
  enable_smt_global_disable_ = v;
}

inline bool SmtControl::CommonSettings::httPerCpuEnabled() const {
  return htt_per_cpu_enable_;
}

inline void SmtControl::CommonSettings::httPerCpuEnabled(bool v) {
  htt_per_cpu_enable_ = v;
}


inline bool SmtControl::CommonValues::smtGlobalDisable() const {
  return smt_global_disable_;
}

inline void SmtControl::CommonValues::smtGlobalDisable(bool v) {
  smt_global_disable_ = v;
}


inline bool SmtControl::Values::httDisable() const {
  return htt_disable_;
}

inline void SmtControl::Values::httDisable(bool v) {
  htt_disable_ = v;
}


inline bool SmtControl::Settings::httDisableEnabled() const {
  return enable_htt_disable_;
}

inline void SmtControl::Settings::httDisableEnabled(bool v) {
  enable_htt_disable_ = v;
}

#endif

