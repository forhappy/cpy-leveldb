Snapshots Object
================

Snapshots provide consistent read-only views over the entire state of the key-value store.

Simple Usage
------------

Snapshots API
-------------

.. method:: LevelDB.Snapshot(db)

Construct a snapshot based on the current LevelDB state.

.. method:: Snapshot.Set()

Set snapshot.

.. method:: Snapshot.Reset()

Reset snapshot to the current state.

.. method:: Snapshot.Release()

Release snapshot

