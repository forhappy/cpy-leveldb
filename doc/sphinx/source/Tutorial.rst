cPy-LevelDB Tutorial
====================

This document shows how to use LevelDB from Python. If you are not familiar with LevelDB,
you will want to get a brief overview of the database and its example API. The official
tutorial is a great place to start.

Next, you will want to install and run LevelDB.

A working Python program complete with examples from this tutorial can be
found in the examples folder of the source distribution.

Simple API
----------

When writing programs with Python, you will be using four different
entities: connections, iterators, snapshots, and writebatch. 

So, for instance, to create a new connection, start by allocating a ``LevelDB`` object:

.. code-block:: python 

    import leveldb;
    db = leveldb.LevelDB("/tmp/test-leveldb")

Next, you can do some operations on the ``LevelDB`` object, such as ``Put``,
``Get``, ``Delete`` and so on:

.. code-block:: python 

    db.Put("hello", "world")
    print db.Get("hello")

Set any optional values, like a block cache size, you can use a specific parameter when allocating a ``LevelDB`` object: 

.. code-block:: python 

    import leveldb;
    db = leveldb.LevelDB(filename = "/tmp/test-leveldb", block_cache_size = 4096 * 128)
    db.Put("hello", "world")
    print db.Get("hello")

When you are finished, close the ``LevelDB`` object:

.. code-block:: python

    import leveldb;
    db = leveldb.LevelDB("/tmp/test-leveldb")
    '''
    do operations on the leveldb db.
    '''
    ......
    db.Close()

There are more details, but that is the basic pattern. Keep this in mind as you learn the API and start using the python package.

Constructing a LevelDB object with other parameters
---------------------------------------------------

Let us start by that connects to the database:

.. code-block:: python 

    import leveldb
    import datetime
    import uuid

    db = leveldb.LevelDB(\
        filename = "/tmp/test-leveldb",\
        create_if_missing = True,\
        error_if_exists = True,\
        paranoid_checks = False,\
        write_buffer_size = 4096 * 32,\
        block_size = 128,\
        block_restart_inteval = 100,\
        block_cache_size = 4096 * 128,\
        compression = True)
    
    for i in range(n):
        db.Put(str(i), "this is item %d"%(i))
    print("%s: lookup a item from map..."%(datetime.datetime.now()))
    print(db.Get("43"))
    print("%s: ok"%(datetime.datetime.now()))


Iteration
---------
The following example demonstrates how to print key,value pairs in a database.

.. code-block:: python

    import leveldb
    db = leveldb.LevelDB("/tmp/test-leveldb/")
    batch = leveldb.WriteBatch();
    for i in xrange(1000):
    	batch.Put(str(i), "hello, hello, hello string %s" %i)
    db.Write(batch)
    
    print "Creating iterator"
    iter = leveldb.Iterator(db);

    print "Seek to First."
    iter.First()
    print "\n"
    print "Print iterator's key"
    print iter.Key()
    print "\n"
    print "Print iterator's value"
    print iter.Value()
    print "\n"
    
    print "Seek to next"
    iter.Next()
    print "\n"
    print "Print iterator's key again"
    print iter.Key()
    print "\n"
    print "Print iterator's value again"
    print iter.Value()
    print "\n"
    
    print "Seek to Last"
    iter.Last()
    print "\n"
    print "Print iterator's key again"
    print iter.Key()
    print "\n"
    print "Print iterator's value again"
    print iter.Value()
    print "\n"

The following example show how to iterate all key-value pairs in a datebase.

.. code-block:: python

    import leveldb
    db = leveldb.LevelDB("/tmp/test-leveldb/")
    
    batch = leveldb.WriteBatch();
    
    for i in xrange(1000):
    	batch.Put(str(i), "hello, hello, hello string %s" %i)
    db.Write(batch)
    
    print "Creating iterator"
    
    iter = leveldb.Iterator(db);
    iter.First()
    while True:
        if (iter.Validate()):
            print iter.Key()
            print iter.Value()
            iter.Next()
        else:
            break

The following variation shows how to process just the keys in the range [start,limit):

.. code-block:: python

    import datetime
    import uuid
    import leveldb
    
    def main():
        n=100
        print("%s: write %d items to db.."%(datetime.datetime.now(),n))
        db = leveldb.LevelDB("./leveldb.db")
        for i in range(n):
            db.Put(str(i), "this is item %d"%(i))
            
        print("%s: lookup items from map between range key_from to key_to..."\
            %(datetime.datetime.now()))
        print list(db.RangeIter(key_from = '80', key_to = '90'))
        print("%s: ok"%(datetime.datetime.now()))
        return 0
    
    if __name__ == '__main__':
        main()

Synchronous Writes
------------------

By default, each write to ``LevelDB`` is asynchronous: it returns after pushing
the write from the process into the operating system. The transfer from
operating system memory to the underlying persistent storage happens
asynchronously. The ``sync`` flag can be turned on for a particular write to make
the write operation not return until the data being written has been pushed all
the way to persistent storage. (On ``Posix`` systems, this is implemented by calling
either ``fsync(...)`` or ``fdatasync(...)`` or ``msync(..., MS_SYNC)`` before the write
operation returns.) 

Asynchronous writes are often more than a thousand times as fast as synchronous
writes. The downside of asynchronous writes is that a crash of the machine may
cause the last few updates to be lost. Note that a crash of just the writing
process (i.e., not a reboot) will not cause any loss since even when ``sync`` is
false, an update is pushed from the process memory into the operating system
before it is considered done.

Asynchronous writes can often be used safely. For example, when loading a large
amount of data into the database you can handle lost updates by restarting the
bulk load after a crash. A hybrid scheme is also possible where every Nth write
is synchronous, and in the event of a crash, the bulk load is restarted just
after the last synchronous write finished by the previous run. (The synchronous
write can update a marker that describes where to restart on a crash.)

``WriteBatch`` provides an alternative to asynchronous writes. Multiple updates may
be placed in the same WriteBatch and applied together using a synchronous write
The extra cost of the synchronous write will be amortized across all of the writes in the batch. 

.. code-block:: python

    import leveldb
    db = leveldb.LevelDB("/tmp/test-leveldb/")
    batch = leveldb.WriteBatch();

    for i in xrange(1000):
	    batch.Put(str(i), "hello, hello, hello string %s" %i)
    print "Print a string before WriteBatch takes effect. "
    print db.Get("888")
    print "\n"
    db.Write(batch)
    print "Print a string after WriteBatch takes effect. "
    print db.Get("888")

Snapshots
---------
Snapshots provide consistent read-only views over the entire state of the key-value store.
The following example shows how to work with ``Snapshot`` object.

.. code-block:: python

    import leveldb
    db = leveldb.LevelDB("/tmp/test-leveldb/")
    
    db.Put("hello", "world")
    db.Put("1", "111111")
    db.Put("2", "222222")
    db.Put("3", "333333")
    
    print "Creating snapshot."
    snap = leveldb.Snapshot(db)
    print "\n"
    
    print "Deleting key 1."
    db.Delete("1")
    print "\n"
    
    print "Getting key 1."
    print db.Get("1")
    print "\n"
    
    print "Applying snapshot."
    snap.Set()
    print "\n"
    
    print "Getting key 1 again."
    print db.Get("1")
    print "\n"
    
    
    print "Resetting snapshot."
    snap.Reset()
    print "\n"
    
    
    print "Getting key 1 again."
    print db.Get("1")
    print "\n"
    # Need to release the Snapshot object.
    snap.Release()
    print "O.K."

Further Reading
---------------
This overview just touches on the basics of using LevelDB from Python. For more examples,
check out the other documentation pages, and have a look at the package's test cases.
