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

#ifndef CoreAdjust_ShellCommand
#define CoreAdjust_ShellCommand

// STL
#include <string>
#include <vector>
#include <initializer_list>
// Qt
#include <QDialog>
#include <QPushButton>
#include <QScrollArea>
#include <QTextEdit>

/*
 Using class ShellCommand:

    class foo {
      public:
        foo();
        void bar();
        void baz();
      private:
        ShellCommand shell_;
    };

    // ctor
    foo::foo() : shell_(this, "TITLE", FLAGS) {
      ...
      connect(&shell_, SIGNAL(finished()), &shell_, SLOT(accept()));
    }

    // basic usage
    void foo::bar() {
      bool retv = shell_.run(...);
    }

    // extended usage
    void foo::baz() {
      shell_.setWindowTitle("New title");
      shell_.cls();
      shell_.run(..., false, false);
      shell_.run(..., false, false);
      shell_.run(..., false, false);
      shell_.allowClose();
    }

*/

class ShellCommand : public QDialog {
  Q_OBJECT
  public:

    typedef enum {
      DisableAbortButton = 1,                   /**< Disable the 'abort' button. */
      EnableStdout = 2,                         /**< Display STDOUT messages. */
      EnableStderr = 4,                         /**< Display STDERR messages. */
      DisableColor = 8,                         /**< Do not use colors */
      EnableStd = EnableStdout | EnableStderr,  /**< Display STDOUT and STDERR messages (default). */
    } Flags;

    explicit ShellCommand(
        QWidget* parent = nullptr,
        const QString& title = "",
        ShellCommand::Flags flags = EnableStd);

    ~ShellCommand() override = default;

    int run(char** argv,
        bool allowClose = true, bool cls = true);

    int run(std::vector<std::string> args,
        bool allowClose = true, bool cls = true);

    int run(const std::string& cmd,
        bool allowClose = true, bool cls = true);

    int run(std::initializer_list<std::string>&& il,
        bool allowClose = true, bool cls = true);

    void allowClose();
    void cls();
    void append(const QString& str);

    QSize	sizeHint() const override;

  signals:
    void aborted();
    void finished();

  public slots:
    void abort();
    void abort(int signal);

  private:
    int callback(int, std::string);
    int timeout();
    QRect charBounds_;
    QScrollArea* scrollArea_;
    QWidget* scrollWidget_;
    QPushButton* closeBtn_;
    QPushButton* abortBtn_;
    QTextEdit* textEdit_;
    Flags flags_;
    int mustAbort_ { 0 };
};

inline constexpr ShellCommand::Flags
operator| (const ShellCommand::Flags lhs, const ShellCommand::Flags rhs) {
  return static_cast<ShellCommand::Flags>(int(lhs) | int(rhs));
}

#endif

