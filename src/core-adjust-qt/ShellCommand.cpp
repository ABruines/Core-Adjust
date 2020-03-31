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
  * @file src/core-adjust-qt/ShellCommand.hpp
  * @brief A QDialog that runs an external process and displays the output.
  *
  * @file src/core-adjust-qt/ShellCommand.cpp
  * @brief A QDialog that runs an external process and displays the output (implementation).
  *
  * @class ShellCommand
  * @brief A QDialog that runs an external process and displays the output.
  *
  * @fn explicit ShellCommand::ShellCommand(QWidget* parent = nullptr, const QString& title = "", ShellCommand::Flags flags = EnableStd)
  * @param parent The parent QWidget.
  * @param title The title of the dialog.
  * @param flags The options for the dialog.
  *
  * @fn int ShellCommand::run(char** argv, bool allowClose = true, bool cls = true)
  * @brief Run an external (child) process and write its output to this dialog.
  * @param argv Array with the command and its optional arguments.
  * @param allowClose Set to false to not enable the 'close' button when the child process has finished.
  * @param cls Set to true to clear the textedit box before executing the command.
  * @return The exit value of the child process.
  * @note This method blocks until the child process has finished.
  * @note The allowClose option can be used to prevent the user closing the dialog when running several commands in succession.
  * @see xxx::shell_command()
  *
  * @fn int ShellCommand::run(std::vector<std::string> args, bool allowClose = true, bool cls = true)
  * @brief Run an external (child) process and write its output to this dialog.
  * @param args Vector with the command and its optional arguments.
  * @param allowClose Set to false to not enable the 'close' button when the child process has finished.
  * @param cls Set to true to clear the textedit box before executing the command.
  * @return The exit value of the child process.
  * @note This method blocks until the child process has finished.
  * @note The allowClose option can be used to prevent the user closing the dialog when running several commands in succession.
  * @see xxx::shell_command()
  *
  * @fn int ShellCommand::run(const std::string& cmd, bool allowClose = true, bool cls = true)
  * @brief Run an external (child) process and write its output to this dialog.
  * @param cmd The command and its optional arguments.
  * @param allowClose Set to false to not enable the 'close' button when the child process has finished.
  * @param cls Set to true to clear the textedit box before executing the command.
  * @return The exit value of the child process.
  * @note This method blocks until the child process has finished.
  * @note The allowClose option can be used to prevent the user closing the dialog when running several commands in succession.
  * @see xxx::shell_command()
  *
  * @fn int ShellCommand::run(std::initializer_list<std::string> il, bool allowClose = true, bool cls = true)
  * @brief Run an external (child) process and write its output to this dialog.
  * @param il Initializer list with the command and its optional arguments.
  * @param allowClose Set to false to not enable the 'close' button when the child process has finished.
  * @param cls Set to true to clear the textedit box before executing the command.
  * @return The exit value of the child process.
  * @note This method blocks until the child process has finished.
  * @note The allowClose option can be used to prevent the user closing the dialog when running several commands in succession.
  * @see xxx::shell_command();
  *
  * @fn void ShellCommand::allowClose()
  * @brief If run() was called with the 'allowClose' parameter set to false,
  * then calling this method will enable the close button.
  *
  * @fn void ShellCommand::cls()
  * @brief Clear the 'screen' when called after run()
  *
  * @fn void ShellCommand::append(const QString& str)
  * @brief Append text to the 'console'
  *
  * @fn void ShellCommand::aborted()
  * @brief This signal is emitted when the Abort button is clicked
  *
  * @fn void ShellCommand::finished()
  * @brief This signal is emitted when the Close button is clicked
  *
  * @fn void ShellCommand::abort()
  * @brief Sends the SIGKILL signal to the child process.
  *
  * @fn void ShellCommand::abort(int signal)
  * @brief Sends 'signal' to the child process.
  *
  * @fn QSize	ShellCommand::sizeHint() const override
  * @brief Override for QWidget::sizeHint()
  *
  * @fn inline constexpr ShellCommand::Flags operator| (const ShellCommand::Flags lhs, const ShellCommand::Flags rhs)
  * @brief Bitwise OR operator for ShellCommand::Flags.
  */
// STL
#include <functional>
// Qt
#include <QApplication>
#include <QVBoxLayout>
#include <QStyle>
// App
#include "Shell.hpp"
#include "ShellCommand.hpp"
#include "Strings.hpp"

ShellCommand::ShellCommand(
  QWidget* parent,
  const QString& title,
  Flags flags)
  : QDialog(parent), flags_(flags) {

  setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
  setModal(true);
  setWindowTitle(title);

  textEdit_ = new QTextEdit();
  textEdit_->setReadOnly(true);
  textEdit_->setLineWrapMode(QTextEdit::FixedColumnWidth);
  textEdit_->setLineWrapColumnOrWidth(80);
  textEdit_->setWordWrapMode(QTextOption::WrapAnywhere);
  textEdit_->setFont(QFont("monospace"));
  charBounds_ = textEdit_->fontMetrics().boundingRect("X");

  closeBtn_ = new QPushButton("Close");
  abortBtn_ = new QPushButton("Abort");

  if (flags_ & DisableAbortButton) {
    abortBtn_->setVisible(false);
  }

  auto* btnLayout = new QHBoxLayout();
  btnLayout->addStretch(1);
  btnLayout->addWidget(closeBtn_);
  btnLayout->addWidget(abortBtn_);
  btnLayout->addStretch(1);

  /* main layout */

  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(textEdit_);
  mainLayout->addLayout(btnLayout);

  connect(closeBtn_, SIGNAL(released()), this, SIGNAL(finished()));
  connect(abortBtn_, SIGNAL(released()), this, SIGNAL(aborted()));
}

QSize ShellCommand::sizeHint() const {
  /* close enough */
  int horizontal_margin =
      (qApp->style()->pixelMetric(QStyle::PM_LayoutLeftMargin) * 2) +
      qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent) +
      (qApp->style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing) * 2) +
      (qApp->style()->pixelMetric(QStyle::PM_LayoutRightMargin) * 2) + 20;

  /* Set the QTextEdit size to (about) 80x25 characters. */
  return QSize(
      charBounds_.width() * 80 + horizontal_margin,
      charBounds_.height() * 30);
}

void ShellCommand::abort() {
  mustAbort_ = -1;
}

void ShellCommand::abort(int signal) {
  mustAbort_ = signal;
}

void ShellCommand::allowClose() {
  closeBtn_->setEnabled(true);
}

void ShellCommand::cls() {
  textEdit_->clear();
}

void ShellCommand::append(const QString& str) {
  textEdit_->setTextColor(Qt::darkGreen);
  textEdit_->append(str);
}

int ShellCommand::callback(int origin, std::string line) {
  if (line.back() == '\n') line.pop_back();
  switch (origin) {
    case 0:
      if (flags_ & EnableStdout) {
        if (!(flags_ & DisableColor)) {
          textEdit_->setTextColor(palette().color(QPalette::WindowText));
        }
        textEdit_->append(QString::fromStdString(xxx::strip_ansi(line)));
      }
      break;
    case 1:
      if (flags_ & EnableStderr) {
        if (!static_cast<int>(flags_ & DisableColor)) {
          textEdit_->setTextColor(Qt::red);
        }
        textEdit_->append(QString::fromStdString(xxx::strip_ansi(line)));
      }
      break;
    default:
      if (!(flags_ & DisableColor)) {
        textEdit_->setTextColor(Qt::red);
      }
      textEdit_->append(QString::fromStdString(xxx::strip_ansi(line)));
      break;
  }
  return 0;
}

int ShellCommand::timeout() {
  QApplication::processEvents();
  if (mustAbort_) {
    int rv = mustAbort_;
    mustAbort_ = 0;
    return rv;
  }
  return 0;
}

int ShellCommand::run(char** argv, bool allowClose, bool cls) {
  using namespace std::placeholders;
  closeBtn_->setEnabled(false);
  abortBtn_->setEnabled(!(flags_ & DisableAbortButton));
  if (cls) textEdit_->clear();
  show();
  int retv = xxx::shell_command(
    argv,
    std::bind(&ShellCommand::callback, this, _1, _2),
    std::bind(&ShellCommand::timeout, this), 10
  );
  if (allowClose) closeBtn_->setEnabled(true);
  abortBtn_->setEnabled(false);
  mustAbort_ = 0;
  return retv;
}

int ShellCommand::run(std::vector<std::string> argv, bool allowClose, bool cls) {
  using namespace std::placeholders;
  closeBtn_->setEnabled(false);
  abortBtn_->setEnabled(!(flags_ & DisableAbortButton));
  if (cls) textEdit_->clear();
  show();
  int retv = xxx::shell_command(
    std::move(argv),
    std::bind(&ShellCommand::callback, this, _1, _2),
    std::bind(&ShellCommand::timeout, this), 10
  );
  if (allowClose) closeBtn_->setEnabled(true);
  abortBtn_->setEnabled(false);
  mustAbort_ = 0;
  return retv;
}

int ShellCommand::run(const std::string& command, bool allowClose, bool cls) {
  using namespace std::placeholders;
  closeBtn_->setEnabled(false);
  abortBtn_->setEnabled(!(flags_ & DisableAbortButton));
  if (cls) textEdit_->clear();
  show();
  int retv = xxx::shell_command(
    command,
    std::bind(&ShellCommand::callback, this, _1, _2),
    std::bind(&ShellCommand::timeout, this), 10
  );
  if (allowClose) closeBtn_->setEnabled(true);
  abortBtn_->setEnabled(false);
  mustAbort_ = 0;
  return retv;
}

int ShellCommand::run(std::initializer_list<std::string>&& argv, bool allowClose, bool cls) {
  return run(std::vector<std::string>(std::move(argv)), allowClose, cls);
}


