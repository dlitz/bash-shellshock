bash-shellshock wrapper
=======================

This is a small wrapper around /bin/bash that refuses to start bash (or
optionally strips bad environment variables and continues) if any environment
variables start with '('.

You can install this as a temporary workaround if you don't fully trust the
latest patches for CVE-2014-6271 and CVE-2014-7169, which (as of 2014-09-25)
seem like they've been a bit rushed.


Downloading
-----------

**Binaries available:** https://github.com/dlitz/bash-shellshock/tree/binaries/binaries

Source code: https://github.com/dlitz/bash-shellshock


Installation
------------

To build and install this package under Debian or Ubuntu, run:

    dpkg-buildpackage -us -uc -tc
    sudo dpkg -i ../bash-shellshock_0.0_amd64.deb

For manual installation, run this:

    make
    make install
    ln /bin/bash /bin/bash.real
    ln -sf /bin/bash-shellshock /bin/bash


Configuration
-------------

If bash-shellshock finds environment variables starting with '(', it will do
one of three things (in order of priority):

1. If `/etc/bash-shellshock.log-only` exists, it will log the event to syslog
   and exec `/bin/bash.real`.

2. If `/etc/bash-shellshock.strip-vars` exists, it will log the event to
   syslog, remove the variable from the environment, and exec `/bin/bash.real`.

3. Otherwise, it will log the event to syslog and abort with exit status 255.


Testing
-------

* For CVE-2014-6271

        env X='() { ignored; }; /bin/echo "You are vulnerable to CVE 2014-6271"' bash -c true

* For CVE-2014-7169 ([reference](https://twitter.com/taviso/status/514887394294652929)):

        env X='() { (a)=>\' bash -c "echo date"; cat echo



Logging
-------

Logs go to the `LOG_AUTHPRIV` syslog facility, which is typically found at `/var/log/auth.log`.


Caveat
------

I don't really know what this breaks.


License
-------
Copyright 2014 InboxApp Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
