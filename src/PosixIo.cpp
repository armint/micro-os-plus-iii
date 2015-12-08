/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2015 Liviu Ionescu.
 *
 * µOS++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * µOS++ is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "posix-io/FileDescriptorsManager.h"
#include "posix-io/PosixIo.h"
#include <cassert>
#include <cerrno>
#include <cstdarg>

namespace os
{

  // --------------------------------------------------------------------------

  PosixIo::PosixIo ()
  {
    fFileDescriptor = noFileDescriptor;
  }

  PosixIo::~PosixIo ()
  {
    ;
  }

  // --------------------------------------------------------------------------

  int
  PosixIo::open (const char *path, int oflag, ...)
  {
    int ret;

    errno = 0;

    // Execute the implementation specific code.
    va_list args;
    va_start(args, oflag);
    ret = doOpen (path, oflag, args);
    va_end(args);

    if (ret == 0)
      {
        // If successful, allocate a file descriptor
        ret = FileDescriptorsManager::allocFileDescriptor (this);
        if (ret == -1)
          {
            // If allocation failed, close it.
            doClose ();
            fFileDescriptor = noFileDescriptor;
          }
      }
    return ret;
  }

  int
  PosixIo::close (void)
  {
    int ret;

    errno = 0;

    // Execute the implementation specific code.
    ret = doClose ();

    // Remove this IO from the file descriptors registry.
    FileDescriptorsManager::freeFileDescriptor (fFileDescriptor);
    fFileDescriptor = noFileDescriptor;

    return ret;
  }

  // ----------------------------------------------------------------------------

  // All these wrappers are required just to clear 'errno'.

  ssize_t
  PosixIo::read (void *buf, size_t nbyte)
  {
    errno = 0;

    // Execute the implementation specific code.
    return doRead (buf, nbyte);
  }

  ssize_t
  PosixIo::write (const void *buf, size_t nbyte)
  {
    errno = 0;

    // Execute the implementation specific code.
    return doWrite (buf, nbyte);
  }

  int
  PosixIo::ioctl (int request, ...)
  {
    int ret;

    errno = 0;

    // Execute the implementation specific code.
    va_list args;
    va_start(args, request);
    ret = doIoctl (request, args);
    va_end(args);

    return ret;
  }

  // ----------------------------------------------------------------------------

  int
  PosixIo::doClose (void)
  {
    errno = ENOSYS; // Not implemented
    return -1;
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

  ssize_t
  PosixIo::doRead (void *buf, size_t nbyte)
  {
    errno = ENOSYS; // Not implemented
    return -1;
  }

  ssize_t
  PosixIo::doWrite (const void *buf, size_t nbyte)
  {
    errno = ENOSYS; // Not implemented
    return -1;
  }

  int
  PosixIo::doIoctl (int request, ...)
  {
    errno = ENOSYS; // Not implemented
    return -1;
  }

#pragma GCC diagnostic pop

} // namespace os

// ----------------------------------------------------------------------------
