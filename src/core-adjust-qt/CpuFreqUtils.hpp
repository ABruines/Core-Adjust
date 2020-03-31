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

#ifndef CoreAdjust_CpuFreqUtils
#define CoreAdjust_CpuFreqUtils

#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <QCheckBox>
#include <QGroupBox>
#include <QScrollArea>
#include <QLabel>
#include "SaferCombo.hpp"
#include "SaferSlider.hpp"
#include "ShellCommand.hpp"
#include "TabMemberBase.hpp"

/**
  * @brief A TabMemberWidget for adjusting Frequency Scaling settings
  */
class CpuFreqUtils : public TabMemberTemplateAllCpus {
  Q_OBJECT
  public:
    class Processor;
    class Values;
    class Settings;

    explicit CpuFreqUtils(
      const CpuInfo& cpuInfo, const CpuId& cpuId,
      TabValues& tabValues, TabSettings& tabSettings,
      QWidget* parent = nullptr);

    ~CpuFreqUtils() override = default;

    void load() override;
    void store() override;
    void refresh() override;
    bool apply() override;
    bool read(std::ostringstream& err) override;
    bool compare() override;
    void timed(bool) override {}

    bool acdc() const { return acdc_; } // true = ac, false = dc powered

    static std::vector<std::string> GenerateShellCmd(
        const CpuInfo&, const TabSettings&);

  private:
    ShellCommand shell_;
    std::vector<Processor*> processors_;
    bool acdc_;

    int getDriverName(std::string& out);

  private slots:
    void valueChangedSlot();
};

/** @brief A single processor-tab for CpuFreqUtils */
class CpuFreqUtils::Processor : public QWidget {
  Q_OBJECT
  public:

    class Thread;

    explicit Processor(
      CpuFreqUtils& cpuFreqUtils,
      const SingleCpuInfo& cpuInfo, const SingleCpuId& cpuId,
      TabMemberValues& tabValues, TabMemberSettings& tabSettings,
      QWidget* parent = nullptr);

    virtual ~Processor() = default;

    void load();
    void store();
    void refresh();
    bool read(std::ostringstream& err);
    bool compare();

    std::vector<Thread*>& threads() { return threads_; }
    QTabWidget* tabs() { return tabs_; }

  private:
    std::reference_wrapper<CpuFreqUtils> cpuFreqUtils_;
    std::reference_wrapper<const SingleCpuInfo> cpuInfo_;
    std::reference_wrapper<TabMemberValues> tabValues_;
    std::reference_wrapper<TabMemberSettings> tabSettings_;
    std::vector<Thread*> threads_; /* front() == all cpus */
    QVBoxLayout* layout_;
    QTabWidget* tabs_;

    inline CpuFreqUtils& cpuFreqUtils() { return cpuFreqUtils_; }
    inline const SingleCpuInfo& cpuInfo() { return cpuInfo_; }
    inline TabMemberValues& tabValues() { return tabValues_; }
    inline TabMemberSettings& tabSettings() { return tabSettings_; }

    void addWidgets();
    void removeWidgets();
};

/**
  * @class CpuFreqUtils::Processor::Thread
  * @brief Cpu frequency utility for a single (or all) threads of a processor.
  *
  * @fn CpuFreqUtils::Processor::Thread::Thread(const LogicalCpuNr& cpu, const SingleCpuInfo& cpuInfo, QWidget* parent = nullptr)
  * @param cpu The logical cpu this instance is for, or -1 for all logical cpus of this processor.
  * @param cpuInfo SingleCpuInfo that the LogicalCpuNr parameter belongs to.
  * @param parent The parent widget or nullptr.
  *
  */
class CpuFreqUtils::Processor::Thread : public QWidget {
  Q_OBJECT
  public:

    explicit Thread(
        CpuFreqUtils& cpuFreqUtils,
        Processor& processor,
        const LogicalCpuNr& cpu,
        const SingleCpuInfo& cpuInfo,
        TabMemberValues& tabValues,
        TabMemberSettings& tabSettings,
        QWidget* parent = nullptr);

    ~Thread() override = default;

    void load();
    void store();
    void refresh();
    bool read(std::ostringstream& err);
    bool compare();

    QSize sizeHint() const override;

  signals:
    /* Signals CpuFreqUtils instance that we are
     * (not) using per logical cpu frequency scaling */
    void adjustSeperately(bool); // true == per logical cpu frequency scaling enabled

  private:
    bool enabled_;
    static constexpr int SliderStepping = 10;
    LogicalCpuNr cpu_;
    std::reference_wrapper<CpuFreqUtils> cpuFreqUtils_;
    std::reference_wrapper<Processor> processor_;
    std::reference_wrapper<const SingleCpuInfo> cpuInfo_;
    std::reference_wrapper<TabMemberValues> tabValues_;
    std::reference_wrapper<TabMemberSettings> tabSettings_;

    QList<QString> governors_list_;
    int current_governor_;
    int userspace_idx_;
    unsigned int hw_min_;
    unsigned int hw_max_;

    SaferCombo* governors_ac_;
    SaferSlider* minFrequency_ac_;
    SaferSlider* maxFrequency_ac_;
    SaferSlider* frequency_ac_;
    QLabel* minLabel_ac_;
    QLabel* minValue_ac_;
    QLabel* maxLabel_ac_;
    QLabel* maxValue_ac_;
    QLabel* frequencyLabel_ac_;
    QLabel* frequencyValue_ac_;
    QGroupBox* ac_box_;

    SaferCombo* governors_dc_;
    SaferSlider* minFrequency_dc_;
    SaferSlider* maxFrequency_dc_;
    SaferSlider* frequency_dc_;
    QLabel* minLabel_dc_;
    QLabel* minValue_dc_;
    QLabel* maxLabel_dc_;
    QLabel* maxValue_dc_;
    QLabel* frequencyLabel_dc_;
    QLabel* frequencyValue_dc_;
    QGroupBox* dc_box_;

    QCheckBox* cb_battery_;
    QCheckBox* cb_adjust_;
    QCheckBox* cb_all_;

    QVBoxLayout* scroll_layout_;
    QScrollArea* scroll_area_;
    QWidget* scroll_widget_;

    inline CpuFreqUtils& cpuFreqUtils() { return cpuFreqUtils_; }
    inline Processor& processor() { return processor_; }
    inline const SingleCpuInfo& cpuInfo() const { return cpuInfo_; }
    inline TabMemberValues& tabValues() { return tabValues_; }
    inline TabMemberSettings& tabSettings() { return tabSettings_; }

    void store(CpuFreqUtils::Settings&);

    unsigned int sliderValueFromFreq(unsigned int freq);
    unsigned int freqFromSliderValue(unsigned int value);

    int getGovernors(QList<QString>& governors_list);
    int getHwFreqLimits(unsigned int &min, unsigned int &max);
    int getHwFreq(unsigned int& freq);
    int getFreqLimitsAndPolicy(unsigned int& min, unsigned int& max, std::string& policy);

  private slots:

    void governorAcIndexChanged(int);
    void minFreqAcValueChanged(int);
    void maxFreqAcValueChanged(int);
    void usrFreqAcValueChanged(int);

    void governorDcIndexChanged(int);
    void minFreqDcValueChanged(int);
    void maxFreqDcValueChanged(int);
    void usrFreqDcValueChanged(int);

    void adjustStateChanged(int);
    void batteryStateChanged(int);
    void allStateChanged(int);
};

/** @brief Container class for per-processor values read by CpuFreqUtils. */
class CpuFreqUtils::Values {
  friend class CpuFreqUtils;
  friend class CpuFreqUtils::Processor;
  friend class CpuFreqUtils::Processor::Thread;

  protected:
    Values(const SingleCpuInfo& cpuInfo);

  private:

    std::vector<LogicalCpuNr> logical_;

    static constexpr const char* RANGE_MSG { "Logical CPU number out of range." };

    /* per-thread values */

    const std::string& frequencyScalingGovernorAc(const LogicalCpuNr& l) const;
    void frequencyScalingGovernorAc(const LogicalCpuNr& l, std::string s);
    unsigned int frequencyScalingMinFreqAc(const LogicalCpuNr& l) const;
    void frequencyScalingMinFreqAc(const LogicalCpuNr& l, unsigned int freq);
    unsigned int frequencyScalingMaxFreqAc(const LogicalCpuNr& l) const;
    void frequencyScalingMaxFreqAc(const LogicalCpuNr& l, unsigned int freq);
    unsigned int frequencyScalingFreqAc(const LogicalCpuNr& l) const;
    void frequencyScalingFreqAc(const LogicalCpuNr& l, unsigned int freq);

    const std::string& frequencyScalingGovernorDc(const LogicalCpuNr& l) const;
    void frequencyScalingGovernorDc(const LogicalCpuNr& l, std::string s);
    unsigned int frequencyScalingMinFreqDc(const LogicalCpuNr& l) const;
    void frequencyScalingMinFreqDc(const LogicalCpuNr& l, unsigned int freq);
    unsigned int frequencyScalingMaxFreqDc(const LogicalCpuNr& l) const;
    void frequencyScalingMaxFreqDc(const LogicalCpuNr& l, unsigned int freq);
    unsigned int frequencyScalingFreqDc(const LogicalCpuNr& l) const;
    void frequencyScalingFreqDc(const LogicalCpuNr& l, unsigned int freq);

    std::vector<std::string> frequency_scaling_governor_ac_;
    std::vector<unsigned int> frequency_scaling_min_freq_ac_;
    std::vector<unsigned int> frequency_scaling_max_freq_ac_;
    std::vector<unsigned int> frequency_scaling_usr_freq_ac_;

    std::vector<std::string> frequency_scaling_governor_dc_;
    std::vector<unsigned int> frequency_scaling_min_freq_dc_;
    std::vector<unsigned int> frequency_scaling_max_freq_dc_;
    std::vector<unsigned int> frequency_scaling_usr_freq_dc_;
};

/** @brief Container class for the per-processor settings stored by CpuFreqUtils. */
class CpuFreqUtils::Settings
  : private CpuFreqUtils::Values,
    virtual protected TabMemberWidget::Settings {

  friend class CpuFreqUtils;
  friend class CpuFreqUtils::Processor;
  friend class CpuFreqUtils::Processor::Thread;

  public:
    Settings(const SingleCpuInfo& ci);

  protected:
    void load(QSettings&, const TabMemberValues&) override;
    void save(QSettings&) override;

  private:
    static constexpr const char* CPUFREQ_INFO = "/usr/bin/cpufreq-info";
    static constexpr const char* INI_CPUFREQ_GRP { "CpuFreqUtils" };
    static constexpr const char* INI_CPUFREQ_GRP_CPU { "CpuFreqUtils_Thread" };
    static constexpr const char* INI_CPUFREQ_ENABLE { "Enabled" };
    static constexpr const char* INI_CPUFREQ_PER_CPU_ENABLE { "PerCpu_Enabled" };
    static constexpr const char* INI_CPUFREQ_BATTERY_ENABLE { "Battery_Enabled" };
    static constexpr const char* INI_CPUFREQ_GOVERNOR_AC { "Governor_AC" };
    static constexpr const char* INI_CPUFREQ_MIN_FREQ_AC { "Min_Frequency_AC" };
    static constexpr const char* INI_CPUFREQ_MAX_FREQ_AC { "Max_Frequency_AC" };
    static constexpr const char* INI_CPUFREQ_USR_FREQ_AC { "Userspace_Frequency_AC" };
    static constexpr const char* INI_CPUFREQ_GOVERNOR_DC { "Governor_DC" };
    static constexpr const char* INI_CPUFREQ_MIN_FREQ_DC { "Min_Frequency_DC" };
    static constexpr const char* INI_CPUFREQ_MAX_FREQ_DC { "Max_Frequency_DC" };
    static constexpr const char* INI_CPUFREQ_USR_FREQ_DC { "Userspace_Frequency_DC" };
    static constexpr const char* RANGE_MSG { "Logical CPU number out of range." };

    std::vector<LogicalCpuNr> logical_;

    /* per-thread settings */

    bool frequencyScalingEnabled(const LogicalCpuNr& l) const;
    void frequencyScalingEnabled(const LogicalCpuNr& l, bool state);
    bool frequencyScalingBatteryEnabled(const LogicalCpuNr& l) const;
    void frequencyScalingBatteryEnabled(const LogicalCpuNr& l, bool state);

    std::vector<bool> enable_frequency_scaling_;
    std::vector<bool> enable_battery_frequency_scaling_;

    /* per-processor settings */

    bool gFrequencyScalingEnabled() const;
    void gFrequencyScalingEnabled(bool state);
    bool gFrequencyScalingPerCpuEnabled() const;
    void gFrequencyScalingPerCpuEnabled(bool state);
    bool gFrequencyScalingBatteryEnabled() const;
    void gFrequencyScalingBatteryEnabled(bool state);

    bool g_enable_frequency_scaling_;
    bool g_enable_per_cpu_frequency_scaling_;
    bool g_enable_battery_frequency_scaling_;

    inline const SingleCpuInfo& cpuInfo() const override { return cpuInfo_; }
    std::reference_wrapper<const SingleCpuInfo> cpuInfo_;
    int hw_threads_;
};

#endif

