/*
 * =====================================================================================
 *
 *       Filename:  leveldb-capi-python.c
 *
 *    Description:  python binding for leveldb C API 
 *
 *        Version:  1.0
 *        Created:  08/28/2011 11:31:39 AM
 *       Revision:  r1
 *       Compiler:  gcc
 *
 *         Author:  Fu Haiping
 *        Company:  ICT
 *
 * =====================================================================================
 */
#include <Python.h>
#include "structmember.h"
#include "leveldb/c.h"
#if !defined(_XDECREF)
#define _XDECREF(ptr) do { if ((ptr) != NULL) free((ptr));} while(0)
#else
#undef _XDECREF
#define _XDECREF(ptr) do { if ((ptr) != NULL) free((ptr));} while(0)
#endif
typedef struct {
	PyObject_HEAD
	leveldb_t *_db;
	leveldb_options_t *_options;
	leveldb_cache_t *_cache;
	leveldb_readoptions_t *_roptions;
	leveldb_writeoptions_t *_woptions;
	leveldb_env_t *_env;

	leveldb_writebatch_t *_writebatch;

	leveldb_snapshot_t *_snapshot;

	leveldb_iterator_t *_iterator;

} LevelDB;

static void LevelDB_dealloc(LevelDB* self)
{
	Py_BEGIN_ALLOW_THREADS
	_XDECREF(self->_db);
	_XDECREF(self->_options);
	_XDECREF(self->_cache);
	_XDECREF(self->_roptions);
	_XDECREF(self->_woptions);
	_XDECREF(self->_env);
	_XDECREF(self->_writebatch);
	_XDECREF(self->_snapshot);
	_XDECREF(self->_iterator);
	Py_END_ALLOW_THREADS

	self->_db = NULL;
	self->_options = NULL;
	self->_cache = NULL;
	self->_roptions = NULL;
	self->_woptions = NULL;
	self->_env = NULL;
	self->_writebatch = NULL;
	self->_snapshot = NULL;
	self->_iterator = NULL;


	self->ob_type->tp_free((PyObject *)self);
}


static PyObject* LevelDB_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	LevelDB* self = (LevelDB*)type->tp_alloc(type, 0);

	if (self != NULL) {
		self->_db = NULL;
		self->_options = NULL;
		self->_cache = NULL;
		self->_roptions = NULL;
		self->_woptions = NULL;
		self->_env = NULL;
		self->_writebatch = NULL;
		self->_snapshot = NULL;
		self->_iterator = NULL;
	}

	return (PyObject*)self;
}


static int LevelDB_init(LevelDB* self, PyObject* args, PyObject* kwds)
{
	// cleanup
	if (self->_db || self->_cache || self->_options
			|| self->_roptions || self->_woptions
			|| self->_env || self->_writebatch
			|| self->_snapshot || self->_iterator) {
		Py_BEGIN_ALLOW_THREADS
		_XDECREF(self->_db);
		_XDECREF(self->_options);
		_XDECREF(self->_cache);
		_XDECREF(self->_roptions);
		_XDECREF(self->_woptions);
		_XDECREF(self->_env);
		_XDECREF(self->_writebatch);
		_XDECREF(self->_snapshot);
		_XDECREF(self->_iterator);
		Py_END_ALLOW_THREADS
		self->_db = NULL;
		self->_options = NULL;
		self->_cache = NULL;
		self->_roptions = NULL;
		self->_roptions = NULL;
		self->_env = NULL;
		self->_writebatch = NULL;
		self->_snapshot = NULL;
		self->_iterator = NULL;
	}

	/* arguments */
	const char* db_dir = NULL;
	PyObject* create_if_missing = Py_True;
	PyObject* error_if_exists = Py_False;
	PyObject* paranoid_checks = Py_False;
	int block_cache_size = 8 * (2 << 20);
	int write_buffer_size = 4<<20;
	int block_size = 4096;
	int max_open_files = 1000;
	int block_restart_interval = 16;
	PyObject *compression = Py_False;
	const char* kwargs[] = {"filename", "create_if_missing", "error_if_exists", "paranoid_checks", "write_buffer_size", "block_size", "max_open_files", "block_restart_interval", "block_cache_size", "compression", 0};

	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s|O!O!O!iiiiiO!", (char**)kwargs,
		&db_dir,
		&PyBool_Type, &create_if_missing,
		&PyBool_Type, &error_if_exists,
		&PyBool_Type, &paranoid_checks,
		&write_buffer_size,
		&block_size,
		&max_open_files,
		&block_restart_interval,
		&block_cache_size,
		&PyBool_Type, &compression))
		return -1;

	if (write_buffer_size < 0 || block_size < 0 || max_open_files < 0 || block_restart_interval < 0 || block_cache_size < 0) {
		return -1;
	}

	/* initializing LeveldbType fields */
	self->_options = leveldb_options_create(); 
	self->_cache = leveldb_cache_create_lru(block_cache_size); 
	self->_env = leveldb_create_default_env();

	self->_roptions = leveldb_readoptions_create();
	self->_woptions = leveldb_writeoptions_create();

	self->_writebatch = leveldb_writebatch_create();
	self->_snapshot = NULL;
	self->_iterator = NULL;

	if (self->_options == NULL || self->_cache == NULL
			|| self->_roptions == NULL 
			|| self->_woptions == NULL
			|| self->_env == NULL
			|| self->_writebatch == NULL) {
		_XDECREF(self->_options);
		_XDECREF(self->_cache);
		_XDECREF(self->_roptions);
		_XDECREF(self->_woptions);
		_XDECREF(self->_env);
		_XDECREF(self->_writebatch);

		self->_options = NULL;
		self->_cache = NULL;
		self->_roptions = NULL;
		self->_woptions = NULL;
		self->_env = NULL;
		self->_writebatch = NULL;
		return -1;
	}
	leveldb_options_set_create_if_missing(self->_options, (create_if_missing == Py_True) ? 1 : 0);
	leveldb_options_set_error_if_exists(self->_options, (error_if_exists == Py_True) ? 1 : 0);
	leveldb_options_set_paranoid_checks(self->_options, (paranoid_checks == Py_True) ? 1 : 0);
	leveldb_options_set_write_buffer_size(self->_options, write_buffer_size);
	leveldb_options_set_block_size(self->_options, block_size);
	leveldb_options_set_cache(self->_options, self->_cache);
	leveldb_options_set_max_open_files(self->_options, max_open_files);
	leveldb_options_set_block_restart_interval(self->_options, 8);
	leveldb_options_set_compression(self->_options, (compression == Py_True) ? 1 : 0);
	leveldb_options_set_env(self->_options, self->_env);
	leveldb_options_set_info_log(self->_options, NULL);
	
	char *err = NULL;

	Py_BEGIN_ALLOW_THREADS
	self->_db = leveldb_open(self->_options, db_dir, &err);

	if (err != NULL) {
		_XDECREF(self->_db);
		_XDECREF(self->_options);
		_XDECREF(self->_cache);
		_XDECREF(self->_roptions);
		_XDECREF(self->_woptions);
		_XDECREF(self->_env);
		_XDECREF(self->_writebatch);
		self->_db = NULL;
		self->_options = NULL;
		self->_cache = NULL;
		self->_roptions = NULL;
		self->_roptions = NULL;
		self->_env = NULL;
		self->_writebatch = NULL;
	
		fprintf(stderr, "error occurs in opening leveldb:\n\t%s\n", err);
	}
	Py_END_ALLOW_THREADS
	return 0;
}
#define LEVELDB_DEFINE_KVBUF(buf) const char * s_##buf = NULL; size_t i_##buf

static PyObject* LevelDB_Put(LevelDB* self, PyObject* args, PyObject* kwds)
{
	const char* kwargs[] = {"key", "value", "sync", 0};
	PyObject* sync = Py_False;

	char *err = NULL;
	LEVELDB_DEFINE_KVBUF(key);
	LEVELDB_DEFINE_KVBUF(value);


	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s#s#|O!", (char**)kwargs, &s_key, &i_key, &s_value, &i_value, &PyBool_Type, &sync)) {
		return 0;
	}

	printf("in Put: key = %s, value = %s\n", s_key, s_value);
	Py_BEGIN_ALLOW_THREADS
	leveldb_writeoptions_set_sync(self->_woptions, (sync == Py_True) ? 1 : 0);

	leveldb_put(self->_db, self->_woptions, (const char *)s_key, (size_t)i_key, (const char *)s_value, (size_t)i_value, &err);
	Py_END_ALLOW_THREADS
	if (err != NULL) {
		fprintf(stderr, "error occurs when put:\n\t%s\n", err);
	}

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject* LevelDB_Get(LevelDB* self, PyObject* args, PyObject* kwds)
{
	PyObject* verify_checksums = Py_False;
	PyObject* fill_cache = Py_True;
	const char* kwargs[] = {"key", "verify_checksums", "fill_cache", 0};

	char *value;
	size_t value_len;
	char *err = NULL;
	LEVELDB_DEFINE_KVBUF(key);

	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s#|O!O!", (char**)kwargs, &s_key, &i_key, &PyBool_Type, &verify_checksums, &PyBool_Type, &fill_cache))
		return 0;

	Py_BEGIN_ALLOW_THREADS

	leveldb_readoptions_set_verify_checksums(self->_roptions, (verify_checksums == Py_True) ? 1 : 0);
	leveldb_readoptions_set_fill_cache(self->_roptions, (fill_cache == Py_True) ? 1 : 0);

	value = leveldb_get(self->_db, self->_roptions, (const char *)s_key, (size_t)i_key, &value_len, &err);
	Py_END_ALLOW_THREADS

	if (err != NULL) {
		fprintf(stderr, "error occurs get:\n\t%s\n", err);
	}
	return PyString_FromStringAndSize(value, value_len);
}

static PyObject* LevelDB_Delete(LevelDB* self, PyObject* args, PyObject* kwds)
{
	PyObject* sync = Py_False;
	const char* kwargs[] = {"key", "sync", 0};

	LEVELDB_DEFINE_KVBUF(key);	


	char *err = NULL;
	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s#|O!", (char**)kwargs, &s_key, &i_key, &PyBool_Type, &sync))
		return 0;
	
	leveldb_writeoptions_set_sync(self->_woptions, (sync == Py_True) ? 1 : 0);

	Py_BEGIN_ALLOW_THREADS

	leveldb_delete(self->_db, self->_woptions, (const char *)s_key, (size_t)i_key, &err);

	Py_END_ALLOW_THREADS


	if (err != NULL) {
		fprintf(stderr, "error occurs when delete:\n\t%s\n", err);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * LevelDB_WriteBatch_Put(LevelDB *self, PyObject *args)
{

	LEVELDB_DEFINE_KVBUF(key);
	LEVELDB_DEFINE_KVBUF(value);

	if (!PyArg_ParseTuple(args, (char*)"t#t#", &s_key, &i_key, &s_value, &i_value))
		return 0;

	leveldb_writebatch_put(self->_writebatch, (const char *)s_key, (size_t)i_key, (const char *)s_value, (size_t)i_value);

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject * LevelDB_WriteBatch_Delete(LevelDB *self, PyObject *args)
{

	LEVELDB_DEFINE_KVBUF(key);

	if (!PyArg_ParseTuple(args, (char*)"t#", &s_key, &i_key))
		return 0;

	leveldb_writebatch_delete(self->_writebatch, (const char *)s_key, (size_t)i_key);

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject * LevelDB_Property(LevelDB *self, PyObject *args)
{

	const char *propname = NULL;
	char *propvalue;
	PyObject *result;

	if (!PyArg_ParseTuple(args, (char*)"s", &propname))
		return 0;
	propvalue = leveldb_property_value(self->_db, propname);
	if (propvalue != NULL) {
		result = Py_BuildValue("s#", propvalue, strlen(propvalue));
		return result;
	} else {
		fprintf(stderr, "No such property.\n");
	}
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * LevelDB_WriteBatch_Clear(LevelDB *self, PyObject *args)
{
	leveldb_writebatch_clear(self->_writebatch);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * LevelDB_Write(LevelDB *self, PyObject *args)
{
	char *err = NULL;
	leveldb_write(self->_db, self->_woptions, self->_writebatch, &err);
	if (err != NULL) {
		fprintf(stderr, "error occurs in leveldb write :\n\t%s\n", err);
	}
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject * LevelDB_Close(LevelDB *self, PyObject *args)
{
	leveldb_close(self->_db);
	leveldb_options_destroy(self->_options);
	leveldb_readoptions_destroy(self->_roptions);
	leveldb_writeoptions_destroy(self->_woptions);
	leveldb_cache_destroy(self->_cache);
	leveldb_env_destroy(self->_env);
	leveldb_writebatch_destroy(self->_writebatch);
	Py_INCREF(Py_None);
	return Py_None;
}



static PyObject *LevelDB_RepairDB(LevelDB* self, PyObject* args, PyObject* kwds)
{
	const char* db_dir = NULL;
	PyObject* create_if_missing = Py_False;
	PyObject* error_if_exists = Py_False;
	char *err = NULL;
	const char* kwargs[] = {"filename", "create_if_missing", "error_if_exists", 0};

	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s|O!O!", (char**)kwargs,
		&db_dir,
		&PyBool_Type, &create_if_missing,
		&PyBool_Type, &error_if_exists))
		return -1;
	leveldb_options_set_create_if_missing(self->_options, (create_if_missing == Py_True) ? 1 : 0);
	leveldb_options_set_error_if_exists(self->_options, (error_if_exists == Py_True) ? 1 : 0);

	Py_BEGIN_ALLOW_THREADS
	leveldb_close(self->_db);
	leveldb_repair_db(self->_options, db_dir, &err);
	if (err != NULL) {
		fprintf(stderr, "error occurs in repair_db:\n\t%s\n", err);
	}

	self->_db = leveldb_open(self->_options, db_dir, &err);
	if (err != NULL) {
		_XDECREF(self->_db);
		_XDECREF(self->_options);
		_XDECREF(self->_cache);
		_XDECREF(self->_roptions);
		_XDECREF(self->_woptions);
		_XDECREF(self->_env);
		_XDECREF(self->_writebatch);
		_XDECREF(self->_snapshot);
		_XDECREF(self->_iterator);

		self->_db = NULL;
		self->_options = NULL;
		self->_cache = NULL;
		self->_roptions = NULL;
		self->_roptions = NULL;
		self->_env = NULL;
		self->_writebatch = NULL;
		self->_snapshot = NULL;
		self->_iterator = NULL;
		fprintf(stderr, "error occurs in opening leveldb:\n\t%s\n", err);
	}
	Py_END_ALLOW_THREADS
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject * LevelDB_Create_Snapshot(LevelDB *self, PyObject *args)
{
	const leveldb_snapshot_t *snapshot;
	snapshot = leveldb_create_snapshot(self->_db);
	self->_snapshot = snapshot;
	if (self->_snapshot == NULL) {
		fprintf(stderr, "Failed to create snapshot\n");
	}
	Py_INCREF(Py_None);
	return Py_None;
}



static PyObject * LevelDB_Active_Snapshot(LevelDB *self, PyObject *args)
{
	leveldb_readoptions_set_snapshot(self->_roptions, self->_snapshot);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * LevelDB_Reset_Snapshot(LevelDB *self, PyObject *args)
{
	leveldb_readoptions_set_snapshot(self->_roptions, NULL);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * LevelDB_Release_Snapshot(LevelDB *self, PyObject *args)
{
	leveldb_release_snapshot(self->_db, self->_snapshot);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * LevelDB_CreateIter(LevelDB *self, PyObject *args)
{
	self->_iterator = leveldb_create_iterator(self->_db, self->_roptions);
	if(self->_iterator == NULL) {
		fprintf(stderr, "Failed to create iterator\n");
	}
	Py_INCREF(Py_None);
	return Py_None;
}



static PyObject * LevelDB_Iter_Valid(LevelDB *self, PyObject *args)
{
	PyObject *result;
	unsigned char valid;
	valid = leveldb_iter_valid(self->_iterator);
	result = Py_BuildValue("b", valid);
	return result;
}

static PyObject * LevelDB_Iter_Seek_To_First(LevelDB *self, PyObject *args)
{
	leveldb_iter_seek_to_first(self->_iterator);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * LevelDB_Iter_Seek_To_Last(LevelDB *self, PyObject *args)
{
	leveldb_iter_seek_to_last(self->_iterator);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * LevelDB_Iter_Next(LevelDB *self, PyObject *args)
{
	leveldb_iter_next(self->_iterator);
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject * LevelDB_Iter_Prev(LevelDB *self, PyObject *args)
{
	leveldb_iter_prev(self->_iterator);
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject * LevelDB_Iter_Seek(LevelDB *self, PyObject *args)
{

	LEVELDB_DEFINE_KVBUF(key);

	if (!PyArg_ParseTuple(args, (char*)"t#", &s_key, &i_key))
		return 0;

	leveldb_iter_seek(self->_iterator, s_key, i_key);
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject * LevelDB_Iter_Get_Error(LevelDB *self, PyObject *args)
{

	PyObject *result;
	char *err = NULL;

	leveldb_iter_get_error(self->_iterator, &err);
	result = Py_BuildValue("s#", err, strlen(err));
	_XDECREF(err);
	return result;
}



static PyObject * LevelDB_Iter_Key(LevelDB *self, PyObject *args)
{

	PyObject *result;

	const char *key;
	size_t len;
	key = leveldb_iter_key(self->_iterator, &len);
	result = Py_BuildValue("s#", key, len);
	return result;
}


static PyObject * LevelDB_Iter_Value(LevelDB *self, PyObject *args)
{

	PyObject *result;

	const char *value;
	size_t len;
	value = leveldb_iter_value(self->_iterator, &len);
	result = Py_BuildValue("s#", value, len);
	return result;
}
static PyMethodDef LevelDB_methods[] = {
	{(char*)"Put",       (PyCFunction)LevelDB_Put,       METH_KEYWORDS, (char*)"add a key/value pair to database, with an optional synchronous disk write" },
	{(char*)"Get",       (PyCFunction)LevelDB_Get,       METH_KEYWORDS, (char*)"get a value from the database" },
	{(char*)"Delete",    (PyCFunction)LevelDB_Delete,    METH_KEYWORDS, (char*)"delete a value in the database" },
	{(char*)"Write",    (PyCFunction)LevelDB_Write,    METH_KEYWORDS, (char*)"apply a writebatch in database" },
	{(char*)"Property",    (PyCFunction)LevelDB_Property,    METH_KEYWORDS, (char*)"get a property value" },
	{(char*)"RepairDB",    (PyCFunction)LevelDB_RepairDB,    METH_KEYWORDS, (char*)"repair database" },

	{(char*)"CreateIter",    (PyCFunction)LevelDB_CreateIter,    METH_KEYWORDS, (char*)"create iterator" },
	{(char*)"IterValid",    (PyCFunction)LevelDB_Iter_Valid,    METH_KEYWORDS, (char*)"validate iterator" },
	{(char*)"IterFirst",    (PyCFunction)LevelDB_Iter_Seek_To_First,    METH_KEYWORDS, (char*)"seek to first" },
	{(char*)"IterLast",    (PyCFunction)LevelDB_Iter_Seek_To_Last,    METH_KEYWORDS, (char*)"seek to last" },
	{(char*)"IterSeek",    (PyCFunction)LevelDB_Iter_Seek,    METH_KEYWORDS, (char*)"iterator seek" },
	{(char*)"IterNext",    (PyCFunction)LevelDB_Iter_Next,    METH_KEYWORDS, (char*)"iterator to next" },
	{(char*)"IterPrev",    (PyCFunction)LevelDB_Iter_Prev,    METH_KEYWORDS, (char*)"iterator to prev" },
	{(char*)"IterKey",    (PyCFunction)LevelDB_Iter_Key,    METH_KEYWORDS, (char*)"get key throuth current iterator" },
	{(char*)"IterValue",    (PyCFunction)LevelDB_Iter_Value,    METH_KEYWORDS, (char*)"get value throuth current iterator" },
	{(char*)"IterGetError",    (PyCFunction)LevelDB_Iter_Get_Error,    METH_KEYWORDS, (char*)"get iterator error" },

	{(char*)"CreateSnapshot",    (PyCFunction)LevelDB_Create_Snapshot,    METH_KEYWORDS, (char*)"create a snapshot" },
	{(char*)"ActiveSnapshot",    (PyCFunction)LevelDB_Active_Snapshot,    METH_KEYWORDS, (char*)"active snapshot" },
	{(char*)"ResetSnapshot",    (PyCFunction)LevelDB_Reset_Snapshot,    METH_KEYWORDS, (char*)"reset snapshot" },
	{(char*)"ReleaseSnapshot",    (PyCFunction)LevelDB_Release_Snapshot,    METH_KEYWORDS, (char*)"release snapshot" },

	{(char*)"WriteBatchPut",    (PyCFunction)LevelDB_WriteBatch_Put,    METH_KEYWORDS, (char*)"add a put to writebatch" },
	{(char*)"WriteBatchDelete",    (PyCFunction)LevelDB_WriteBatch_Delete,    METH_KEYWORDS, (char*)"delete a put to writebatch" },
	{(char*)"WriteBatchClear",    (PyCFunction)LevelDB_WriteBatch_Clear,    METH_KEYWORDS, (char*)"clear a writebatch" },

	{(char*)"Close",    (PyCFunction)LevelDB_Close,    METH_KEYWORDS, (char*)"close database" },
	{NULL}
};
PyTypeObject LevelDBType = {
	PyObject_HEAD_INIT(NULL)
	0,                             /*ob_size*/
	(char*)"leveldb.LevelDB",      /*tp_name*/
	sizeof(LevelDB),             /*tp_basicsize*/
	0,                             /*tp_itemsize*/
	(destructor)LevelDB_dealloc, /*tp_dealloc*/
	0,                             /*tp_print*/
	0,                             /*tp_getattr*/
	0,                             /*tp_setattr*/
	0,                             /*tp_compare*/
	0,                             /*tp_repr*/
	0,                             /*tp_as_number*/
	0,                             /*tp_as_sequence*/
	0,                             /*tp_as_mapping*/
	0,                             /*tp_hash */
	0,                             /*tp_call*/
	0,                             /*tp_str*/
	0,                             /*tp_getattro*/
	0,                             /*tp_setattro*/
	0,                             /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,            /*tp_flags*/
	(char*)"LevelDB bindings",   /*tp_doc */
	0,                             /*tp_traverse */
	0,                             /*tp_clear */
	0,                             /*tp_richcompare */
	0,                             /*tp_weaklistoffset */
	0,                             /*tp_iter */
	0,                             /*tp_iternext */
	LevelDB_methods,             /*tp_methods */
	0,                             /*tp_members */
	0,                             /*tp_getset */
	0,                             /*tp_base */
	0,                             /*tp_dict */
	0,                             /*tp_descr_get */
	0,                             /*tp_descr_set */
	0,                             /*tp_dictoffset */
	(initproc)LevelDB_init,      /*tp_init */
	0,                             /*tp_alloc */
	LevelDB_new,                 /*tp_new */
};

#define LevelDB_Check(op) PyObject_TypeCheck(op, &LevelDBType)

PyMODINIT_FUNC
initleveldb(void)
{
	PyObject* leveldb_module = Py_InitModule3((char*)"leveldb", LevelDB_methods, 0);

	if (leveldb_module == 0)
		return;

	if (PyType_Ready(&LevelDBType) < 0)
		return;


	// add custom types to the different modules
	Py_INCREF(&LevelDBType);

	if (PyModule_AddObject(leveldb_module, (char*)"LevelDB", (PyObject*)&LevelDBType) != 0)
		return;

}
