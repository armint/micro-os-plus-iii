/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2016 Liviu Ionescu.
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

#include <cmsis-plus/std/system_error>
#include <cmsis-plus/diag/trace.h>
#include <cmsis-plus/rtos/os.h>

#include <cstdlib>
#include <cstring>
#if defined(__EXCEPTIONS)
#include <string>
#endif

// ----------------------------------------------------------------------------

using namespace os::cmsis;

namespace os
{
  namespace cmsis
  {
    namespace std
    {
      // ======================================================================

#if defined(__EXCEPTIONS)

      struct system_error_category : public ::std::error_category
      {
        virtual const char*
        name () const noexcept
        {
          return "system";
        }

        virtual ::std::string
        message (int i) const
        {
          return ::std::string (strerror (i));
        }
      };

      struct cmsis_error_category : public ::std::error_category
      {
        virtual const char*
        name () const noexcept
        {
          return "cmsis";
        }

        virtual ::std::string
        message (int i) const
        {
          return ::std::string (rtos::kernel::strerror ((rtos::return_t) i));
        }
      };

#endif

      void
      __throw_system_error (int ev, const char* what_arg)
      {
#if defined(__EXCEPTIONS)
        throw ::std::system_error (
            ::std::error_code (ev, system_error_category ()), what_arg);
#else
        trace_printf ("system_error(%d, %s)\n", ev, what_arg);
        ::std::abort ();
#endif
      }

      void
      __throw_cmsis_error (int ev, const char* what_arg)
      {
#if defined(__EXCEPTIONS)
        throw ::std::system_error (
            ::std::error_code (ev, cmsis_error_category ()), what_arg);
#else
        trace_printf ("system_error(%d, %s)\n", ev, what_arg);
        ::std::abort ();
#endif
      }

    } /* namespace std */
  } /* namespace cmsis */
} /* namespace os */

// ----------------------------------------------------------------------------