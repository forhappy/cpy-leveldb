LevelDB Object
==============


Simple Usage
------------


LevelDB API
------------
.. method:: leveldb.LevelDB(filename, [create_if_missing, error_if_exists, paranoid_checks, write_buffer_size, block_size, max_open_files, block_restart_interval, block_cache_size, compression])

Construct a connection to LevelDB datebase.Here are the parameters explaination:

 ``filename``: Specifing the datebase filename.

 ``create_if_missing``: If the datebase specified by ``filename`` does not exists, then create a new datebase.

 ``error_if_exists``: If the datebase specified by ``filename`` exits, then error occurs.
 
 ``paranoid_checks``: Period checks.

 ``write_buffer_size``: Specifing the write buffer size.

 ``block_size``: Specifing the block size.

 ``max_open_files``: Specifing the max number of files that can be opened.

 ``block_restart_interval``: Specifing the block restart interval.

 ``block_cache_size``: Specifing the block cache size.

 ``compression``: Specifing data compressed or not.

.. method:: LevelDB.Put(key, value[, sync])

Add a key/value pair to database, with an optional synchronous disk write.

.. method:: LevelDB.Get(key[, verify_checksums, fill_cache])

Get a value from the database.

.. method:: LevelDB.Delete(key[, sync])

Delete a value in the database.

.. method:: LevelDB.Write(writebach[, sync])

Apply a writebatch in database.

.. method:: LevelDB.Property()

Get a property value.

.. method:: LevelDB.RepairDB(filename[, create_if_missing, error_if_exists])

Repair database.

.. method:: LevelDB.RangeIter(key_from, key_to[, verify_checksums, fill_cache, include_value])

Range iterator.

.. method:: LevelDB.GetApproximateSizes(num_ranges, range_start_key, range_limit_key)

Get approximate sizes.

.. method:: LevelDB.Close()

Close database.

