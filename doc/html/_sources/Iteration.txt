Iteration Object
================

Simple Usage
------------

Iterator API
------------

.. method:: LevelDB.Iterator(leveldb)

Construct an iterator based on the current LevelDB state.


.. method:: Iterator.Validate()
Validate whether an iterator is valid, it is useful to verify an iterator
exceeding out of bounds.

.. method:: Iterator.First()

Seek to *First* key-value pair.

.. method:: Iterator.Last()

Seek to *Last* key-value pair.

.. method:: Iterator.Seek()

Iterator seek, find the specified ``key``.

.. method:: Iterator.Next()

Iterator to Next.

.. method:: Iterator.Prev()

Iterator to Prev.

.. method:: Iterator.Key()

Get key throuth current iterator.

.. method:: Iterator.Value()

Get value throuth current iterator.

.. method:: Iterator.GetError()
Get iterator error.

.. method:: Iterator.Destroy()

Destroy iterator.

