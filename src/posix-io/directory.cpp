/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2015 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <cmsis-plus/posix-io/directory.h>
#include <cmsis-plus/posix-io/file-system.h>
#include <cmsis-plus/posix-io/mount-manager.h>
#include <cmsis-plus/posix-io/pool.h>
#include <cerrno>
#include <cassert>

// ----------------------------------------------------------------------------

namespace os
{
  namespace posix
  {
    // ------------------------------------------------------------------------

    directory*
    opendir (const char* dirname)
    {
      if (dirname == nullptr)
        {
          errno = EFAULT;
          return nullptr;
        }

      if (*dirname == '\0')
        {
          errno = ENOENT;
          return nullptr;
        }

      errno = 0;

      auto adjusted_dirname = dirname;
      auto* const fs = os::posix::mount_manager::identify_file_system (
          &adjusted_dirname);

      // The manager will return null if there are no file systems
      // registered, no need to check this condition separately.
      if (fs == nullptr)
        {
          errno = EBADF;
          return nullptr;
        }

      // Use the file system implementation to open the directory, using
      // the adjusted path (mount point prefix removed).
      return fs->opendir (adjusted_dirname);
    }

    // ------------------------------------------------------------------------

    directory::directory (void)
    {
      file_system_ = nullptr;
    }

    directory::~directory ()
    {
      file_system_ = nullptr;
    }

    // ------------------------------------------------------------------------

    struct dirent*
    directory::read (void)
    {
      assert (file_system_ != nullptr);
      errno = 0;

      // Execute the implementation specific code.
      return do_read ();
    }

    void
    directory::rewind (void)
    {
      assert (file_system_ != nullptr);
      errno = 0;

      // Execute the implementation specific code.
      do_rewind ();
    }

    int
    directory::close (void)
    {
      assert (file_system_ != nullptr);
      errno = 0;

      // Execute the implementation specific code.
      int ret = do_close ();
      auto* const pool = file_system_->dirs_pool ();
      if (pool != nullptr)
        {
          pool->release (this);
        }
      return ret;
    }

    // ------------------------------------------------------------------------
    // Default implementations; overwrite them with real code.

    // do_vopen() is not here because it is an abstract virtual to be
    // implemented by derived classes.

    struct dirent*
    directory::do_read (void)
    {
      // Return end of directory.
      return nullptr;
    }

    void
    directory::do_rewind (void)
    {
      // Ignore rewind.
      return;
    }

    int
    directory::do_close (void)
    {
      // Ignore close, return ok.
      return 0;
    }

  } /* namespace posix */
} /* namespace os */

// ----------------------------------------------------------------------------
