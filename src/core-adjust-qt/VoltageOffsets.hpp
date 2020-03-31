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

#ifndef CoreAdjust_VoltageOffsets
#define CoreAdjust_VoltageOffsets

// C++
#include <sstream>
// Qt
#include <QScrollArea>
// App
#include "ShellCommand.hpp"
#include "TabMemberBase.hpp"
#include "VoltageOffsetSlider.hpp"

class VoltageOffsets final : public TabMemberTemplate {
  Q_OBJECT
  public:
    class Values;
    class Settings;

    explicit VoltageOffsets(
        const SingleCpuInfo&, const SingleCpuId&, TabMemberValues&,
        TabMemberSettings&, QWidget* parent = nullptr);

    ~VoltageOffsets() override = default;

    void load() override;
    void store() override;
    void refresh() override;
    bool read(std::ostringstream& err) override;
    bool apply() override;
    bool compare() override;
    void timed(bool) override {}

  private:
    void store(Settings&);

    ShellCommand shell_;
    QCheckBox* force_voltage_;
    VoltageOffsetSlider* plane0_;
    VoltageOffsetSlider* plane1_;
    VoltageOffsetSlider* plane2_;
    VoltageOffsetSlider* plane3_;
    VoltageOffsetSlider* plane4_;
    VoltageOffsetSlider* plane5_;
    QLabel* current_plane0_value_;
    QLabel* current_plane1_value_;
    QLabel* current_plane2_value_;
    QLabel* current_plane3_value_;
    QLabel* current_plane4_value_;
    QLabel* current_plane5_value_;

  private slots:
    void voltageChanged();
    void forceVoltageChecked(int);
    void plane0RangeSelected(enum VoltageOffsetSlider::Range range);
    void plane1RangeSelected(enum VoltageOffsetSlider::Range range);
    void plane2RangeSelected(enum VoltageOffsetSlider::Range range);
    void plane3RangeSelected(enum VoltageOffsetSlider::Range range);
    void plane4RangeSelected(enum VoltageOffsetSlider::Range range);
    void plane5RangeSelected(enum VoltageOffsetSlider::Range range);
};

class VoltageOffsets::Values {
  private:
    double plane0_voffset_ { 0. };
    double plane1_voffset_ { 0. };
    double plane2_voffset_ { 0. };
    double plane3_voffset_ { 0. };
    double plane4_voffset_ { 0. };
    double plane5_voffset_ { 0. };
  public:
    double plane0VoltageOffset() const;
    double plane1VoltageOffset() const;
    double plane2VoltageOffset() const;
    double plane3VoltageOffset() const;
    double plane4VoltageOffset() const;
    double plane5VoltageOffset() const;
    void plane0VoltageOffset(double vo);
    void plane1VoltageOffset(double vo);
    void plane2VoltageOffset(double vo);
    void plane3VoltageOffset(double vo);
    void plane4VoltageOffset(double vo);
    void plane5VoltageOffset(double vo);
};

class VoltageOffsets::Settings
  : private VoltageOffsets::Values,
    virtual protected TabMemberWidget::Settings {

  friend class VoltageOffsets;

  public:
    Settings() = default;
    ~Settings() override = default;

    static constexpr const double VOLTAGE_OFFSET_STEP { 0.9765625 };
    static constexpr const double VOLTAGE_OFFSET_MIN { -999.0234375 };
    static constexpr const double VOLTAGE_OFFSET_MAX { 999.0234375 };

  protected:
    void load(QSettings&, const TabMemberValues&) override;
    void save(QSettings&) override;

  private:
    bool plane0VoltageOffsetEnabled() const;
    bool plane1VoltageOffsetEnabled() const;
    bool plane2VoltageOffsetEnabled() const;
    bool plane3VoltageOffsetEnabled() const;
    bool plane4VoltageOffsetEnabled() const;
    bool plane5VoltageOffsetEnabled() const;
    void plane0VoltageOffsetEnabled(bool state);
    void plane1VoltageOffsetEnabled(bool state);
    void plane2VoltageOffsetEnabled(bool state);
    void plane3VoltageOffsetEnabled(bool state);
    void plane4VoltageOffsetEnabled(bool state);
    void plane5VoltageOffsetEnabled(bool state);
    bool allowPositiveValues() const;
    void allowPositiveValues(bool state);

    static constexpr const char* INI_FIVR_FORCE { "Allow_Overvolting" };
    static constexpr const char* INI_FIVR_PLANE0_VOFFSET { "Voltage_Offset_Plane_0" };
    static constexpr const char* INI_FIVR_PLANE1_VOFFSET { "Voltage_Offset_Plane_1" };
    static constexpr const char* INI_FIVR_PLANE2_VOFFSET { "Voltage_Offset_Plane_2" };
    static constexpr const char* INI_FIVR_PLANE3_VOFFSET { "Voltage_Offset_Plane_3" };
    static constexpr const char* INI_FIVR_PLANE4_VOFFSET { "Voltage_Offset_Plane_4" };
    static constexpr const char* INI_FIVR_PLANE5_VOFFSET { "Voltage_Offset_Plane_5" };
    static constexpr const char* INI_FIVR_PLANE0_VOFFSET_ENABLE { "Voltage_Offset_Plane_0_Enabled" };
    static constexpr const char* INI_FIVR_PLANE1_VOFFSET_ENABLE { "Voltage_Offset_Plane_1_Enabled" };
    static constexpr const char* INI_FIVR_PLANE2_VOFFSET_ENABLE { "Voltage_Offset_Plane_2_Enabled" };
    static constexpr const char* INI_FIVR_PLANE3_VOFFSET_ENABLE { "Voltage_Offset_Plane_3_Enabled" };
    static constexpr const char* INI_FIVR_PLANE4_VOFFSET_ENABLE { "Voltage_Offset_Plane_4_Enabled" };
    static constexpr const char* INI_FIVR_PLANE5_VOFFSET_ENABLE { "Voltage_Offset_Plane_5_Enabled" };

    bool enable_plane0_voffset_ { false };
    bool enable_plane1_voffset_ { false };
    bool enable_plane2_voffset_ { false };
    bool enable_plane3_voffset_ { false };
    bool enable_plane4_voffset_ { false };
    bool enable_plane5_voffset_ { false };
    bool allow_positive_values_ { false };
};


inline double VoltageOffsets::Values::plane0VoltageOffset() const {
  return plane0_voffset_;
}

inline void VoltageOffsets::Values::plane0VoltageOffset(double v) {
  plane0_voffset_ = v;
}

inline double VoltageOffsets::Values::plane1VoltageOffset() const {
  return plane1_voffset_;
}

inline void VoltageOffsets::Values::plane1VoltageOffset(double v) {
  plane1_voffset_ = v;
}

inline double VoltageOffsets::Values::plane2VoltageOffset() const {
  return plane2_voffset_;
}

inline void VoltageOffsets::Values::plane2VoltageOffset(double v) {
  plane2_voffset_ = v;
}

inline double VoltageOffsets::Values::plane3VoltageOffset() const {
  return plane3_voffset_;
}

inline void VoltageOffsets::Values::plane3VoltageOffset(double v) {
  plane3_voffset_ = v;
}

inline double VoltageOffsets::Values::plane4VoltageOffset() const {
  return plane4_voffset_;
}

inline void VoltageOffsets::Values::plane4VoltageOffset(double v) {
  plane4_voffset_ = v;
}

inline double VoltageOffsets::Values::plane5VoltageOffset() const {
  return plane5_voffset_;
}

inline void VoltageOffsets::Values::plane5VoltageOffset(double v) {
  plane5_voffset_ = v;
}


inline bool VoltageOffsets::Settings::plane0VoltageOffsetEnabled() const {
  return enable_plane0_voffset_;
}

inline void VoltageOffsets::Settings::plane0VoltageOffsetEnabled(bool v) {
  enable_plane0_voffset_ = v;
}

inline bool VoltageOffsets::Settings::plane1VoltageOffsetEnabled() const {
  return enable_plane1_voffset_;
}

inline void VoltageOffsets::Settings::plane1VoltageOffsetEnabled(bool v) {
  enable_plane1_voffset_ = v;
}

inline bool VoltageOffsets::Settings::plane2VoltageOffsetEnabled() const {
  return enable_plane2_voffset_;
}

inline void VoltageOffsets::Settings::plane2VoltageOffsetEnabled(bool v) {
  enable_plane2_voffset_ = v;
}

inline bool VoltageOffsets::Settings::plane3VoltageOffsetEnabled() const {
  return enable_plane3_voffset_;
}

inline void VoltageOffsets::Settings::plane3VoltageOffsetEnabled(bool v) {
  enable_plane3_voffset_ = v;
}

inline bool VoltageOffsets::Settings::plane4VoltageOffsetEnabled() const {
  return enable_plane4_voffset_;
}

inline void VoltageOffsets::Settings::plane4VoltageOffsetEnabled(bool v) {
  enable_plane4_voffset_ = v;
}

inline bool VoltageOffsets::Settings::plane5VoltageOffsetEnabled() const {
  return enable_plane5_voffset_;
}

inline void VoltageOffsets::Settings::plane5VoltageOffsetEnabled(bool v) {
  enable_plane5_voffset_ = v;
}

inline bool VoltageOffsets::Settings::allowPositiveValues() const {
  return allow_positive_values_;
}

inline void VoltageOffsets::Settings::allowPositiveValues(bool v) {
  allow_positive_values_ = v;
}

#endif

