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
  * @file src/libcommon/Shell.cpp
  * @brief Execute shell-commands using fork()/execvp().
  */
#include <csignal>
#include <iostream>
#include <cstring>
#include <chrono>
#include <cstdio>
#include <fcntl.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include "Shell.hpp"

void xxx::shell_safety_test(const char* argv0) {
  struct stat self;
  if (stat(argv0, &self)) {
    std::ostringstream oss;
    oss << "Could not get file permissions for '" <<  argv0 << "', "
        "please run this application as root.";
    throw std::runtime_error(oss.str());
  }
  if (self.st_mode & S_ISUID) {
    throw std::runtime_error(
        "This application should never be started SUID, this is a security risc!\n"
        "Please remove the set-user-ID bit from the executable file with "
        "'chmod u-s'"
    );
  }
  if (self.st_mode & S_ISGID) {
    throw std::runtime_error(
        "This application should never be started SGID, this is a security risc!\n"
        "Please remove the set-group-ID bit from the executable file with "
        "'chmod g-s'"
    );
  }
}

std::vector<std::string> xxx::shell_command_to_vector(const std::string& cmd) {
  std::vector<std::string> out;
  bool s_quote = false;
  bool d_quote = false;
  std::string s;
  for (char c : cmd) {
    if (s_quote) {
      if (c == '\'') s_quote = false;
      s.push_back(c);
    }
    else if (d_quote) {
      if (c == '"') d_quote = false;
      s.push_back(c);
    }
    else if (std::isspace(c)) {
      if (!s.empty()) {
        out.push_back(std::move(s));
        s.clear();
      }
      continue;
    }
    else if (c == '\'') {
      if (!d_quote) s_quote = true;
      s.push_back(c);
    }
    else if (c == '"') {
      if (!s_quote) d_quote = true;
      s.push_back(c);
    }
    else {
      s.push_back(c);
    }
  }
  if (!s.empty()) out.push_back(std::move(s));
  return out;
}

int xxx::shell_command(
    char** argv,
    const std::function<int(int,std::string)>& callback,
    const std::function<int()>& timeout,
    int timeout_ms) {

  /* Sanitize arguments */
  if (argv == nullptr) return -1;
  if (timeout_ms <= 0) timeout_ms = shell_command_default_timeout_ms;

  /* Create a non-blocking pipe to capture STDOUT of the child process.
   * p_stdout[0] is the file descriptor for one end of the pipe,
   * p_stdout[1] is the file descriptor for the other end of the pipe. */
  int p_stdout[2];
  if (pipe2(p_stdout, O_NONBLOCK)) return -1;

  /* Create another pipe to capture STDERR of the child process. */
  int p_stderr[2];
  if (pipe2(p_stderr, O_NONBLOCK)) {
    close(p_stdout[0]);
    close(p_stdout[1]);
    return -1;
  }

  /* Create yet another pipe that we will use to detect if execvp() failed.
   * Note that reading this pipe will block until it closes automaticly when
   * the call to execvp() is successfull (O_CLOEXEC). */
  int p_detect[2];
  if (pipe2(p_detect, O_CLOEXEC)) {
    close(p_stdout[0]);
    close(p_stdout[1]);
    close(p_stderr[0]);
    close(p_stderr[1]);
    return -1;
  }

  /* Fork a new process.
   * Both parent and child process start out with both ends of the pipes.
   * The parent process shall use the p[0] endpoints and must close p[1],
   * the child process uses the p[1] endpoints and must close p[0]. */
  pid_t pid = fork();
  switch (pid) {

    case -1:
      /* Error, close both ends of all pipes and return with error status. */
      close(p_stdout[0]);
      close(p_stdout[1]);
      close(p_stderr[0]);
      close(p_stderr[1]);
      close(p_detect[0]);
      close(p_detect[1]);
      return -1;

    case 0:
      /* This is the child process, close the other end of the pipes first. */
      close(p_stdout[0]);
      close(p_stderr[0]);
      close(p_detect[0]);
      /* Connect STDOUT and STDERR of the child process to our end of the pipes. */
      dup2(p_stdout[1], STDOUT_FILENO);
      dup2(p_stderr[1], STDERR_FILENO);
      /* Execute the program (then exit the child process). */
      execvp(*argv, argv);
      /* Error executing execvp();
       * Inform the parent process by writing the errno to
       * the detection pipe */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result" 
      write(p_detect[1], &errno, sizeof(int));
#pragma GCC diagnostic pop
      /* Terminate this process 'immediately'
       * (Closes all pipe endpoints on this end.) */
      _exit(-1);

    default: {
      /* This is the parent process, close the other ends of the pipes first */
      close(p_stdout[1]);
      close(p_stderr[1]);
      close(p_detect[1]);

      /* Read the execvp() error detection pipe (blocks!) */
      ssize_t count;
      int err;
      while ((count = read(p_detect[0], &err, sizeof(int))) == -1) {
        if (errno != EAGAIN && errno != EINTR) break;
      }
      if (count) {
        std::string s("execvp: ");
        s += strerror(err);
        s.push_back('\n');
        callback(-1, std::move(s));
        close(p_stdout[0]);
        close(p_stderr[0]);
        close(p_detect[0]);
        return -1;
      }
      close(p_detect[0]);

      break;
    }
  }  

  /* Read characters from our ends of the pipes (non-blocking)
   * until they are closed (by the child process).
   * When a complete line was read call the 'callback' function.
   * Sleep while no data is received and call the 'timeout' function periodicly. */
  std::string _stdout;
  std::string _stderr;
  bool do_break_stdout = false;
  bool do_break_stderr = false;
  bool do_sleep_stdout = false;
  bool do_sleep_stderr = false;
  bool have_exit_status = false;
  int exit_status = 0;
  while (true) {
    char c;
    ssize_t r;
    /* Read the STDOUT pipe */
    if (!do_break_stdout) {
      r = read(p_stdout[0], &c, 1);
      if (r == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
        /* No data yet, sleep and call the 'timeout' function. */
        do_sleep_stdout = true;
      }
      else {
        if (r > 0) {
          /* Received data, append it to the output string. */
          _stdout.push_back(c);
          /* EOL? */
          if (c == '\n') {
            /*  Yes, pass the string to the 'callback' function */
            int action = callback(0, std::move(_stdout));
            _stdout.clear();
            /* Send a signal to the child process? */
            if (action && !have_exit_status) {
              if (action < 0) action = SIGKILL;
              kill(pid, action);
            }
          }
          do_sleep_stdout = false;
        }
        else {
          /* Pipe closed, call the 'callback' function
           * if there is unprocessed data available, then
           * break out of the while loop. */
          if (!_stdout.empty()) {
            int action = callback(0, std::move(_stdout));
            /* Send a signal to the child process? */
            if (action && !have_exit_status) {
              if (action < 0) action = SIGKILL;
              kill(pid, action);
            }
          }
          do_break_stdout = true;
        }
      }
    }

    /* Read the STDERR pipe */
    if (!do_break_stderr) {
      r = read(p_stderr[0], &c, 1);
      if (r == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
        /* No data yet, sleep and call the 'timeout' function. */
        do_sleep_stderr = true;
      }
      else {
        if (r > 0) {
          /* Received data, append it to the output string. */
          _stderr.push_back(c);
          /* EOL? */
          if (c == '\n') {
            /*  Yes, pass the string to the 'callback' function */
            int action = callback(1, std::move(_stderr));
            _stderr.clear();
            /* Send a signal to the child process? */
            if (action && !have_exit_status) {
              if (action < 0) action = SIGKILL;
              kill(pid, action);
            }
          }
          do_sleep_stderr = false;
        }
        else {
          /* Pipe closed, call the 'callback' function
           * if there is unprocessed data available, then
           * break out of the while loop. */
          if (!_stderr.empty()) {
            int action = callback(1, std::move(_stderr));
            /* Send a signal to the child process? */
            if (action && !have_exit_status) {
              if (action < 0) action = SIGKILL;
              kill(pid, action);
            }
          }
          do_break_stderr = true;
        }
      }
    }

    /* Break out of the while loop when both pipes have been
     * closed by the child process. */
    if (do_break_stderr && do_break_stdout) break;

    /* Sleep while there is no data available on either pipe.
     * Call the 'timeout' function every 'timeout_ms' milliseconds. */
    if (do_sleep_stdout && do_sleep_stderr) {
      using namespace std::chrono;
      do_sleep_stdout = false;
      do_sleep_stderr = false;
      auto tp = high_resolution_clock::now();
      while (true) {
        /* child still alive? */
        pid_t result = waitpid(pid, &exit_status, WNOHANG);
        if (result != 0) {
          /* Child has exited or error calling waitpid, do not sleep any longer */
          have_exit_status = true;
          break;
        }
        /* Child still alive, continue sleeping untill timeout_ms have passed */
        duration<int, std::milli> sleep_duration =
            duration_cast<duration<int, std::milli>>(
                high_resolution_clock::now() - tp);
        if (sleep_duration.count() >= timeout_ms) break;
        std::this_thread::sleep_for(microseconds(1));
      }
      /* Call the timeout function (at least once). */
      int action = timeout();
      /* Send a signal to the child process? */
      if (action && !have_exit_status) {
        if (action < 0) action = SIGKILL;
        kill(pid, action);
      }
    }

    /* ends while (true) loop */
  }

  /* Close our end of the pipe */
  close(p_stdout[0]);
  close(p_stderr[0]);

  /* Retrieve the exit status from the child process. */
  if (!have_exit_status) waitpid(pid, &exit_status, 0);

  /* Was the child process killed by a signal? */
  if (WIFSIGNALED(exit_status)) {
    std::string s("fork: child terminated by signal ");
    s += std::move(std::to_string(WTERMSIG(exit_status)));
    callback(-2, std::move(s));
    return -1;
  }

  /* The actual value returned by WEXITSTATUS is only 8 bits wide,
   * let the compiler know so that it will extend the sign bit. */
  return static_cast<int8_t>(WEXITSTATUS(exit_status));
}

int xxx::shell_command(
    std::vector<std::string> args,
    const std::function<int(int,std::string)>& callback,
    const std::function<int()>& timeout,
    int timeout_ms) {
  if (!args.empty()) {
    char* argv[args.size() + 1];
    char** p = argv;
    for (auto& s : args) *(p++) = &*s.begin();
    *p = nullptr;
    return shell_command(argv, callback, timeout, timeout_ms);
  }
  return -1;
}

int xxx::shell_command_default_callback(int type, std::string s) {
  if (type == 0) {
    std::cout << s;
  }
  else {
    std::cerr << s;
  }
  return 0;
}

int xxx::shell_command_default_timeout() {
  return 0;
}

