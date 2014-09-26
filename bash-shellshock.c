/* Wrapper for bash -- removes environment variables values start with "("
 * 2014-09-25 Dwayne Litzenberger <dlitz@dlitz.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
extern char **environ;

#ifndef PROG_NAME
#define PROG_NAME "bash-shellshock"
#endif

#ifndef REAL_BASH
#define REAL_BASH "/bin/bash.real"
#endif

#ifndef CONFIG_FILE_LOG_ONLY
#define CONFIG_FILE_LOG_ONLY "/etc/bash-shellshock.log-only"
#endif

#ifndef CONFIG_FILE_STRIP_VARS
#define CONFIG_FILE_STRIP_VARS "/etc/bash-shellshock.strip-vars"
#endif

static char cwdbuf[8192];
static char *cwd;
static pid_t ppid;
static uid_t uid, euid;
static gid_t gid, egid;
static char *info;  /* for logging */

#define MODE_UNINITIALIZED 0
#define MODE_HARD_EXIT 1
#define MODE_STRIP_VARS 2
#define MODE_LOG_ONLY 3

static int config_mode = MODE_UNINITIALIZED;

static void setup(void) {
    struct stat st;

    /* Load configuration */
    if (0 == lstat(CONFIG_FILE_LOG_ONLY, &st)) {
        config_mode = MODE_LOG_ONLY;
    } else if (0 == lstat(CONFIG_FILE_STRIP_VARS, &st)) {
        config_mode = MODE_STRIP_VARS;
    } else {
        config_mode = MODE_HARD_EXIT;
    }

    /* Get a bunch of info for logging */
    ppid = getppid();
    uid = getuid();
    euid = geteuid();
    gid = getgid();
    egid = getegid();
    cwd = getcwd(cwdbuf, sizeof(cwdbuf) - 1);
    if (NULL == cwd) {
        cwd = "";
    }

    if (asprintf(&info, "PPID=%d UID=%d GID=%d EUID=%d EGID=%d CWD=%s", ppid, uid, gid, euid, egid, cwd) < 0) {
        info = "<err>";
    }

    openlog(PROG_NAME, LOG_PID | LOG_CONS, LOG_AUTHPRIV);
}

int main(int argc, char **argv) {
    char **src;
    char **dest;
    char *eq;

    for(src = dest = environ; *src != NULL; src++) {
        eq = strchr(*src, '=');
        if (eq != NULL && eq[1] == '(') {
            if (config_mode == MODE_UNINITIALIZED) {
                setup();
            }

            if (config_mode == MODE_LOG_ONLY) {
                syslog(LOG_WARNING, "(%s) Possibly unsafe environment variable: %s", info, *src);
            } else if (config_mode == MODE_STRIP_VARS) {
                syslog(LOG_WARNING, "(%s) Stripping possibly unsafe environment variable: %s", info, *src);
                *dest = NULL;
            } else {
                syslog(LOG_CRIT, "(%s) Refusing to start due to possibly unsafe environment variable: %s", info, *src);
                fprintf(stderr, "bash-shellshock: Refusing to start due to possibly unsafe environment variable (see syslog)\n");
                exit(255);
            }
        } else {
            if (config_mode == MODE_STRIP_VARS) {
                *dest = *src;
                dest++;
            }
        }
    }

    execve(REAL_BASH, argv, environ);
    syslog(LOG_ERR, "(%s) Failed to start " REAL_BASH ": errno=%d", info, errno);
    exit(127);
}
