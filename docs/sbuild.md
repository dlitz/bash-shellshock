Building binary packages on Debian using sbuild
===============================================

- Install and configure sbuild (adapted from the Debian [sbuild
  wiki](https://wiki.debian.org/sbuild)):

        sudo apt-get install sbuild
        sudo sbuild-update --keygen
        sudo sbuild-adduser $LOGNAME
         ... *logout* and *re-login*
        sudo sbuild-createchroot --arch=amd64 --make-sbuild-tarball=/var/lib/sbuild/wheezy-amd64.tar.gz wheezy `mktemp -d` http://ftp.debian.org/debian
        sudo sbuild-createchroot --arch=i386 --make-sbuild-tarball=/var/lib/sbuild/wheezy-i386.tar.gz wheezy `mktemp -d` http://ftp.debian.org/debian

- Build the packages:

        # Clone the sources
        git clone https://github.com/dlitz/bash-shellshock

        # Build source package
        ( cd bash-shellshock && dpkg-buildpackage -us -uc -S -tc )

        # Build binary packages
        sbuild -d wheezy --arch=amd64 *.dsc
        sbuild -d wheezy --arch=i386 *.dsc
