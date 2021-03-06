#include "jemalloc/internal/jemalloc_preamble.h"
#include "jemalloc/internal/jemalloc_internal_includes.h"

static void (*safety_check_abort)(const char *message);

void safety_check_fail_sized_dealloc(bool current_dealloc) {
	assert(config_opt_safety_checks);
	char *src = current_dealloc ? "the current pointer being freed" :
	    "in thread cache, possibly from previous deallocations";

	safety_check_fail("<jemalloc>: size mismatch detected, likely caused by"
	   " application sized deallocation bugs (source: %s). Suggest building"
	    "with --enable-debug or address sanitizer for debugging. Abort.\n",
	    src);
	abort();
}

void safety_check_set_abort(void (*abort_fn)(const char *)) {
	safety_check_abort = abort_fn;
}

void safety_check_fail(const char *format, ...) {
	char buf[MALLOC_PRINTF_BUFSIZE];

	va_list ap;
	va_start(ap, format);
	malloc_vsnprintf(buf, MALLOC_PRINTF_BUFSIZE, format, ap);
	va_end(ap);

	if (safety_check_abort == NULL) {
		malloc_write(buf);
		abort();
	} else {
		safety_check_abort(buf);
	}
}
