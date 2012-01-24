Building the LevelDB Python bindings 
====================================

First checkout the version you want to build, *Always build from a particular tag, since HEAD may be
a work in progress,* for example, to build version 0.4, run:

.. code-block:: bash

    git checkout v0.4

Then follow the build steps below:


First of all, you need to build the included ``snappy`` and  ``leveldb`` library.

Building ``Snappy``
-------------------

.. code-block:: bash

    $ cd snappy
    $ ./configure && make && make install


Building ``LevelDB``
--------------------

.. code-block:: bash

    $ cd leveldb
    $ make 

    g++ -c -I. -I./include -fno-builtin-memcmp -DLEVELDB_PLATFORM_POSIX -DLEVELDB_CSTDATOMIC_PRESENT\
    -std=c++0x -pthread -DOS_LINUX -O2 -DNDEBUG -DSNAPPY util/histogram.cc -o util/histogram.o
    ... ...
    g++ -c -I. -I./include -fno-builtin-memcmp -DLEVELDB_PLATFORM_POSIX -DLEVELDB_CSTDATOMIC_PRESENT\
    -std=c++0x -pthread -DOS_LINUX -O2 -DNDEBUG        -DSNAPPY util/options.cc -o util/options.o
    rm -f libleveldb.a
    ar -rs libleveldb.a ./db/builder.o ./db/c.o ./db/db_impl.o ./db/db_iter.o ./db/filename.o\
    ... ./util/histogram.o ./util/logging.o ./util/options.o ./util/status.o
    ar: creating libleveldb.a


Building ``cPy-LevelDB`` 
-------------------------

.. code-block:: bash

    $ python setup.py build

    running build
    running build_py
    running build_ext
    building 'leveldb' extension
    gcc -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes\
    -fPIC -I/usr/include/python2.6 -c src/snapshot.c -o build/temp.linux-i686-2.6/src/snapshot.o\
    -Wall -pedantic -I./leveldb/include/ -shared -std=gnu99 -fPIC -g -D_GNU_SOURCE
    ... ...
    gcc -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes\
    -fPIC -I/usr/include/python2.6 -c src/range_iterator.c -o build/temp.linux-i686-2.6/src/range_iterator.o\
    -Wall -pedantic -I./leveldb/include/ -shared -std=gnu99 -fPIC -g -D_GNU_SOURCE
    gcc -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions build/temp.linux-i686-2.6/src/initmodule.o\
    build/temp.linux-i686-2.6/src/leveldb.o build/temp.linux-i686-2.6/src/write_batch.o\
    build/temp.linux-i686-2.6/src/iterator.o build/temp.linux-i686-2.6/src/snapshot.o\
    build/temp.linux-i686-2.6/src/range_iterator.o -o build/lib.linux-i686-2.6/leveldb.so\
    -L./leveldb -static -lleveldb -lsnappy -lpthread


Dependencies
------------

The binding itself has no dependencies.

