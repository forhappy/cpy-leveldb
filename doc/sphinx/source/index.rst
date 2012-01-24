cPy-LevelDB Documentation
=========================


The cPy-LevelDB is a python binding for Google's LevelDB.
It is written in pure C and based on LevelDB's c API. The goal is to be super strict 
for ultimate portability, no dependencies, and generic embeddability.

The binding is still considered alpha but is undergoing active
development. 

:doc:`Tutorial`
    An overview of the python API for LevelDB.

:doc:`Building`
    How to build the binding from source code.

:doc:`LevelDB`
    How to use LevelDB object.

:doc:`Iteration`
    How to work with key-value iterator.

:doc:`Snapshots`
    Snapshots provide consistent read-only views over the entire state of the key-value store.

:doc:`WriteBatch`
    The WriteBatch holds a sequence of edits to be made to the database, and these edits within the batch are applied in order.

`API Docs <api/index.html>`_
    Doxygen-generated API docs.

`Source code <http://github.com/forhappy/cpy-leveldb>`_
    The source code is hosted on GitHub.

.. toctree::
   :maxdepth: 3 

   Tutorial
   Building
   LevelDB
   Iteration
   Snapshots
   WriteBatch
