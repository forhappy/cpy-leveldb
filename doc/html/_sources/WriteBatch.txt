WriteBatch Object
=================

Simple Usage
------------


WriteBatch API
--------------

.. method:: LevelDB.WriteBatch()

Construct a write batch based on the current database state.

.. method:: WriteBatch.Put()

Add a ``Put`` operation to batch.

.. method:: WriteBatch.Delete()

Add a ``Delete`` operation to batch.

.. method:: WriteBatch.Clear()

Clear the current batch.

.. method:: WriteBatch.Release()

Release a batch.



