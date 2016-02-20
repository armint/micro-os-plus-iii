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

/*
 * This file is part of the CMSIS++ proposal, intended as a CMSIS
 * replacement for C++ applications.
 *
 * The code provides an implementation of the C API for the CMSIS++
 * and legacy CMSIS, using the CMSIC++ RTOS definitions.
 */

#include <cmsis-plus/rtos/os.h>
#include <cmsis-plus/rtos/os-c-api.h>
#include <cmsis-plus/diag/trace.h>

// Note: The legacy header is included later, to avoid polluting the namespace.

#include <cassert>
#include <cerrno>
#include <cstring>
#include <new>

// ----------------------------------------------------------------------------

using namespace os::rtos;

// ----------------------------------------------------------------------------

// Validate C structs sizes (should match the C++ objects sizes).
// TODO: validate individual members (size & offset).

static_assert(sizeof(Thread) == sizeof(os_thread_t), "adjust os_thread_t size");
static_assert(sizeof(thread::Attributes) == sizeof(os_thread_attr_t), "adjust os_thread_attr_t size");

static_assert(sizeof(Timer) == sizeof(os_timer_t), "adjust size of os_timer_t");
static_assert(sizeof(timer::Attributes) == sizeof(os_timer_attr_t), "adjust size of os_timer_attr_t");

static_assert(sizeof(Mutex) == sizeof(os_mutex_t), "adjust size of os_mutex_t");
static_assert(sizeof(mutex::Attributes) == sizeof(os_mutex_attr_t), "adjust size of os_mutex_attr_t");

static_assert(sizeof(Condition_variable) == sizeof(os_condvar_t), "adjust size of os_condvar_t");
static_assert(sizeof(condvar::Attributes) == sizeof(os_condvar_attr_t), "adjust size of os_condvar_attr_t");

static_assert(sizeof(Semaphore) == sizeof(os_semaphore_t), "adjust size of os_semaphore_t");
static_assert(sizeof(semaphore::Attributes) == sizeof(os_semaphore_attr_t), "adjust size of os_semaphore_attr_t");

static_assert(sizeof(Memory_pool) == sizeof(os_mempool_t), "adjust size of os_mempool_t");
static_assert(sizeof(mempool::Attributes) == sizeof(os_mempool_attr_t), "adjust size of os_mempool_attr_t");

static_assert(sizeof(Message_queue) == sizeof(os_mqueue_t), "adjust size of os_mqueue_t");
static_assert(sizeof(mqueue::Attributes) == sizeof(os_mqueue_attr_t), "adjust size of os_mqueue_attr_t");

// ----------------------------------------------------------------------------

os_result_t
os_sched_initialize (void)
{
  return static_cast<os_result_t> (scheduler::initialize ());
}

os_result_t
os_sched_start (void)
{
  return static_cast<os_result_t> (scheduler::start ());
}

bool
os_sched_is_started (void)
{
  return scheduler::is_started ();
}

// ----------------------------------------------------------------------------

void
os_thread_create (os_thread_t* thread, const os_thread_attr_t* attr,
                  os_thread_func_t func, const os_thread_func_args_t args)
{
  new (thread) Thread ((thread::Attributes&) *attr, (thread::func_t) func,
                       (thread::func_args_t) args);
}

void
os_thread_destroy (os_thread_t* thread)
{
  (reinterpret_cast<Thread&> (thread)).~Thread ();
}

void
os_thread_exit (os_thread_t* thread, void* exit_ptr)
{
  (reinterpret_cast<Thread&> (thread)).exit (exit_ptr);
}

os_result_t
os_thread_join (os_thread_t* thread, void** exit_ptr)
{
  return (reinterpret_cast<Thread&> (thread)).join (exit_ptr);
}

os_thread_prio_t
os_thread_get_prio (os_thread_t* thread)
{
  return (reinterpret_cast<Thread&> (thread)).sched_prio ();
}

os_result_t
os_thread_set_prio (os_thread_t* thread, os_thread_prio_t prio)
{
  return (reinterpret_cast<Thread&> (thread)).sched_prio (prio);
}

void
os_thread_wakeup (os_thread_t* thread)
{
  return (reinterpret_cast<Thread&> (thread)).wakeup ();
}

// ----------------------------------------------------------------------------

void
os_mutex_create (os_mutex_t* mutex, const os_mutex_attr_t* attr)
{
  new (mutex) Mutex ((mutex::Attributes&) *attr);
}

void
os_mutex_destroy (os_mutex_t* mutex)
{
  (reinterpret_cast<Mutex&> (mutex)).~Mutex ();
}

os_result_t
os_mutex_lock (os_mutex_t* mutex)
{
  return (os_result_t) (reinterpret_cast<Mutex&> (mutex)).lock ();
}

os_result_t
os_mutex_try_lock (os_mutex_t* mutex)
{
  return (os_result_t) (reinterpret_cast<Mutex&> (mutex)).try_lock ();
}

os_result_t
os_mutex_timed_lock (os_mutex_t* mutex, os_systicks_t ticks)
{
  return (os_result_t) (reinterpret_cast<Mutex&> (mutex)).timed_lock (ticks);
}

os_result_t
os_mutex_unlock (os_mutex_t* mutex)
{
  return (os_result_t) (reinterpret_cast<Mutex&> (mutex)).unlock ();
}

os_thread_prio_t
os_get_mutex_prio_ceiling (os_mutex_t* mutex)
{
  return (os_thread_prio_t) (reinterpret_cast<Mutex&> (mutex)).prio_ceiling ();
}

os_result_t
os_set_mutex_prio_ceiling (os_mutex_t* mutex, os_thread_prio_t prio_ceiling,
                           os_thread_prio_t* old_prio_ceiling)
{
  return (os_result_t) (reinterpret_cast<Mutex&> (mutex)).prio_ceiling (
      prio_ceiling, old_prio_ceiling);
}

// ----------------------------------------------------------------------------

void
os_condvar_create (os_condvar_t* condvar, os_condvar_attr_t* attr)
{
  new (condvar) Condition_variable ((condvar::Attributes&) *attr);
}

void
os_condvar_destroy (os_condvar_t* condvar)
{
  (reinterpret_cast<Condition_variable&> (condvar)).~Condition_variable ();
}

os_result_t
os_condvar_signal (os_condvar_t* condvar)
{
  return (os_result_t) (reinterpret_cast<Condition_variable&> (condvar)).signal ();
}

os_result_t
os_condvar_broadcast (os_condvar_t* condvar)
{
  return (os_result_t) (reinterpret_cast<Condition_variable&> (condvar)).broadcast ();
}

os_result_t
os_condvar_wait (os_condvar_t* condvar, os_mutex_t* mutex)
{
  return (os_result_t) (reinterpret_cast<Condition_variable&> (condvar)).wait (
      (Mutex&) *mutex);
}

os_result_t
os_condvar_timed_wait (os_condvar_t* condvar, os_mutex_t* mutex,
                       os_systicks_t ticks)
{
  return (os_result_t) (reinterpret_cast<Condition_variable&> (condvar)).timed_wait (
      (Mutex&) *mutex, ticks);
}

// ----------------------------------------------------------------------------

void
os_semaphore_create (os_semaphore_t* semaphore, os_semaphore_attr_t* attr)
{
  new (semaphore) Semaphore ((semaphore::Attributes&) *attr);
}

void
os_semaphore_destroy (os_semaphore_t* semaphore)
{
  (reinterpret_cast<Semaphore&> (semaphore)).~Semaphore ();
}

os_result_t
os_semaphore_post (os_semaphore_t* semaphore)
{
  return (os_result_t) (reinterpret_cast<Semaphore&> (semaphore)).post ();
}

os_result_t
os_semaphore_wait (os_semaphore_t* semaphore)
{
  return (os_result_t) (reinterpret_cast<Semaphore&> (semaphore)).wait ();
}

os_result_t
os_semaphore_try_wait (os_semaphore_t* semaphore)
{
  return (os_result_t) (reinterpret_cast<Semaphore&> (semaphore)).try_wait ();
}

os_result_t
os_semaphore_timed_wait (os_semaphore_t* semaphore, os_systicks_t ticks)
{
  return (os_result_t) (reinterpret_cast<Semaphore&> (semaphore)).timed_wait (
      ticks);
}

os_semaphore_count_t
os_semaphore_get_value (os_semaphore_t* semaphore)
{
  return (os_semaphore_count_t) (reinterpret_cast<Semaphore&> (semaphore)).value ();
}

os_result_t
os_semaphore_reset (os_semaphore_t* semaphore)
{
  return (os_result_t) (reinterpret_cast<Semaphore&> (semaphore)).reset ();
}

// ----------------------------------------------------------------------------

void
os_mempool_create (os_mempool_t* mempool, os_mempool_attr_t* attr,
                   os_mempool_size_t blocks, os_mempool_size_t block_size_bytes)
{
  new (mempool) Memory_pool ((mempool::Attributes&) *attr, blocks,
                             block_size_bytes);
}

void
os_mempool_destroy (os_mempool_t* mempool)
{
  (reinterpret_cast<Memory_pool&> (mempool)).~Memory_pool ();
}

void*
os_mempool_alloc (os_mempool_t* mempool)
{
  return (reinterpret_cast<Memory_pool&> (mempool)).alloc ();
}

void*
os_mempool_try_alloc (os_mempool_t* mempool)
{
  return (reinterpret_cast<Memory_pool&> (mempool)).try_alloc ();
}

void*
os_mempool_timed_alloc (os_mempool_t* mempool, os_systicks_t ticks)
{
  return (reinterpret_cast<Memory_pool&> (mempool)).timed_alloc (ticks);
}

os_result_t
os_mempool_free (os_mempool_t* mempool, void* block)
{
  return (os_result_t) (reinterpret_cast<Memory_pool&> (mempool)).free (block);
}

size_t
os_mempool_get_size (os_mempool_t* mempool)
{
  return (reinterpret_cast<Memory_pool&> (mempool)).size ();
}

size_t
os_mempool_get_count (os_mempool_t* mempool)
{
  return (reinterpret_cast<Memory_pool&> (mempool)).count ();
}

size_t
os_mempool_get_block_size (os_mempool_t* mempool)
{
  return (reinterpret_cast<Memory_pool&> (mempool)).block_size ();
}

bool
os_mempool_is_empty (os_mempool_t* mempool)
{
  return (reinterpret_cast<Memory_pool&> (mempool)).is_empty ();
}

bool
os_mempool_is_full (os_mempool_t* mempool)
{
  return (reinterpret_cast<Memory_pool&> (mempool)).is_full ();
}

os_result_t
os_mempool_reset (os_mempool_t* mempool)
{
  return (os_result_t) (reinterpret_cast<Memory_pool&> (mempool)).reset ();
}

// --------------------------------------------------------------------------

void
os_mqueue_create (os_mqueue_t* mqueue, os_mqueue_attr_t* attr,
                  os_mqueue_size_t msgs, os_mqueue_size_t msg_size_bytes)
{
  new (mqueue) Message_queue ((mqueue::Attributes&) *attr, msgs,
                              msg_size_bytes);
}

void
os_mqueue_destroy (os_mqueue_t* mqueue)
{
  (reinterpret_cast<Message_queue&> (mqueue)).~Message_queue ();
}

os_result_t
os_mqueue_send (os_mqueue_t* mqueue, const char* msg, size_t nbytes,
                os_mqueue_prio_t mprio)
{
  return (os_result_t) (reinterpret_cast<Message_queue&> (mqueue)).send (msg,
                                                                         nbytes,
                                                                         mprio);
}

os_result_t
os_mqueue_try_send (os_mqueue_t* mqueue, const char* msg, size_t nbytes,
                    os_mqueue_prio_t mprio)
{
  return (os_result_t) (reinterpret_cast<Message_queue&> (mqueue)).try_send (
      msg, nbytes, mprio);
}

os_result_t
os_mqueue_timed_send (os_mqueue_t* mqueue, const char* msg, size_t nbytes,
                      os_mqueue_prio_t mprio, os_systicks_t ticks)
{
  return (os_result_t) (reinterpret_cast<Message_queue&> (mqueue)).timed_send (
      msg, nbytes, mprio, ticks);
}

os_result_t
os_mqueue_receive (os_mqueue_t* mqueue, char* msg, size_t nbytes,
                   os_mqueue_prio_t* mprio)
{
  return (os_result_t) (reinterpret_cast<Message_queue&> (mqueue)).receive (
      msg, nbytes, mprio);
}

os_result_t
os_mqueue_try_receive (os_mqueue_t* mqueue, char* msg, size_t nbytes,
                       os_mqueue_prio_t* mprio)
{
  return (os_result_t) (reinterpret_cast<Message_queue&> (mqueue)).try_receive (
      msg, nbytes, mprio);
}

os_result_t
os_mqueue_timed_receive (os_mqueue_t* mqueue, char* msg, size_t nbytes,
                         os_mqueue_prio_t* mprio, os_systicks_t ticks)
{
  return (os_result_t) (reinterpret_cast<Message_queue&> (mqueue)).timed_receive (
      msg, nbytes, mprio, ticks);
}

size_t
os_mqueue_get_length (os_mqueue_t* mqueue)
{
  return (reinterpret_cast<Message_queue&> (mqueue)).length ();
}

size_t
os_mqueue_get_size (os_mqueue_t* mqueue)
{
  return (reinterpret_cast<Message_queue&> (mqueue)).size ();
}

size_t
os_mqueue_get_msg_size (os_mqueue_t* mqueue)
{
  return (reinterpret_cast<Message_queue&> (mqueue)).msg_size ();
}

bool
os_mqueue_get_is_empty (os_mqueue_t* mqueue)
{
  return (reinterpret_cast<Message_queue&> (mqueue)).is_empty ();
}

bool
os_mqueue_get_is_full (os_mqueue_t* mqueue)
{
  return (reinterpret_cast<Message_queue&> (mqueue)).is_full ();
}

os_result_t
os_mqueue_reset (os_mqueue_t* mqueue)
{
  return (os_result_t) (reinterpret_cast<Message_queue&> (mqueue)).reset ();
}

// ****************************************************************************
// ***** Legacy CMSIS RTOS implementation *****

#include <cmsis_os.h>

// ----------------------------------------------------------------------------
//  ==== Kernel Control Functions ====

/**
 * @details
 * Initialise of the RTOS Kernel to allow peripheral setup and creation
 * of other RTOS objects with the functions:
 * - osThreadCreate : Create a thread function.
 * - osTimerCreate : Define attributes of the timer callback function.
 * - osMutexCreate : Define and initialise a mutex.
 * - osSemaphoreCreate : Define and initialise a semaphore.
 * - osPoolCreate : Define and initialise a fix-size memory pool.
 * - osMessageCreate : Define and initialise a message queue.
 * - osMailCreate : Define and initialise a mail queue with fix-size memory blocks.
 *
 * The RTOS scheduler does not start thread switching until the function
 * osKernelStart is called.
 *
 * @note In case that the RTOS Kernel starts thread execution with the
 * function main the function osKernelInitialize stops thread
 * switching. This allows you to setup the system to a defined
 * state before thread switching is resumed with osKernelStart.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osKernelInitialize (void)
{
  if (scheduler::in_handler_mode ())
    {
      return osErrorISR;
    }
  scheduler::initialize ();
  return osOK;
}

/**
 * @details
 * Start the RTOS Kernel and begin thread switching.
 *
 * @note When the CMSIS-RTOS starts thread execution with the
 * function main this function resumes thread switching.
 * The main thread will continue executing after osKernelStart.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osKernelStart (void)
{
  return static_cast<osStatus> (scheduler::start ());
}

/**
 * @details
 * Identifies if the RTOS scheduler is started. For systems with the
 * option to start the main function as a thread this allows
 * you to identify that the RTOS scheduler is already running.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
int32_t
osKernelRunning (void)
{
  return scheduler::is_started () ? 1 : 0;
}

#if (defined (osFeature_SysTick)  &&  (osFeature_SysTick != 0))

/**
 * @details
 * Get the value of the Kernel SysTick timer for time comparison.
 * The value is a rolling 32-bit counter that is typically composed
 * of the kernel system interrupt timer value and an counter that
 * counts these interrupts.
 *
 * This function allows the implementation of timeout checks.
 * These are for example required when checking for a busy status
 * in a device or peripheral initialisation routine.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
uint32_t
osKernelSysTick (void)
{
  Systick_clock::current_t crt;
  Systick_clock::now (&crt);
  // Convert ticks to cycles.
  return static_cast<uint32_t> (crt.ticks) * crt.divisor + crt.cycles;
}

#endif    // System Timer available

// ----------------------------------------------------------------------------
//  ==== Thread Management ====

/**
 * @details
 * Start a thread function by adding it to the Active Threads list
 * and set it to state READY. The thread function receives the argument
 * pointer as function argument when the function is started. When the
 * priority of the created thread function is higher than the current
 * RUNNING thread, the created thread function starts instantly and
 * becomes the new RUNNING thread.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osThreadId
osThreadCreate (const osThreadDef_t* thread_def, void* args)
{
  Thread* thread = new (thread_def->data) Thread (
      (thread::func_t) thread_def->pthread, args);
  return reinterpret_cast<osThreadId> (thread);
}

/**
 * @details
 * Get the thread ID of the current running thread.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osThreadId
osThreadGetId (void)
{
  return reinterpret_cast<osThreadId> (&this_thread::thread ());
}

/**
 * @details
 * Remove the thread function from the active thread list. If the
 * thread is currently RUNNING the execution will stop.
 *
 * @note In case that osThreadTerminate terminates the currently
 * running task, the function never returns and other threads
 * that are in the READY state are started.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osThreadTerminate (osThreadId thread_id)
{
  // TODO: check if something needs to be done to terminate.
  (reinterpret_cast<Thread&> (thread_id)).~Thread ();
  return osOK;
}

/**
 * @details
 * Pass control to the next thread that is in state READY.
 * If there is no other thread in the state READY, the current
 * thread continues execution and no thread switching occurs.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osThreadYield (void)
{
  if (scheduler::in_handler_mode ())
    {
      return osErrorISR;
    }

  this_thread::yield ();

  return osOK;
}

/**
 * @details
 * Change the priority of a running thread.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osThreadSetPriority (osThreadId thread_id, osPriority priority)
{
  thread::priority_t prio = static_cast<thread::priority_t> (priority);
  return static_cast<osStatus> ((reinterpret_cast<Thread&> (thread_id)).sched_prio (
      prio));
}

/**
 * @details
 * Get the priority of an active thread. In case of a failure the value
 * osPriorityError is returned.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osPriority
osThreadGetPriority (osThreadId thread_id)
{
  thread::priority_t prio =
      (reinterpret_cast<Thread&> (thread_id)).sched_prio ();
  return static_cast<osPriority> (prio);
}

// ----------------------------------------------------------------------------
//  ==== Generic Wait Functions ====

/**
 * @details
 * Wait for a specified time period in _millisec_.
 *
 * The millisec value specifies the number of timer ticks and is therefore
 * an upper bound. The exact time delay depends on the actual time elapsed
 * since the last timer tick.
 *
 * For a value of 1, the system waits until the next timer tick occurs.
 * That means that the actual time delay may be up to one timer tick less.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osDelay (uint32_t millisec)
{
  return static_cast<osStatus> (Systick_clock::sleep_for (
      Systick_clock::ticks_cast (millisec * 1000u)));
}

#if (defined (osFeature_Wait)  &&  (osFeature_Wait != 0))

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waggregate-return"

/**
 * @details
 * Wait for any event of the type signal, message, mail for a specified
 * time period in millisec. While the system waits, the thread that is
 * calling this function is put into the state WAITING. When millisec
 * is set to osWaitForever, the function will wait for an infinite time
 * until an event occurs.
 *
 * The osWait function puts a thread into the state WAITING and waits
 * for any of the following events:
 *
 * - a signal sent to that thread explicitly
 * - a message from a message object that is registered to that thread
 * - a mail from a mail object that is registered to that thread
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osEvent
osWait (uint32_t millisec)
{
  osEvent event;

  result_t res = Systick_clock::sleep_for (
      Systick_clock::ticks_cast (millisec));
  // TODO: return events
  event.status = static_cast<osStatus> (res);
  return event;
}

#pragma GCC diagnostic pop

#endif  // Generic Wait available

// ----------------------------------------------------------------------------
//  ==== Timer Management Functions ====

/**
 * @details
 * Create a one-shot or periodic timer and associate it with a callback
 * function argument. The timer is in stopped until it is started with
 * osTimerStart.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osTimerId
osTimerCreate (const osTimerDef_t* timer_def, os_timer_type type, void* args)
{
  timer::Attributes attr
    { timer_def->name };
  attr.tm_type = (timer::type_t) type;

  return reinterpret_cast<osTimerId> (new ((void*) &timer_def->data) Timer (
      attr, (timer::func_t) timer_def->ptimer, (timer::func_args_t) args));
}

/**
 * @details
 * Start or restart the timer.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osTimerStart (osTimerId timer_id, uint32_t millisec)
{
  return static_cast<osStatus> ((reinterpret_cast<Timer&> (timer_id)).start (
      Systick_clock::ticks_cast (millisec)));
}

/**
 * @details
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osTimerStop (osTimerId timer_id)
{
  return static_cast<osStatus> ((reinterpret_cast<Timer&> (timer_id)).stop ());
}

/**
 * @details
 * Delete the timer object that was created by @ref osTimerCreate.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osTimerDelete (osTimerId timer_id)
{
  (reinterpret_cast<Timer&> (timer_id)).~Timer ();
  return osOK;
}

// ----------------------------------------------------------------------------
//  ==== Signal Management ====

/**
 * @details
 * Set the signal flags of an active thread.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
int32_t
osSignalSet (osThreadId thread_id, int32_t signals)
{
  int32_t ret;
  flags::set ((Thread&) (*thread_id), (event_flags_t) signals,
              (event_flags_t*) &ret);
  return ret;
}

/**
 * @details
 * Clear the signal flags of an active thread.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
int32_t
osSignalClear (osThreadId thread_id, int32_t signals)
{
  int32_t ret;
  flags::clear ((Thread&) (*thread_id), (event_flags_t) signals,
                (event_flags_t*) &ret);
  return ret;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waggregate-return"

/**
 * @details
 * Suspend the execution of the current RUNNING thread until all
 * specified signal flags with the parameter signals are set.
 * When the parameter signals is 0 the current RUNNING thread
 * is suspended until any signal is set. When these signal flags are
 * already set, the function returns instantly. Otherwise the thread
 * is put into the state WAITING. Signal flags that are reported as
 * event are automatically cleared.
 *
 * The argument millisec specifies how long the system waits for
 * the specified signal flags. While the system waits the tread
 * calling this function is put into the state WAITING. The timeout
 * value can have the following values:
 *
 * - when millisec is 0, the function returns instantly.
 * - when millisec is set to osWaitForever the function will wait
 * an infinite time until a specified signal is set.
 * - all other values specify a time in millisecond for a timeout.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osEvent
osSignalWait (int32_t signals, uint32_t millisec)
{
  osEvent event;
  result_t res;
  if (millisec == osWaitForever)
    {
      res = flags::wait ((event_flags_t) signals,
                         (event_flags_t*) &event.value.signals);
    }
  else if (millisec == 0)
    {
      res = flags::try_wait ((event_flags_t) signals,
                             (event_flags_t*) &event.value.signals);
    }
  else
    {
      res = flags::timed_wait ((event_flags_t) signals,
                               (event_flags_t*) &event.value.signals,
                               Systick_clock::ticks_cast (millisec * 1000u));
    }
  // TODO: set osEventSignal, osEventTimeout
  event.status = static_cast<osStatus> (res);
  return event;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

//  ==== Mutex Management ====

/**
 * @details
 * Create and initialize a mutex object.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osMutexId
osMutexCreate (const osMutexDef_t* mutex_def)
{
  return reinterpret_cast<osMutexId> (new ((void*) &mutex_def->data) Mutex ());
}

/**
 * @details
 * Wait until a Mutex becomes available. If no other thread has
 * obtained the Mutex, the function instantly returns and blocks
 * the mutex object.
 *
 * The argument millisec specifies how long the system waits for
 * a mutex. While the system waits the thread that is calling this
 * function is put into the state WAITING. The millisec timeout can
 * have the following values:
 *
 * - when millisec is 0, the function returns instantly.
 * - when millisec is set to osWaitForever the function will
 * wait for an infinite time until the mutex becomes available.
 * - all other values specify a time in millisecond for a timeout.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osMutexWait (osMutexId mutex_id, uint32_t millisec)
{
  result_t status;
  if (millisec == osWaitForever)
    {
      status = (reinterpret_cast<Mutex&> (mutex_id)).lock ();
    }
  else if (millisec == 0)
    {
      status = (reinterpret_cast<Mutex&> (mutex_id)).try_lock ();
    }
  else
    {
      status = (reinterpret_cast<Mutex&> (mutex_id)).timed_lock (
          Systick_clock::ticks_cast (millisec * 1000u));
    }

  // TODO: return legacy code for POSIX codes
  return static_cast<osStatus> (status);
}

/**
 * @details
 * Release a Mutex that was obtained with osMutexWait. Other
 * threads that currently wait for the same mutex will be now
 * put into the state READY.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osMutexRelease (osMutexId mutex_id)
{
  result_t status;
  status = (reinterpret_cast<Mutex&> (mutex_id)).unlock ();

  // TODO: return legacy code for POSIX codes
  return static_cast<osStatus> (status);
}

/**
 * @details
 * Delete a mutex object. The function releases internal memory
 * obtained for mutex handling. After this call the mutex_id is no
 * longer valid and cannot be used. The mutex may be created again
 * using the function osMutexCreate.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osMutexDelete (osMutexId mutex_id)
{
  (reinterpret_cast<Mutex&> (mutex_id)).~Mutex ();
  return osOK;
}

// ----------------------------------------------------------------------------

//  ==== Semaphore Management Functions ====

#if (defined (osFeature_Semaphore)  &&  (osFeature_Semaphore != 0))

/**
 * @details
 * Create and initialise a semaphore object that is used to manage
 * access to shared resources. The parameter count specifies
 * the number of available resources. The count value 1 creates
 * a binary semaphore.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osSemaphoreId
osSemaphoreCreate (const osSemaphoreDef_t* semaphore_def, int32_t count)
{
  semaphore::Attributes attr
    { semaphore_def->name };
  attr.sm_initial_count = (semaphore::count_t) count;
  return reinterpret_cast<osSemaphoreId> (new ((void*) &semaphore_def->data) Semaphore (
      attr));
}

/**
 * @details
 * Wait until a Semaphore token becomes available. When no Semaphore
 * token is available, the function waits for the time specified with
 * the parameter millisec.
 *
 * The argument millisec specifies how long the system waits for a
 * Semaphore token to become available. While the system waits the
 * thread that is calling this function is put into the state WAITING.
 * The millisec timeout can have the following values:
 *
 * - when millisec is 0, the function returns instantly.
 * - when millisec is set to osWaitForever the function will wait
 * for an infinite time until the Semaphore token becomes available.
 * - all other values specify a time in millisecond for a timeout.
 *
 * The return value indicates the number of available tokens (the
 * semaphore count value). If 0 is returned, then no semaphore was
 * available.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
int32_t
osSemaphoreWait (osSemaphoreId semaphore_id, uint32_t millisec)
{
  if (millisec == osWaitForever)
    {
      (reinterpret_cast<Semaphore&> (semaphore_id)).wait ();
    }
  else if (millisec == 0)
    {
      (reinterpret_cast<Semaphore&> (semaphore_id)).try_wait ();
    }
  else
    {
      (reinterpret_cast<Semaphore&> (semaphore_id)).timed_wait (
          Systick_clock::ticks_cast (millisec * 1000u));
    }

  return (int32_t) (reinterpret_cast<Semaphore&> (semaphore_id)).value ();
}

/**
 * @details
 * Release a Semaphore token. This increments the count of
 * available semaphore tokens.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
osStatus
osSemaphoreRelease (osSemaphoreId semaphore_id)
{
  return static_cast<osStatus> ((reinterpret_cast<Semaphore&> (semaphore_id)).post ());
}

/**
 * @details
 * Delete a Semaphore object. The function releases internal memory
 * obtained for Semaphore handling. After this call the semaphore_id
 * is no longer valid and cannot be used. The Semaphore may be created
 * again using the function osSemaphoreCreate.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osStatus
osSemaphoreDelete (osSemaphoreId semaphore_id)
{
  (reinterpret_cast<Semaphore&> (semaphore_id)).~Semaphore ();
  return osOK;
}

#endif /* Semaphore available */

// ----------------------------------------------------------------------------
//  ==== Memory Pool Management Functions ====

#if (defined (osFeature_Pool)  &&  (osFeature_Pool != 0))

/**
 * @details
 * Create and initialize a memory pool.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osPoolId
osPoolCreate (const osPoolDef_t* pool_def)
{
  mempool::Attributes attr
    { pool_def->name };
  attr.mp_pool_address = pool_def->pool;
  return reinterpret_cast<osPoolId> (new ((void*) &pool_def->data) Memory_pool (
      (mempool::size_t) pool_def->pool_sz, (mempool::size_t) pool_def->item_sz));
}

/**
 * @details
 * Allocate a memory block from the memory pool.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
void*
osPoolAlloc (osPoolId pool_id)
{
  return (reinterpret_cast<Memory_pool&> (pool_id)).try_alloc ();
}

/**
 * @details
 * Allocate a memory block from a memory pool and set memory block to zero.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
void*
osPoolCAlloc (osPoolId pool_id)
{
  void* ret;
  ret = (reinterpret_cast<Memory_pool&> (pool_id)).try_alloc ();
  if (ret != nullptr)
    {
      memset (ret, 0, (reinterpret_cast<Memory_pool&> (pool_id)).block_size ());
    }
  return ret;
}

/**
 * @details
 * Return an allocated memory block back to the memory pool.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
osStatus
osPoolFree (osPoolId pool_id, void* block)
{
  return static_cast<osStatus> ((reinterpret_cast<Memory_pool&> (pool_id)).free (
      block));
}

#endif /* Memory Pool Management available */

// ----------------------------------------------------------------------------
//  ==== Message Queue Management Functions ====

#if (defined (osFeature_MessageQ)  &&  (osFeature_MessageQ != 0))

/**
 * @details
 * Create and initialise a message queue.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osMessageQId
osMessageCreate (const osMessageQDef_t* queue_def,
                 osThreadId thread_id __attribute__((unused)))
{
  mqueue::Attributes attr
    { queue_def->name };
  attr.queue_address = queue_def->queue;
  attr.queue_size_bytes = queue_def->queue_sz;

  return reinterpret_cast<osMessageQId> (new ((void*) &queue_def->data) Message_queue (
      attr, (mqueue::size_t) queue_def->items,
      (mqueue::size_t) queue_def->item_sz));
}

/**
 * @details
 * Put the message info in a message queue specified by queue_id.
 *
 * When the message queue is full, the system retries for a specified
 * time with millisec. While the system retries the thread that is
 * calling this function is put into the state WAITING. The millisec
 * timeout can have the following values:
 *
 * - when millisec is 0, the function returns instantly.
 * - when millisec is set to osWaitForever the function will wait
 *  for an infinite time until a message queue slot becomes available.
 * - all other values specify a time in millisecond for a timeout.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
osStatus
osMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec)
{
#pragma GCC diagnostic push
#if defined ( __clang__ )
#pragma clang diagnostic ignored "-Wint-to-pointer-cast"
#endif
  if (millisec == osWaitForever)
    {
      return static_cast<osStatus> ((reinterpret_cast<Message_queue&> (queue_id)).send (
          (const char*) info, sizeof(uint32_t), 0));
    }
  else if (millisec == 0)
    {
      return static_cast<osStatus> ((reinterpret_cast<Message_queue&> (queue_id)).try_send (
          (const char*) info, sizeof(uint32_t), 0));
    }
  else
    {
      return static_cast<osStatus> ((reinterpret_cast<Message_queue&> (queue_id)).timed_send (
          (const char*) info, sizeof(uint32_t), 0,
          Systick_clock::ticks_cast (millisec * 1000u)));
    }
#pragma GCC diagnostic pop
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waggregate-return"

/**
 * @details
 * Suspend the execution of the current RUNNING thread until a
 * message arrives. When a message is already in the queue,
 * the function returns instantly with the message information.
 *
 * The argument millisec specifies how long the system waits for
 * a message to become available. While the system waits the thread
 * that is calling this function is put into the state WAITING.
 * The millisec timeout value can have the following values:
 *
 * - when millisec is 0, the function returns instantly.
 * - when millisec is set to osWaitForever the function will
 * wait for an infinite time until a message arrives.
 * - all other values specify a time in millisecond for a timeout.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
osEvent
osMessageGet (osMessageQId queue_id, uint32_t millisec)
{
  osEvent event;
  uint32_t msg;
  result_t res;
  if (millisec == osWaitForever)
    {
      res = (reinterpret_cast<Message_queue&> (queue_id)).receive (
          (char*) &msg, sizeof(uint32_t), NULL);
      // result::event_message;
    }
  else if (millisec == 0)
    {
      res = (reinterpret_cast<Message_queue&> (queue_id)).try_receive (
          (char*) &msg, sizeof(uint32_t), NULL);
      // result::event_message when message;
      // result::ok when no meessage
    }
  else
    {
      res = (reinterpret_cast<Message_queue&> (queue_id)).timed_receive (
          (char*) &msg, sizeof(uint32_t), NULL,
          Systick_clock::ticks_cast (millisec * 1000u));
      // result::event_message when message;
      // result::event_timeout when timeout;
    }

  // TODO: be sure osEventMessage is returned when appropriate.
  event.status = static_cast<osStatus> (res);
  event.value.v = msg;
  return event;
}

#pragma GCC diagnostic pop

#endif /* Message Queues available */

// ----------------------------------------------------------------------------
//  ==== Mail Queue Management Functions ====

#if (defined (osFeature_MailQ)  &&  (osFeature_MailQ != 0))

/**
 * @details
 * Create and initialise a mail queue.
 *
 * @warning Cannot be invoked from Interrupt Service Routines.
 */
osMailQId
osMailCreate (const osMailQDef_t* queue_def,
              osThreadId thread_id __attribute__((unused)))
{
  mempool::Attributes pool_attr
    { queue_def->name };
  pool_attr.mp_pool_address = queue_def->pool;
  new ((void*) &queue_def->data->pool) Memory_pool (
      (mempool::size_t) queue_def->pool_sz,
      (mempool::size_t) queue_def->pool_item_sz);

  mqueue::Attributes queue_attr
    { queue_def->name };
  queue_attr.queue_address = queue_def->queue;
  queue_attr.queue_size_bytes = queue_def->queue_sz;
  new ((void*) &queue_def->data->queue) Message_queue (
      queue_attr, (mqueue::size_t) queue_def->items,
      (mqueue::size_t) queue_def->queue_item_sz);

  return (osMailQId) (&queue_def->data);
}

/**
 * @details
 * Allocate a memory block from the mail queue that is filled
 * with the mail information.
 *
 * The argument queue_id specifies a mail queue identifier that
 * is obtain with osMailCreate.
 *
 * The argument millisec specifies how long the system waits for
 * a mail slot to become available. While the system waits the
 * tread calling this function is put into the state WAITING.
 * The millisec timeout can have the following values:
 *
 * - when millisec is 0, the function returns instantly.
 * - when millisec is set to osWaitForever the function will
 * wait for an infinite time until a mail slot can be allocated.
 * - all other values specify a time in millisecond for a timeout.
 *
 * A NULL pointer is returned when no memory slot can be obtained
 * or queue specifies an illegal parameter.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
void*
osMailAlloc (osMailQId queue_id, uint32_t millisec)
{
  void* ret = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
  if (millisec == osWaitForever)
    {
      ret = (reinterpret_cast<Memory_pool&> (queue_id->pool)).alloc ();
    }
  else if (millisec == 0)
    {
      ret = (reinterpret_cast<Memory_pool&> (queue_id->pool)).try_alloc ();
    }
  else
    {
      ret = (reinterpret_cast<Memory_pool&> (queue_id->pool)).timed_alloc (
          Systick_clock::ticks_cast (millisec * 1000u));
    }
#pragma GCC diagnostic pop
  return ret;
}

/**
 * @details
 * Allocate a memory block from the mail queue that is filled with
 * the mail information. The memory block returned is cleared.
 *
 * The argument queue_id specifies a mail queue identifier that is
 * obtain with osMailCreate.
 *
 * The argument millisec specifies how long the system waits for a
 * mail slot to become available. While the system waits the thread
 * that is calling this function is put into the state WAITING.
 * The millisec timeout can have the following values:
 *
 * - when millisec is 0, the function returns instantly.
 * - when millisec is set to osWaitForever the function will
 * wait for an infinite time until a mail slot can be allocated.
 * - all other values specify a time in millisecond for a timeout.
 *
 * A NULL pointer is returned when no memory block can be obtained
 * or queue specifies an illegal parameter.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
void*
osMailCAlloc (osMailQId queue_id, uint32_t millisec)
{
  void* ret = osMailAlloc (queue_id, millisec);
  if (ret != nullptr)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
      memset (
          ret, 0,
          (reinterpret_cast<Memory_pool&> ((queue_id->pool))).block_size ());
#pragma GCC diagnostic pop
    }
  return ret;
}

/**
 * @details
 * Put the memory block specified with mail into the mail queue
 * specified by queue.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
osStatus
osMailPut (osMailQId queue_id, void* mail)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
  return static_cast<osStatus> ((reinterpret_cast<Message_queue&> ((queue_id->queue))).try_send (
      (const char*) mail, sizeof(void*), 0));
#pragma GCC diagnostic pop
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waggregate-return"

/**
 * @details
 * Suspend the execution of the current RUNNING thread until a mail
 * arrives. When a mail is already in the queue, the function returns
 * instantly with the mail information.
 *
 * The argument millisec specifies how long the system waits for a
 * mail to arrive. While the system waits the thread that is calling
 * this function is put into the state WAITING. The millisec timeout
 * can have the following values:
 *
 * - when millisec is 0, the function returns instantly.
 * - when millisec is set to osWaitForever the function will wait
 * for an infinite time until a mail arrives.
 * - all other values specify a time in millisecond for a timeout.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
osEvent
osMailGet (osMailQId queue_id, uint32_t millisec)
{
  osEvent event;
  void* msg;
  result_t res;
  if (millisec == osWaitForever)
    {
      res = (reinterpret_cast<Message_queue&> (queue_id)).receive (
          (char*) &msg, sizeof(void*), NULL);
    }
  else if (millisec == 0)
    {
      res = (reinterpret_cast<Message_queue&> (queue_id)).try_receive (
          (char*) &msg, sizeof(void*), NULL);
    }
  else
    {
      res = (reinterpret_cast<Message_queue&> (queue_id)).timed_receive (
          (char*) &msg, sizeof(void*), NULL,
          Systick_clock::ticks_cast (millisec * 1000u));
    }

  event.status = static_cast<osStatus> (res);
  if (event.status == osEventMessage)
    {
      event.status = osEventMail;
    }
  event.value.p = msg;
  return event;
}

#pragma GCC diagnostic pop

/**
 * @details
 * Free the memory block specified by mail and return it to the mail queue.
 *
 * @note Can be invoked from Interrupt Service Routines.
 */
osStatus
osMailFree (osMailQId queue_id, void* mail)
{
  return osPoolFree (&(queue_id->pool), mail);
}

#endif /* Mail Queues available */

// ----------------------------------------------------------------------------