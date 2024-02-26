/*
 * Functions used across the program.
 *
 * Copyright 2002-2008, 2010, 2012-2015, 2017, 2021, 2023 Andrew Wood
 *
 * License GPLv3+: GNU GPL version 3 or later; see `docs/COPYING'.
 */

#ifndef _PV_H
#define _PV_H 1

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Opaque structure for PV internal state.
 */
struct pvstate_s;
typedef struct pvstate_s *pvstate_t;

/*
 * Valid number types for pv_getnum_check().
 */
typedef enum {
  PV_NUMTYPE_INTEGER,
  PV_NUMTYPE_DOUBLE
} pv_numtype;


/*
 * Simple string functions for processing numbers.
 */

/*
 * Return the given string converted to a double, for use as a time
 * interval.
 */
extern double pv_getnum_interval(const char *);

/*
 * Return the given string converted to an off_t, for use as a size.
 */
extern off_t pv_getnum_size(const char *);

/*
 * Return the given string converted to an unsigned integer, for use as a
 * count such as screen width.
 */
extern unsigned int pv_getnum_count(const char *);

/*
 * Return true if the given string is a number of the given type.  NB an
 * integer is both a valid integer and a valid double.
 */
extern bool pv_getnum_check(const char *, pv_numtype);

/*
 * String handling wrappers.
 */

/*
 * Wrapper for sprintf(), falling back to sprintf() on systems without that
 * function.
 */
extern int pv_snprintf(char *, size_t, const char *, ...);

/*
 * Implementation of strlcat() where it is unavailable: append a string to a
 * buffer, constraining the buffer to a particular size and ensuring
 * termination with '\0'.
 */
extern size_t pv_strlcat(char *, const char *, size_t);

/*
 * Allocate and return a duplicate of a \0-terminated string, ensuring that
 * the duplicate is also \0-terminated.  Returns NULL on error.
 */
/*@null@ */ /*@only@ */ extern char *pv_strdup(const char *);

/*
 * Return a pointer to the last matching character in the buffer, or NULL if
 * not found.
 */
/*@null@ */ /*@temp@ */ extern void *pv_memrchr(const void *, int, size_t);

/*
 * Functions relating to elapsed time.
 */

/*
 * Read the current elapsed time, relative to an unspecified point in the
 * past, and store it in the given timespec buffer.  The time is guaranteed
 * to not go backwards and does not count time when the system was
 * suspended.  See clock_gettime(2) with CLOCK_MONOTONIC.
 */
void pv_elapsedtime_read(struct timespec *);

/* Set the time in the given timespec to zero. */
void pv_elapsedtime_zero(struct timespec *);

/* Copy the second timespec into the first.  Analogous to strcpy(3). */
void pv_elapsedtime_copy(struct timespec *, const struct timespec *);

/*
 * Return -1, 0, or 1 depending on whether the first time is earlier than,
 * equal to, or later than the second time.  Analogous to strcmp(3).
 */
int pv_elapsedtime_compare(const struct timespec *, const struct timespec *);

/* Add the latter two timespecs and store them in the first timespec. */
void pv_elapsedtime_add(struct timespec *, const struct timespec *, const struct timespec *);

/* Add a number of nanoseconds to the given timespec. */
void pv_elapsedtime_add_nsec(struct timespec *, long long);

/* Set the first timespec to the second minus the third. */
void pv_elapsedtime_subtract(struct timespec *, const struct timespec *, const struct timespec *);

/* Convert a timespec to seconds. */
long double pv_elapsedtime_seconds(const struct timespec *);

/* Sleep for a number of nanoseconds. */
void pv_nanosleep(long long);

          
/*
 * Main PV functions.
 */

/*
 * Create a new state structure, and return it, or 0 (NULL) on error.
 */
extern /*@null@*/ /*@only@*/ pvstate_t pv_state_alloc(const char *);

/*
 * Set the formatting string, given a set of old-style formatting options.
 */
extern void pv_state_set_format(pvstate_t state, bool progress,
				bool timer, bool eta,
				bool fineta, bool rate,
				bool average_rate, bool bytes,
				bool bufpercent,
				size_t lastwritten,
				/*@null@*/ const char *name);

/*
 * Set the various options.
 */
extern void pv_state_force_set(pvstate_t, bool);
extern void pv_state_cursor_set(pvstate_t, bool);
extern void pv_state_numeric_set(pvstate_t, bool);
extern void pv_state_wait_set(pvstate_t, bool);
extern void pv_state_delay_start_set(pvstate_t, double);
extern void pv_state_linemode_set(pvstate_t, bool);
extern void pv_state_bits_set(pvstate_t, bool);
extern void pv_state_null_terminated_lines_set(pvstate_t, bool);
extern void pv_state_no_display_set(pvstate_t, bool);
extern void pv_state_skip_errors_set(pvstate_t, unsigned int);
extern void pv_state_error_skip_block_set(pvstate_t, off_t);
extern void pv_state_stop_at_size_set(pvstate_t, bool);
extern void pv_state_sync_after_write_set(pvstate_t, bool);
extern void pv_state_direct_io_set(pvstate_t, bool);
extern void pv_state_rate_limit_set(pvstate_t, off_t);
extern void pv_state_target_buffer_size_set(pvstate_t, size_t);
extern void pv_state_no_splice_set(pvstate_t, bool);
extern void pv_state_discard_input_set(pvstate_t, bool);
extern void pv_state_size_set(pvstate_t, off_t);
extern void pv_state_interval_set(pvstate_t, double);
extern void pv_state_width_set(pvstate_t, unsigned int, bool);
extern void pv_state_height_set(pvstate_t, unsigned int, bool);
extern void pv_state_name_set(pvstate_t, /*@null@*/ const char *);
extern void pv_state_format_string_set(pvstate_t, /*@null@*/ const char *);
extern void pv_state_watch_pid_set(pvstate_t, pid_t);
extern void pv_state_watch_fd_set(pvstate_t, int);
extern void pv_state_average_rate_window_set(pvstate_t, unsigned int);

extern void pv_state_inputfiles(pvstate_t, unsigned int, const char **);

/*
 * Work out whether we are in the foreground.
 */
extern bool pv_in_foreground(void);

/*
 * Work out the terminal size.
 */
extern void pv_screensize(unsigned int *width, unsigned int *height);

/*
 * Calculate the total size of all input files.
 */
extern off_t pv_calc_total_size(pvstate_t);

/*
 * Set up signal handlers ready for running the main loop.
 */
extern void pv_sig_init(pvstate_t);

#ifdef SA_SIGINFO
/*
 * Return true if SIGUSR2 has been received, and indicate the sender.
 */
extern bool pv_sigusr2_received(pvstate_t, pid_t *);
#endif

/*
 * Enter the main transfer loop, transferring all input files to the output.
 */
extern int pv_main_loop(pvstate_t);

/*
 * Watch the selected file descriptor of the selected process.
 */
extern int pv_watchfd_loop(pvstate_t);

/*
 * Watch the selected process.
 */
extern int pv_watchpid_loop(pvstate_t);

/*
 * Shut down signal handlers after running the main loop.
 */
extern void pv_sig_fini(pvstate_t);

/*
 * Free a state structure, after which it can no longer be used.
 */
extern void pv_state_free(/*@only@*/ pvstate_t);


#ifdef ENABLE_DEBUGGING
# if __STDC_VERSION__ < 199901L && !defined(__func__)
#  if __GNUC__ >= 2
#   define __func__ __FUNCTION__
#  else
#   define __func__ "<unknown>"
#  endif
# endif
# define debug(x,...) debugging_output(__func__, __FILE__, __LINE__, x, __VA_ARGS__)
#else
# define debug(x,...) do { } while (0)
#endif

/*
 * Set the debugging destination file, if debugging is enabled.
 */
void debugging_output_destination(const char *);

/*
 * Output debugging information, if debugging is enabled.
 */
void debugging_output(const char *, const char *, int, const char *, ...);


#ifdef __cplusplus
}
#endif

#endif /* _PV_H */

/* EOF */
