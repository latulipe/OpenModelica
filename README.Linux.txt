Debian/Ubuntu Compile Cheat Sheet (or read on for the full guide)
=================================================================

$ sudo su -c "echo deb http://build.openmodelica.org/apt nightly contrib >> /etc/apt/sources.list"
$ sudo su -c "echo deb-src http://build.openmodelica.org/apt nightly contrib >> /etc/apt/sources.list"
$ sudo apt-get update
$ sudo apt-get build-dep openmodelica
$ autoconf
$ ./configure --with-omniORB
$ make # or make omc if you only want the omc core and not the qtclients

How to compile on Linux/BSD (all from source)
===================================================

$ ./configure --prefix=/usr/local
$ make
$ sudo make install

But first you need to install dependencies:
    rml+mmc (http://www.ida.liu.se/~pelab/rml/)
        Just grab it from subversion:
        svn co https://openmodelica.org/svn/MetaModelica/trunk mmc
        user: anonymous
        pass: none
    rml needs smlnj: http://www.smlnj.org (working version v110.xx) and/or mlton (mlton.org)
    mico or omniORB:
        omniORB:
          Is better maintained by Linux distributions, but doesn't print as
          good error messages if things go really wrong.
        mico:
          http://www.mico.org - tested on 2.3.11, 2.3.12, 2.3.13
          Note: for newer gcc compilers you might need to add
            #include <limits.h> in orb/fast_array.cc
    java     version > 1.4
    gcc      (tested with most of the versions; 4.4 or 4.6 recommended over 4.5)
    readline & libreadlineX-dev, currently X=5
    liblpsolve: http://www.cs.sunysb.edu/~algorith/implement/lpsolve/distrib/lp_solve_5.5.0.11_source.tar.gz
    sqlite3
The latest OpenModelica uses Qt for potting and 3D functionality. You will also need:
    Qt 4.x.x (http://trolltech.com - >= 4.6)
    Coin3D   (http://www.coin3d.org - tested with 3.0.0; deprecated)
    SoQt     (http://www.coin3d.org - tested with 1.4.1; deprecated)
OMOptim uses some packages for its optimization algorithms
    paradisEO (http://paradiseo.gforge.inria.fr/ - tested with 1.3; see the Debian installer for the directory structure or send openmodelica <at> ida.liu.se a listing of the files paradiseo installs to /usr/local/ to have the Makefiles updated)
Note:
    FreeBSD versions of smlnj/mlton only compile using 32-bit versions of the OS.
    The rml-mmc package needs some manual changes, too.

How to compile on Ubuntu Linux (using available binary packages for dependencies)
=================================================================================

You need:
    antlr
        $ sudo apt-get install antlr libantlr-dev
    rml+mmc see above and:
        $ sudo apt-get install libsmlnj-smlnj
        or if you like to use mlton
        $ sudo apt-get install mlton
    java
        you need to install OpenJDK Java runtime or Sun Java runtime
        $ sudo apt-get install openjdk-6-jre
        $ sudo update-java-alternatives -s java-6-openjdk
        or
        $ sudo apt-get install sun-java6-jre
        $ sudo update-java-alternatives -s java-6-sun
    Qt+Coin3D+SoQt
        you need readline and Qt dev stuff to compile omc and mosh (OMShell)
        $ sudo apt-get install libreadline5-dev libsoqt4-dev
    paradiseo
      sudo apt-get install paradiseo (using the OpenModelica repository)
    sqlite3
        $ sudo apt-get install sqlite3 libsqlite3-dev
    liblpsolve55
      You can now use the version from the Ubuntu repository
        $ sudo apt-get install liblpsolve55-dev
        
      It is also possible to copy this library from the Compiler runtime:
        For 32-bit OSX:
        $ sudo cp Compiler/runtime/lpsolve/lib/osx/liblpsolve55.dylib /usr/local/lib
        For 64-bit OSX:
        $ sudo cp Compiler/runtime/lpsolve/lib/x86_64-osx/liblpsolve55.dylib /usr/local/lib
        (Note that the Linux versions have been remove now that the configure script has been fixed)
      Alternatively you can compile lpsolve yourself:
        Download the library:
         http://www.cs.sunysb.edu/~algorith/implement/lpsolve/distrib/lp_solve_5.5.0.11_source.tar.gz
        and unpack it, then call in the top folder: 
        $ make -f Makefile.Linux
        then copy lpsolve55/liblpsolve55.a to /usr/local/lib
      Note that some versions of lp_solve depends on libsuitesparse-dev, which provides -lcolamd.


NOTE:
  We assume you took the source from Subversion in a subdirectory called "trunk".
  If you used some other name, replace "trunk" below with your directory.

Setting your environment for compiling OpenModelica
===================================================
  If rmlc is not on the PATH, set RMLHOME to rml installation, e.g.
  /usr/local/rml/x86-linux-gcc/

  If you plan to use mico corba with OMC you need to:
  - set the PATH to path/to/mico/bin (for the idl compiler and mico-cpp)
  - set the LD_LIBRARY_PATH to path/to/installed/mico/lib (for mico libs)
  - set the PATH: $ export PATH=${PATH}:/path/to/installed/mico/bin
    + this is for executables: idl, mico-cpp and mico-config

To Compile OpenModelica
  run:
    $ ./configure --with-CORBA=/path/to/mico (if you want omc to use mico corba)
    $ ./configure --with-omniORB=/path/to/omniORB (if you want omc to use omniORB corba)
    $ ./configure --without-CORBA            (if you want omc to use sockets)
  in the trunk directory
  Make sure that all makefiles are created. Check carefully for error messages.

    $ make omc       (to build omc and simulation runtime)
    $ make mosh      (to build OMShell-terminal)
    $ make qtclients (to build Qt based clients: OMShell, ext, OMNotebook; requires CORBA)

  After the compilation the results are in the path/to/trunk/build.
  To run the testsuite:
    $ make test
  Note: Some Modelica Standard Library functions depend on LAPACK.
        On Ubuntu, this is installed by lpsolve dependencies.
        If you compiled omc using a static lpsolve, it is possible that you
        don't have LAPACK installed.

  If you run into problems read the GENERAL NOTES below and if that
  does not help, subscribe to the OpenModelicaInterest list:
    https://www.openmodelica.org/index.php/home/mailing-list
  and then sent us an email at [OpenModelicaInterest@ida.liu.se].

How to run
==========
For debugging purposes it can be useful to start OMShell and omc in two different termnials.
For this use:
trunk/build/bin/omc +d=interactive      (if you configured with --without-CORBA) or
trunk/build/bin/omc +d=interactiveCorba (if you comfigured with --with-CORBA=path/to/mico)

trunk/build/bin/OMShell-terminal -noserv         (if you configured with --without-CORBA) or
trunk/build/bin/OMShell-terminal -noserv -corba  (if you configured with --with-CORBA=path/to/mico)

( The -noserv argument will prevent mosh from starting its own omc in the background )

If you want to change the port number of the socket connection you
will have to do it manually in mosh.cpp and Compiler/Main.mo.

Example Session
===============
Here is a short example session.

$ cd trunk/build/bin
$ ./OMShell-terminal
OpenModelica 1.8.0
Copyright (c) OSMC 2002-2011
To get help on using OMShell and OpenModelica, type "help()" and press enter.
>> loadModel(Modelica)
true

>> instantiateModel(Modelica.Electrical.Analog.Basic.Resistor)
"class Modelica.Electrical.Analog.Basic.Resistor
Real v(quantity = "ElectricPotential", unit = "V") "Voltage drop between the two pins (= p.v - n.v)";
Real i(quantity = "ElectricCurrent", unit = "A") "Current flowing from pin p to pin n";
Real p.v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
Real p.i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
Real n.v(quantity = "ElectricPotential", unit = "V") "Potential at the pin";
Real n.i(quantity = "ElectricCurrent", unit = "A") "Current flowing into the pin";
parameter Real R(quantity = "Resistance", unit = "Ohm", start = 1.0) "Resistance";
equation
  R * i = v;
  v = p.v - n.v;
  0.0 = p.i + n.i;
  i = p.i;
  n.i = 0.0;
  p.i = 0.0;
end Modelica.Electrical.Analog.Basic.Resistor;
"
Warning, parameter R has no value

>> a:=1:5;
>> b:=3:8
{3,4,5,6,7,8}
>> a*b
Incompatible argument types to operation scalar product, left type: Integer[5],\
 right type: Integer[6]
Incompatible argument types to operation scalar product, left type: Real[5],\
 right type: Real[6]
Cannot resolve type of expression a*b (expressions :{a[1],a[2],a[3],a[4],a[5]},\
 {b[1],b[2],b[3],b[4],b[5],b[6]} types: Integer[5], Integer[6])

>> b:=3:7;
>> a*b
85
>>> listVariables()
{a, b}
>>

GENERAL NOTES:
==============
- Fedora Core 4 has a missing symlink. To fix it, in /usr/lib do:
  ln -s libg2c.so.0 libg2c.so
  Otherwise the testsuite will fail when generating simulation code.

- On some Linux systems when running simulate(Model, ...) the
  executable for the Model enters an infinite loop. To fix this, add -ffloat-store to CFLAGS

Last updated 2012-07-04
