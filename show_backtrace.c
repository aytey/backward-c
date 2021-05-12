/*
 * This code is licensed under MIT license (see LICENSE for details)
 * (c) 2020 Maxim Egorushkin
 * (c) 2021 Andrew V. Jones
 */

/* standard headers */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* for libdw */
#include <elfutils/libdwfl.h>

/* for libunwind */
#define UNW_LOCAL_ONLY
#include <libunwind.h>

/* strdup isn't in c99, but it is here if we're gnu99 */
#if !(defined __USE_SVID || defined __USE_BSD ||                               \
      defined __USE_XOPEN_EXTENDED || defined __USE_XOPEN2K8)
static char *strdup(const char *s) {
  size_t size = strlen(s) + 1;
  char *p = malloc(size);
  if (p) {
    memcpy(p, s, size);
  }
  return p;
}
#endif

typedef struct DebugInfoSession {
  Dwfl_Callbacks callbacks;
  char *debuginfo_path;
  Dwfl *dwfl;
} DebugInfoSession;

static void DebugInfoSession_ctor(DebugInfoSession *dis) {
  /* zeroise everything; all pointers are NULL */
  memset(dis, 0, sizeof(DebugInfoSession));

  /* set-up call backs */
  dis->callbacks.find_elf = dwfl_linux_proc_find_elf;
  dis->callbacks.find_debuginfo = dwfl_standard_find_debuginfo;
  dis->callbacks.debuginfo_path = &dis->debuginfo_path;

  /* start the dwarf session */
  dis->dwfl = dwfl_begin(&dis->callbacks);
  assert(dis->dwfl);

  int r = 0;

  /* report on dwarf from proc using the current pid */
  r = dwfl_linux_proc_report(dis->dwfl, getpid());
  assert(!r);

  /* end the current report */
  r = dwfl_report_end(dis->dwfl, NULL, NULL);
  assert(!r);
}

static void DebugInfoSession_dtor(DebugInfoSession *dis) {
  /* end the dwarf session */
  dwfl_end(dis->dwfl);
}

typedef struct DebugInfo {
  uintptr_t ip;
  const char *function;
  const char *file;
  int line;
} DebugInfo;

static void DebugInfo_ctor(DebugInfo *di, DebugInfoSession *dis, uintptr_t ip) {
  /* zeroise everything; all pointers are NULL */
  memset(di, 0, sizeof(DebugInfo));

  /* store the IP */
  di->ip = ip;

  /* line is -1 */
  di->line = -1;

  /* Get function name */
  Dwfl_Module *module = dwfl_addrmodule(dis->dwfl, ip);
  char const *name = dwfl_module_addrname(module, ip);
  if (name) {
    di->function = strdup(name);
  } else {
    di->function = strdup("<unknown>");
  }

  /* Get source filename and line number */
  Dwfl_Line *dwfl_line = dwfl_module_getsrc(module, ip);
  if (dwfl_line) {
    Dwarf_Addr addr;
    di->file =
        strdup(dwfl_lineinfo(dwfl_line, &addr, &di->line, NULL, NULL, NULL));
  }
}

static void print_context(const char *path, int line_no, int context) {
  /* adjust the line number; users don't use zero-based line numbers */
  int adjusted_line_no = line_no - 1;

  /* how many lines before? */
  int start = adjusted_line_no - context;

  /* how many lines after? */
  int end = adjusted_line_no + context;

  /* how many lines have we read? */
  int lines_read = 0;

  /* open the file */
  FILE *file = fopen(path, "r");

  /* if the file exists ... */
  if (file != NULL) {

    /* line buffer */
    char line[256];

    /* read a line */
    while (fgets(line, sizeof line, file) != NULL) {

      /* if we're showing context */
      if (lines_read >= start && lines_read <= end) {

        /* line we want gets the prefix */
        if (lines_read == adjusted_line_no) {
          fprintf(stderr, "%5s", ">");
        } else {
          fprintf(stderr, "%5s", " ");
        }

        /* print the line */
        fprintf(stderr, "%5d %s", lines_read, line);
      }

      /* if we're after the end, we can stop */
      if (lines_read >= end) {
        break;
      }

      /* total lines read */
      ++lines_read;
    }

    /* close the file */
    fclose(file);
  }
}

void show_backtrace(void) {
  /* uses libunwind to load the current stack, and uses libdw to show the debug
   * information */

  /* create a debug session */
  DebugInfoSession dis;
  DebugInfoSession_ctor(&dis);

  /* libunwind context */
  unw_cursor_t cursor;
  unw_context_t uc;
  unw_word_t ip;

  /* load the stack */
  unw_getcontext(&uc);
  unw_init_local(&cursor, &uc);

  /* for all elements of the stack */
  while (unw_step(&cursor) > 0) {

    /* get the IP for the current stack */
    unw_get_reg(&cursor, UNW_REG_IP, &ip);

    /* decrement the ip to the currently executing statement */
    --ip;

    /* create a debug info object */
    DebugInfo di;
    DebugInfo_ctor(&di, &dis, (uintptr_t)ip);

    /* what's the file + line? */
    if (di.file) {
      fprintf(stderr, "Source \"%s\" line %d ", di.file, di.line);
    } else {
      fprintf(stderr, "Object \"\" at 0x%lx ", di.ip);
    }

    fprintf(stderr, "in %s\n", di.function);

    if (di.file) {
      print_context(di.file, di.line, /* context */ 3);
    }
  }

  /* gracefully shutdown libdw */
  DebugInfoSession_dtor(&dis);
}

// EOF
