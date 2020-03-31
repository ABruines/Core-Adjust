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

#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include "CpuId.hpp"
#include "Dbg.hpp"
#include "TabMember.hpp"
#include "Shell.hpp"

#ifdef DEBUG
CpuId* CpuId::singleton_ = nullptr;
#endif

CpuId::CpuId(bool haveEventLoop) : std::vector<SingleCpuId>() {
#ifdef DEBUG
  if (singleton_ != nullptr) {
    throw std::runtime_error("There can only be one CpuId instance!");
  }
  singleton_ = this;
#endif

  refresh(haveEventLoop);
}

void CpuId::refresh(bool haveEventLoop) {
  /* Delete the current data */
  clear();

  /* Let the core-adjust script get the CPUID information. */
  std::vector<std::string> output;
  auto rv = xxx::shell_command(
      { TabSettings::ScriptPath, "--cpuid" },
      [&output](auto, auto str){
    output.push_back(std::move(str));
    return 0;
  });

  if (rv) {
    QMessageBox::critical(nullptr, "Core Adjust",
      "<p><b>Error, unexpected output from command:</b></p>"
      "<nobr>core-adjust --cpuid</nobr>");
    if (haveEventLoop) {
      qApp->exit();
    }
    else {
      exit(EXIT_FAILURE);
    }
  }

  /* Parse the output of the shell command */
  for (auto iter = output.begin(); iter != output.end();) {

    /* Expecting 33 lines of output per processor */
    constexpr const int numLines = 33;

    if (std::distance(iter, output.end()) < numLines) {
      QMessageBox::critical(nullptr, "Core Adjust",
        "<p><b>Error, unexpected output from command:</b></p>"
        "<nobr>core-adjust --cpuid</nobr>");
      if (haveEventLoop) {
        qApp->exit();
      }
      else {
        exit(EXIT_FAILURE);
      }
    }

    ++iter; /* skip package id line */

    /* loop over all the lines */
    SingleCpuId c;
    for (int i = 0; i < numLines - 1; ++i) {
      /* extract the values on each line to v */
      std::vector<uint32_t> v;
      size_t idx = 0, offset = 0;
      while (offset < iter->length() - 1) {
        v.push_back(static_cast<unsigned int>(
            std::stoul(&iter->at(offset), &idx, 16)));
        offset += idx;
      }
      /* there are exactly 6 values on a line */
      if (v.size() == 6) {
        /*
         * v[0] = CPUID EAX input value (leaf)
         * v[1] = CPUID ECX input value (sub-leaf)
         * v[2] = CPUID EAX output value
         * v[3] = CPUID EBX output value
         * v[4] = CPUID ECX output value
         * v[5] = CPUID EDX output value
         */
        SingleCpuId::Entry e(v[2], v[3], v[4], v[5]);
        /* select the EAX input to CPUID (leaf) */
        switch (v[0]) {
          case 0x00000000: c.EAX_00H = e; break;
          case 0x00000001: c.EAX_01H = e; break;
          case 0x00000002: c.EAX_02H = e; break;
          case 0x00000003: c.EAX_03H = e; break;
          case 0x00000004:
            switch (v[1]) {
              case 0x00: c.EAX_04H_ECX_00H = e; break;
              case 0x01: c.EAX_04H_ECX_01H = e; break;
              case 0x02: c.EAX_04H_ECX_02H = e; break;
              case 0x03: c.EAX_04H_ECX_03H = e; break;
            }
            break;
          case 0x00000005: c.EAX_05H = e; break;
          case 0x00000006: c.EAX_06H = e; break;
          case 0x00000007: c.EAX_07H = e; break;
          case 0x00000008: c.EAX_08H = e; break;
          case 0x00000009: c.EAX_09H = e; break;
          case 0x0000000a: c.EAX_0AH = e; break;
          case 0x0000000b:
            switch (v[1]) {
              case 0x00: c.EAX_0BH_ECX_00H = e; break;
              case 0x01: c.EAX_0BH_ECX_01H = e; break;
            }
            break;
          case 0x0000000c: c.EAX_0CH = e; break;
          case 0x0000000d:
            switch (v[1]) {
              case 0x00: c.EAX_0DH_ECX_00H = e; break;
              case 0x01: c.EAX_0DH_ECX_01H = e; break;
              case 0x02: c.EAX_0DH_ECX_02H = e; break;
            }
            break;
          case 0x0000001f:
            switch (v[1]) {
              case 0x01: c.EAX_1FH_ECX_01H = e; break;
            }
            break;
          case 0x80000000: c.EAX_80000000H = e; break;
          case 0x80000001: c.EAX_80000001H = e; break;
          case 0x80000002: c.EAX_80000002H = e; break;
          case 0x80000003: c.EAX_80000003H = e; break;
          case 0x80000004: c.EAX_80000004H = e; break;
          case 0x80000005: c.EAX_80000005H = e; break;
          case 0x80000006: c.EAX_80000006H = e; break;
          case 0x80000007: c.EAX_80000007H = e; break;
          case 0x80000008: c.EAX_80000008H = e; break;
          case 0x80860000: c.EAX_80860000H = e; break;
          case 0xc0000000: c.EAX_C0000000H = e; break;
        }
      }
      else {
        QMessageBox::critical(nullptr, "Core Adjust",
          "<p><b>Error, unexpected output from command:</b></p>"
          "<nobr>core-adjust --cpuid</nobr>");
        if (haveEventLoop) {
          qApp->exit();
        }
        else {
          exit(EXIT_FAILURE);
        }
      }
      ++iter;
    }
    push_back(std::move(c));
  }
  DBGMSG("CpuId::refresh(): Got CPUID information for" << size() << "processor(s).")
}

SingleCpuId& CpuId::operator[](const PhysCpuNr& idx) {
  return std::vector<SingleCpuId>::operator[](idx.value);
}

const SingleCpuId& CpuId::operator[](const PhysCpuNr& idx) const {
  return std::vector<SingleCpuId>::operator[](idx.value);
}

SingleCpuId& CpuId::at(const PhysCpuNr& idx) {
  return std::vector<SingleCpuId>::at(idx.value);
}

const SingleCpuId& CpuId::at(const PhysCpuNr& idx) const {
  return std::vector<SingleCpuId>::at(idx.value);
}

