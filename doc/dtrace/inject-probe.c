#include <unistd.h>
#include <dtrace.h>
#include <iostream>
#include <signal.h>
#include <stdio.h>

using namespace std;

static int chewrec(const dtrace_probedata_t *data, const dtrace_recdesc_t *rec, void *arg) {
  cerr << "in chewrec" << endl;
  return DTRACE_CONSUME_THIS;
}

static int chew(const dtrace_probedata_t *data, void *arg) {
  cerr << "in chew" << endl;
  return DTRACE_CONSUME_THIS;
}

int main (int argc, char** argv) {
  int err;

  dtrace_hdl_t* dtp;

  if ((dtp = dtrace_open(DTRACE_VERSION, 0, &err)) == NULL) {
     cerr << "failed to initialize dtrace: " << dtrace_errmsg(NULL, err) << endl;
     return 1;
  }

  if (dtrace_setopt(dtp, "strsize", "128") == -1) {
     cerr << "failed to set 'strsize'" << endl;
     return 1;
  }

  if (dtrace_setopt(dtp, "bufsize", "10k") == -1) {
     cerr << "failed to set 'bufsize'" << endl;
     return 1;
  }

  if (dtrace_setopt(dtp, "aggsize", "256k") == -1) {
     cerr << "failed to set 'aggsize'" << endl;
     return 1;
  }

  if (dtrace_setopt(dtp, "aggrate", "1sec") == -1) {
     cerr << "failed to set 'aggrate'" << endl;
     return 1;
  }

  if (dtrace_setopt(dtp, "define", "ARCH64") != 0) {
     cerr << "dtrace_setopt define failed" << endl;
     return 1;
  }

  const char *script = "syscall::open:entry { printf(\"%d %s\", pid, copyinstr(arg0)); }";

  dtrace_prog_t *prog = nullptr;

  prog = dtrace_program_strcompile(dtp, script, DTRACE_PROBESPEC_NAME, DTRACE_C_ZDEFS, 0, NULL);
  if (prog == nullptr) {
     cerr << "failed to compile program" << endl;
     return 1;
  }

  dtrace_proginfo_t info;

  if (dtrace_program_exec(dtp, prog, &info) == -1) {
     cerr << "dtrace_program_exec failed" << endl;
     return 1;
  }

  if (dtrace_go(dtp) != 0) {
     cerr << "dtrace_go failed: " << dtrace_errmsg(dtp, dtrace_errno(dtp)) << endl;
     return 1;
  }

  while (1) {
    dtrace_sleep(dtp);

    int status = dtrace_status(dtp);
    cerr << "status: " << status << endl;
    if (status != DTRACE_STATUS_OKAY) {
      cerr << "status not OK" << endl;
      return 1;
    }

    dtrace_work(dtp, stdout, NULL, chewrec, NULL);
    cerr << endl;

    cerr << "ping" << endl;
    // sleep(1);
  }

  dtrace_close(dtp);
}
