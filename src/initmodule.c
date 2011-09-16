/*
 * =====================================================================================
 *
 *       Filename:  initmodule.c
 *
 *    Description:  python binding for leveldb based on c api.
 *
 *        Version:  0.3.0
 *        Created:  09/16/2011 08:18:48 PM
 *       Revision:  r16 
 *       Compiler:  gcc
 *
 *         Author:  Fu Haiping <haipingf@gmail.com> 
 *        Company:  ICT
 *
 * =====================================================================================
 */

#include "leveldb.h"
#include "write_batch.h"
#include "iterator.h"
#include "snapshot.h"
#include "comparator.h"

PyObject *LevelDBError = NULL;

PyMODINIT_FUNC
initleveldb(void)
{
	VERBOSE( "%s", "Hello, Entering initleveldb.\n");
	PyObject *dict, *value;
	PyObject* leveldb_module = Py_InitModule3((char*)"leveldb", LevelDB_methods, 0);

	if (leveldb_module == 0)
		return;

	if (PyType_Ready(&LevelDBType) < 0)
		return;

	if (PyType_Ready(&WriteBatchType) < 0)
		return;

	if (PyType_Ready(&SnapshotType) < 0)
		return;

	if (PyType_Ready(&IteratorType) < 0)
		return;

//	if (PyType_Ready(&ComparatorType) < 0)
//		return;

	dict = PyModule_GetDict(leveldb_module);

	value = PyString_FromString("Fu Haiping <haipingf@gmail.com>");
	PyDict_SetItemString(dict, "__author__", value);
	Py_DECREF(value);

	value = PyString_FromString(CPY_LEVELDB_MODULE_VERSION);
	PyDict_SetItemString(dict, "__version__", value);
	Py_DECREF(value);


	value = PyString_FromString(CPY_LEVELDB_VERSION_DATE);
	PyDict_SetItemString(dict, "__date__", value);
	Py_DECREF(value);

	LevelDBError = PyErr_NewException((char *)"leveldb.LevelDBError", NULL, NULL);
	if (LevelDBError == NULL) {
		PyErr_Format(LevelDBError, "Failed to create LevelDBError.\n");
	}
	// add custom types to the different modules
	Py_INCREF(&LevelDBType);
	if (PyModule_AddObject(leveldb_module, (char*)"LevelDB", (PyObject*)&LevelDBType) != 0)
		return;

	Py_INCREF(&WriteBatchType);
	if (PyModule_AddObject(leveldb_module, (char*)"WriteBatch", (PyObject*)&WriteBatchType) != 0)
		return;

	Py_INCREF(&SnapshotType);
	if (PyModule_AddObject(leveldb_module, (char*)"Snapshot", (PyObject*)&SnapshotType) != 0)
		return;

	Py_INCREF(&IteratorType);
	if (PyModule_AddObject(leveldb_module, (char*)"Iterator", (PyObject*)&IteratorType) != 0)
		return;
//
//	Py_INCREF(&ComparatorType);
//	if (PyModule_AddObject(leveldb_module, (char*)"Comparator", (PyObject*)&ComparatorType) != 0)
//		return;

	Py_INCREF(LevelDBError);
	if (PyModule_AddObject(leveldb_module, (char *)"LevelDBError", LevelDBError) != 0)
		return;
	VERBOSE("%s", "Reaching end of initleveldb.\n");
}
/* 
 * vim:ts=4:sw=4
 **/
