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
  * @file src/libcommon/Shell.hpp
  * @brief Execute shell-commands using fork()/execvp().
  */
#ifndef CoreAdjust_shell_hpp
#define CoreAdjust_shell_hpp

#include <functional>
#include <string>
#include <utility>
#include <vector>

// FIXME: Text from STDOUT and STDERR is send to the callback 'out of order'

/** @brief Namespace for everything in the libcommon support library */
namespace xxx {

  /** @brief Detect if this application was started with either
    * its set-user-ID or set-group-ID bit set.
    *
    * The application should never be run with SUID or SGID because this is a
    * security risc as this application uses fork()/execvp() to execute shell
    * commands.
    *
    * This function throws a runtime_error if either set-user-ID or
    * set-group-ID bit has been set.
    *
    * @warning The argv[0] of main() may not be the full path, on Linux use /proc/self/exe instead. 
    */
  void shell_safety_test(const char* argv0);

  /** @brief Converts a shell command-line into a vector of command + arguments
    * suitable to be used as 'argv' argument for execvp() */
  std::vector<std::string> shell_command_to_vector(const std::string&);

  int shell_command_default_callback(int type, std::string s);
  int shell_command_default_timeout();

  static constexpr const int shell_command_default_timeout_ms = 100;

  /** @brief Execute a (single) shell-command using fork()/execvp().
    *
    * @param argv
    * The command-line where argv[0] is the command to execute and
    * argv[1+n] is/are the argument(s) to pass to that command.<br/>
    * The last pointer in the array must be a NULL pointer.<br/>
    * See the documentation for execvp().
    *
    * @param callback
    * A non-blocking function that is called once for every line of output from the child process.<br/>
    * This function shall take two arguments (an integer and a std::string) and
    * must return an integer value.<br/>
    * The integer value passed to the function has one of the following values:
    * <ul>
    * <li>-2 if the child process was terminated by a signal, the string contains an error message.</li>
    * <li>-1 if the call to execvp() has failed, the string contains an error message.
    * <li>0 if the string originated from STDOUT of the child process.</li>
    * <li>1 if the string originated from STDERR of the child process.</li>
    * </ul>
    *
    * @param timeout
    * A non-blocking function that is called every 'timeout_ms' milliseconds while the
    * child process is running, it takes no arguments and must return an integer.
    *
    * @param timeout_ms
    * The amount of time (in milliseconds) to wait before calling 'timeout'.
    *
    * @returns The exit value of the child process.
    *
    * @note Please use full paths to execute a shell command (ie. do not
    * rely on the PATH env) and sanitize the command arguments if influenced
    * by user input.
    *
    * @note If 'callback' was called with a negative integer value then its return value
    * is ignored (because the child process has allready exited).
    *
    * @note If either 'callback' or 'timeout' returns a positive value then that value is
    * send to the child process as a signal, returning a negative value sends the
    * SIGKILL signal to the child process.<br/> The default is to return 0 from both
    * 'callback' and 'timeout' to continue the child process until it finishes.
    */
  int shell_command(
      char** argv,
      const std::function<int(int,std::string)>& callback = shell_command_default_callback,
      const std::function<int()>& timeout = shell_command_default_timeout,
      int timeout_ms = shell_command_default_timeout_ms);

  /** @brief Execute a (single) shell-command using fork()/execvp().
    * @param args
    * The command to execute where the fist entry in the vector is the
    * command to execute with each following entry an argument to that command.
    * @param callback
    * A non-blocking function that is called once for every line of output from the child process.<br/>
    * This function shall take two arguments (an integer r-value and a std::string r-value) and
    * must return an integer value.<br/>
    * The integer value passed to the function has one of the following values:
    * <ul>
    * <li>-2 if the child process was terminated by a signal, the string contains an error message.</li>
    * <li>-1 if the call to execvp() has failed, the string contains an error message.
    * <li>0 if the string originated from STDOUT of the child process.</li>
    * <li>1 if the string originated from STDERR of the child process.</li>
    * </ul>
    * @param timeout
    * A non-blocking function that is called every 'timeout_ms' milliseconds while the
    * child process is running, it takes no arguments and must return an integer.
    * @param timeout_ms
    * The amount of time (in milliseconds) to wait before calling 'timeout'.
    * @returns The exit value of the child process.
    * @note Please use full paths to execute a shell command (ie. do not
    * rely on the PATH env) and sanitize the command arguments if influenced
    * by user input.
    * @note If 'callback' was called with a negative integer value then its return value
    * is ignored (because the child process has allready exited).
    * @note If either 'callback' or 'timeout' returns a positive value then that value is
    * send to the child process as a signal, returning a negative value sends the
    * SIGKILL signal to the child process.<br/> The default is to return 0 from both
    * 'callback' and 'timeout' to continue the child process until it finishes.
    */
  int shell_command(
      std::vector<std::string> args,
      const std::function<int(int,std::string)>& callback = shell_command_default_callback,
      const std::function<int()>& timeout = shell_command_default_timeout,
      int timeout_ms = shell_command_default_timeout_ms);

  /** @brief Execute a (single) shell-command using fork()/execvp().
    * @param command
    * The command to execute.
    * @param callback
    * A non-blocking function that is called once for every line of output from the child process.<br/>
    * This function shall take two arguments (an integer r-value and a std::string r-value) and
    * must return an integer value.<br/>
    * The integer value passed to the function has one of the following values:
    * <ul>
    * <li>-2 if the child process was terminated by a signal, the string contains an error message.</li>
    * <li>-1 if the call to execvp() has failed, the string contains an error message.
    * <li>0 if the string originated from STDOUT of the child process.</li>
    * <li>1 if the string originated from STDERR of the child process.</li>
    * </ul>
    * @param timeout
    * A non-blocking function that is called every 'timeout_ms' milliseconds while the
    * child process is running, it takes no arguments and must return an integer.
    * @param timeout_ms
    * The amount of time (in milliseconds) to wait before calling 'timeout'.
    * @returns The exit value of the child process.
    * @note Please use full paths to execute a shell command (ie. do not
    * rely on the PATH env) and sanitize the command arguments if influenced
    * by user input.
    * @note If 'callback' was called with a negative integer value then its return value
    * is ignored (because the child process has allready exited).
    * @note If either 'callback' or 'timeout' returns a positive value then that value is
    * send to the child process as a signal, returning a negative value sends the
    * SIGKILL signal to the child process.<br/> The default is to return 0 from both
    * 'callback' and 'timeout' to continue the child process until it finishes.
    */
  inline int shell_command(
      const std::string& command,
      const std::function<int(int,std::string)>& callback = shell_command_default_callback,
      const std::function<int()>& timeout = shell_command_default_timeout,
      int timeout_ms = shell_command_default_timeout_ms) {
    return shell_command(
        std::forward<std::vector<std::string>>(
            xxx::shell_command_to_vector(command)),
        callback, timeout, timeout_ms);
  }

  /** @brief Execute a (single) shell-command using fork()/execvp().
    * @param il
    * The command to execute where the first entry in the initializer_list is the
    * command to execute with each following entry an argument to that command.
    * @param callback
    * A non-blocking function that is called once for every line of output from the child process.<br/>
    * This function shall take two arguments (an integer r-value and a std::string r-value) and
    * must return an integer value.<br/>
    * The integer value passed to the function has one of the following values:
    * <ul>
    * <li>-2 if the child process was terminated by a signal, the string contains an error message.</li>
    * <li>-1 if the call to execvp() has failed, the string contains an error message.
    * <li>0 if the string originated from STDOUT of the child process.</li>
    * <li>1 if the string originated from STDERR of the child process.</li>
    * </ul>
    * @param timeout
    * A non-blocking function that is called every 'timeout_ms' milliseconds while the
    * child process is running, it takes no arguments and must return an integer.
    * @param timeout_ms
    * The amount of time (in milliseconds) to wait before calling 'timeout'.
    * @returns The exit value of the child process.
    * @note Please use full paths to execute a shell command (ie. do not
    * rely on the PATH env) and sanitize the command arguments if influenced
    * by user input.
    * @note If 'callback' was called with a negative integer value then its return value
    * is ignored (because the child process has allready exited).
    * @note If either 'callback' or 'timeout' returns a positive value then that value is
    * send to the child process as a signal, returning a negative value sends the
    * SIGKILL signal to the child process.<br/> The default is to return 0 from both
    * 'callback' and 'timeout' to continue the child process until it finishes.
    */
  inline int shell_command(
      std::initializer_list<std::string>&& il,
      const std::function<int(int,std::string)>& callback = shell_command_default_callback,
      const std::function<int()>& timeout = shell_command_default_timeout,
      int timeout_ms = shell_command_default_timeout_ms) {
    return shell_command(
        std::vector<std::string>(
            std::forward<std::initializer_list<std::string>>(il)),
        callback, timeout, timeout_ms);
  }

} /* ends namespace xxx */

#endif
