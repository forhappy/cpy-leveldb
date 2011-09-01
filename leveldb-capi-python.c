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

#define LEVELDB_DEFINE_KVBUF(buf) const char * s_##buf = NULL; size_t i_##buf

PyTypeObject WriteBatchType;
PyTypeObject LevelDBType;
PyTypeObject SnapshotType;
PyTypeObject IteratorType;

typedef struct {
	PyObject_HEAD
	leveldb_t *_db;
	leveldb_options_t *_options;
	leveldb_cache_t *_cache;
	leveldb_env_t *_env;
} LevelDB;

typedef struct {
	PyObject_HEAD
	leveldb_writebatch_t *_writebatch;
}WriteBatch;

typedef struct {
	PyObject_HEAD
	const leveldb_snapshot_t *_snapshot;
} Snapshot;

typedef struct {
	PyObject_HEAD
	leveldb_iterator_t *_iterator;
} Iterator;

static void LevelDB_dealloc(LevelDB* self)
{
	Py_BEGIN_ALLOW_THREADS
	_XDECREF(self->_db);
	_XDECREF(self->_options);
	_XDECREF(self->_cache);
	_XDECREF(self->_env);
	Py_END_ALLOW_THREADS

	self->_db = NULL;
	self->_options = NULL;
	self->_cache = NULL;
	self->_env = NULL;

	self->ob_type->tp_free((PyObject *)self);
}


static void WriteBatch_dealloc(WriteBatch* self)
{
	Py_BEGIN_ALLOW_THREADS

	_XDECREF(self->_writebatch);

	Py_END_ALLOW_THREADS

	self->_writebatch = NULL;

	self->ob_type->tp_free(self);
}


static void Snapshot_dealloc(Snapshot *self)
{
	Py_BEGIN_ALLOW_THREADS

	_XDECREF((void *)self->_snapshot);

	Py_END_ALLOW_THREADS

	self->_snapshot = NULL;

	self->ob_type->tp_free(self);
}

static void Iterator_dealloc(Iterator *self)
{
	Py_BEGIN_ALLOW_THREADS

	_XDECREF(self->_iterator);

	Py_END_ALLOW_THREADS

	self->_iterator = NULL;

	self->ob_type->tp_free(self);
}


static PyObject* LevelDB_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	LevelDB* self = (LevelDB *)type->tp_alloc(type, 0);

	if (self != NULL) {
		self->_db = NULL;
		self->_options = NULL;
		self->_cache = NULL;
		self->_env = NULL;
	}

	return (PyObject*)self;
}

static PyObject* WriteBatch_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	WriteBatch* self = (WriteBatch *)type->tp_alloc(type, 0);

	if (self != NULL) {
		self->_writebatch = NULL;
	}
	return (PyObject*)self;
}

static PyObject* Snapshot_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	Snapshot * self = (Snapshot *)type->tp_alloc(type, 0);

	if (self != NULL) {
		self->_snapshot= NULL;
	}
	return (PyObject*)self;
}


static PyObject* Iterator_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	Iterator * self = (Iterator *)type->tp_alloc(type, 0);

	if (self != NULL) {
		self->_iterator= NULL;
	}
	return (PyObject*)self;
}

static int LevelDB_init(LevelDB* self, PyObject* args, PyObject* kwds)
{
	// cleanup
	if (self->_db || self->_cache || self->_options
			|| self->_env ) {
		Py_BEGIN_ALLOW_THREADS
		_XDECREF(self->_db);
		_XDECREF(self->_options);
		_XDECREF(self->_cache);
		_XDECREF(self->_env);
		Py_END_ALLOW_THREADS
		self->_db = NULL;
		self->_options = NULL;
		self->_cache = NULL;
		self->_env = NULL;
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

	/* initializing LevelDBType fields */
	self->_options = leveldb_options_create(); 
	self->_cache = leveldb_cache_create_lru(block_cache_size); 
	self->_env = leveldb_create_default_env();

	if (self->_options == NULL || self->_cache == NULL
			|| self->_env == NULL) {
		_XDECREF(self->_options);
		_XDECREF(self->_cache);
		_XDECREF(self->_env);

		self->_options = NULL;
		self->_cache = NULL;
		self->_env = NULL;
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
		_XDECREF(self->_env);

		self->_db = NULL;
		self->_options = NULL;
		self->_cache = NULL;
		self->_env = NULL;
		
		fprintf(stderr, "error occurs in opening leveldb:\n\t%s\n", err);
	}
	Py_END_ALLOW_THREADS
	return 0;
}

static int WriteBatch_init(WriteBatch* self, PyObject* args, PyObject* kwds)
{
	static char* kwargs[] = {0};

	if (self->_writebatch) {
		Py_BEGIN_ALLOW_THREADS
		_XDECREF(self->_writebatch);
		Py_END_ALLOW_THREADS
		self->_writebatch = NULL;
	}
	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"", kwargs))
		return -1;

	self->_writebatch = leveldb_writebatch_create();
	if (self->_writebatch == NULL) {
		fprintf(stderr, "Failed to create writebatch.\n");
	}

	return 0;
}

static int Snapshot_init(Snapshot* self, PyObject* args, PyObject* kwds)
{
	static char* kwargs[] = {"leveldb", 0};
	LevelDB *leveldb = NULL;
	const leveldb_snapshot_t *snapshot = NULL;

	if (self->_snapshot) {
		Py_BEGIN_ALLOW_THREADS
		_XDECREF((void *)self->_snapshot);
		Py_END_ALLOW_THREADS
		
		self->_snapshot= NULL;
	}
	if (!PyArg_ParseTupleAndKeywords(args, kwds, (const char*)"O!", kwargs, &LevelDBType, &leveldb))
		return -1;
	if (leveldb != NULL) {
		printf("Snapshot_init executed.\n");
		snapshot = leveldb_create_snapshot(leveldb->_db);
		self->_snapshot = snapshot;
	}
	if (self->_snapshot== NULL) {
		fprintf(stderr, "Failed to create snapshot.\n");
	}

	return 0;
}

static int Iterator_init(Iterator *self, PyObject* args, PyObject* kwds)
{
	static char* kwargs[] = {"leveldb", 0};
	LevelDB *leveldb = NULL;
	leveldb_iterator_t *iterator = NULL;
	leveldb_readoptions_t *roptions = NULL;

	if (self->_iterator) {
		Py_BEGIN_ALLOW_THREADS
		_XDECREF(self->_iterator);
		Py_END_ALLOW_THREADS
		
		self->_iterator= NULL;
	}

	roptions = leveldb_readoptions_create();
	if (roptions == NULL) {
		fprintf(stderr, "Failed to create readoptions.\n");
	}

	if (!PyArg_ParseTupleAndKeywords(args, kwds, (const char*)"O!", kwargs, &LevelDBType, &leveldb))
		return -1;
	if (leveldb != NULL) {
		printf("Iterator_init executed.\n");
		iterator = leveldb_create_iterator(leveldb->_db, roptions);
		self->_iterator = iterator;
	}
	if (self->_iterator == NULL) {
		fprintf(stderr, "Failed to create snapshot.\n");
	}

	return 0;
}
static PyObject* LevelDB_Put(LevelDB* self, PyObject* args, PyObject* kwds)
{
	const char* kwargs[] = {"key", "value", "sync", 0};
	PyObject* sync = Py_False;
	char *err = NULL;
	leveldb_writeoptions_t *woptions = NULL;

	LEVELDB_DEFINE_KVBUF(key);
	LEVELDB_DEFINE_KVBUF(value);


	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s#s#|O!", (char**)kwargs, &s_key, &i_key, &s_value, &i_value, &PyBool_Type, &sync)) {
		return -1;
	}

	woptions = leveldb_writeoptions_create();
	if (woptions == NULL) {
		fprintf(stderr, "Failed to create writeoptions.\n");
	}

	Py_BEGIN_ALLOW_THREADS

	leveldb_writeoptions_set_sync(woptions, (sync == Py_True) ? 1 : 0);
	leveldb_put(self->_db, woptions, (const char *)s_key, (size_t)i_key, (const char *)s_value, (size_t)i_value, &err);

	Py_END_ALLOW_THREADS

	if (err != NULL) {
		fprintf(stderr, "error occurs when put:\n\t%s\n", err);
	}

	leveldb_writeoptions_destroy(woptions);
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject* LevelDB_Get(LevelDB* self, PyObject* args, PyObject* kwds)
{
	PyObject* verify_checksums = Py_False;
	PyObject* fill_cache = Py_True;
	leveldb_readoptions_t *roptions = NULL;

	const char* kwargs[] = {"key", "verify_checksums", "fill_cache", 0};
	char *value;
	size_t value_len;
	char *err = NULL;

	roptions  = leveldb_readoptions_create();
	if (roptions == NULL) {
		fprintf(stderr, "Failed to create readoptions.\n");
	}
	LEVELDB_DEFINE_KVBUF(key);

	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s#|O!O!", (char**)kwargs, &s_key, &i_key, &PyBool_Type, &verify_checksums, &PyBool_Type, &fill_cache))
		return -1;

	Py_BEGIN_ALLOW_THREADS

	leveldb_readoptions_set_verify_checksums(roptions, (verify_checksums == Py_True) ? 1 : 0);
	leveldb_readoptions_set_fill_cache(roptions, (fill_cache == Py_True) ? 1 : 0);
	value = leveldb_get(self->_db, roptions, (const char *)s_key, (size_t)i_key, &value_len, &err);

	Py_END_ALLOW_THREADS

	if (err != NULL) {
		fprintf(stderr, "error occurs get:\n\t%s\n", err);
	}

	leveldb_readoptions_destroy(roptions);

	return PyString_FromStringAndSize(value, value_len);
}

static PyObject* LevelDB_Delete(LevelDB* self, PyObject* args, PyObject* kwds)
{
	PyObject* sync = Py_False;
	const char* kwargs[] = {"key", "sync", 0};
	leveldb_writeoptions_t * woptions = NULL;

	LEVELDB_DEFINE_KVBUF(key);	

	char *err = NULL;
	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s#|O!", (char**)kwargs, &s_key, &i_key, &PyBool_Type, &sync))
		return -1;
	
	woptions = leveldb_writeoptions_create();
	if (woptions == NULL) {
		fprintf(stderr, "Failed to create writeoptions.\n");
	}
	leveldb_writeoptions_set_sync(woptions, (sync == Py_True) ? 1 : 0);

	Py_BEGIN_ALLOW_THREADS

	leveldb_delete(self->_db, woptions, (const char *)s_key, (size_t)i_key, &err);

	Py_END_ALLOW_THREADS


	if (err != NULL) {
		fprintf(stderr, "error occurs when delete:\n\t%s\n", err);
	}
	leveldb_writeoptions_destroy(woptions);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * LevelDB_Write(LevelDB *self, PyObject *args, PyObject *kwds)
{
	const char *kwargs[] = {"writebatch", "sync", 0};
	char *err = NULL;
	WriteBatch *batch = NULL;
	PyObject *sync = Py_False;
	leveldb_writeoptions_t *woptions = NULL;

	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char *)"O!|O!", (char **)kwargs, &WriteBatchType, &batch, &PyBool_Type, &sync))
		return -1;

	woptions = leveldb_writeoptions_create();
	if (woptions == NULL) {
		fprintf(stderr, "Failed to create writeoptions.\n");
	}
	leveldb_writeoptions_set_sync(woptions, (sync == Py_True) ? 1 : 0);

	leveldb_write(self->_db, woptions, batch->_writebatch, &err);
	if (err != NULL) {
		fprintf(stderr, "error occurs in leveldb write :\n\t%s\n", err);
	}

	leveldb_writeoptions_destroy(woptions);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * LevelDB_Close(LevelDB *self, PyObject *args)
{
	leveldb_close(self->_db);
	leveldb_options_destroy(self->_options);
	leveldb_cache_destroy(self->_cache);
	leveldb_env_destroy(self->_env);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * WriteBatch_Put(WriteBatch *self, PyObject *args)
{

	LEVELDB_DEFINE_KVBUF(key);
	LEVELDB_DEFINE_KVBUF(value);

	if (!PyArg_ParseTuple(args, (char*)"t#t#", &s_key, &i_key, &s_value, &i_value))
		return -1;

	leveldb_writebatch_put(self->_writebatch, (const char *)s_key, (size_t)i_key, (const char *)s_value, (size_t)i_value);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * WriteBatch_Delete(WriteBatch *self, PyObject *args)
{

	LEVELDB_DEFINE_KVBUF(key);

	if (!PyArg_ParseTuple(args, (char*)"t#", &s_key, &i_key))
		return -1;

	leveldb_writebatch_delete(self->_writebatch, (const char *)s_key, (size_t)i_key);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * WriteBatch_Clear(WriteBatch *self, PyObject *args)
{
	leveldb_writebatch_clear(self->_writebatch);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * WriteBatch_Release(WriteBatch *self, PyObject *args)
{
	leveldb_writebatch_destroy(self->_writebatch);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * LevelDB_Property(LevelDB *self, PyObject *args)
{

	const char *propname = NULL;
	char *propvalue;
	PyObject *result;

	if (!PyArg_ParseTuple(args, (char*)"s", &propname))
		return -1;
	propvalue = leveldb_property_value(self->_db, propname);
	if (propvalue != NULL) {
		result = Py_BuildValue("s#", propvalue, strlen(propvalue));
		return result;
	} else {
		fprintf(stderr, "No such property available.\n");
	}

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
		fprintf(stderr, "error occurs in repair db:\n\t%s\n", err);
	}

	self->_db = leveldb_open(self->_options, db_dir, &err);
	if (err != NULL) {
		_XDECREF(self->_db);
		_XDECREF(self->_options);
		_XDECREF(self->_cache);
		_XDECREF(self->_env);

		self->_db = NULL;
		self->_options = NULL;
		self->_cache = NULL;
		self->_env = NULL;
		fprintf(stderr, "error occurs in opening leveldb:\n\t%s\n", err);
	}

	Py_END_ALLOW_THREADS

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Snapshot_Set(Snapshot *self, PyObject *args)
{
	leveldb_readoptions_t *roptions = NULL;
	roptions = leveldb_readoptions_create();
	if (roptions == NULL) {
		fprintf(stderr, "Failed to create readoptions.\n");
	}

	if (self->_snapshot != NULL) {
		leveldb_readoptions_set_snapshot(roptions, self->_snapshot);
		printf("Set snapshot successfully.\n");
	} else {
		fprintf(stderr, "Unable to set snapshot.\n");
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Snapshot_Reset(Snapshot *self, PyObject *args)
{

	leveldb_readoptions_t *roptions = NULL;
	roptions = leveldb_readoptions_create();
	if (roptions == NULL) {
		fprintf(stderr, "Failed to create readoptions.\n");
	}

	leveldb_readoptions_set_snapshot(roptions, NULL);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Snapshot_Release(Snapshot *self, PyObject *args)
{
	LevelDB *leveldb = NULL;
	if (!PyArg_ParseTuple(args, (const char *)"O!", &LevelDBType, &leveldb)) 
		return -1;
	if (leveldb != NULL) {
		printf("Snapshot_Release executed.\n");
		leveldb_release_snapshot(leveldb->_db, self->_snapshot);
	}
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Iterator_Valid(Iterator *self, PyObject *args)
{
	PyObject *result;
	unsigned char valid;
	valid = leveldb_iter_valid(self->_iterator);
	result = Py_BuildValue("b", valid);
	return result;
}

static PyObject * Iterator_Seek_To_First(Iterator *self, PyObject *args)
{
	leveldb_iter_seek_to_first(self->_iterator);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Iterator_Seek_To_Last(Iterator *self, PyObject *args)
{
	leveldb_iter_seek_to_last(self->_iterator);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Iterator_Next(Iterator *self, PyObject *args)
{
	leveldb_iter_next(self->_iterator);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * Iterator_Prev(Iterator *self, PyObject *args)
{
	leveldb_iter_prev(self->_iterator);
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject * Iterator_Seek(Iterator *self, PyObject *args)
{

	LEVELDB_DEFINE_KVBUF(key);

	if (!PyArg_ParseTuple(args, (char*)"t#", &s_key, &i_key))
		return 0;

	leveldb_iter_seek(self->_iterator, s_key, i_key);
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject * Iterator_Get_Error(Iterator *self, PyObject *args)
{

	PyObject *result;
	char *err = NULL;

	leveldb_iter_get_error(self->_iterator, &err);
	result = Py_BuildValue("s#", err, strlen(err));
	_XDECREF(err);
	return result;
}



static PyObject * Iterator_Key(Iterator *self, PyObject *args)
{

	PyObject *result;

	const char *key;
	size_t len;
	key = leveldb_iter_key(self->_iterator, &len);
	result = Py_BuildValue("s#", key, len);
	return result;
}


static PyObject * Iterator_Value(Iterator *self, PyObject *args)
{

	PyObject *result;

	const char *value;
	size_t len;
	value = leveldb_iter_value(self->_iterator, &len);
	result = Py_BuildValue("s#", value, len);
	return result;
}


static PyObject * Iterator_Destroy(Iterator *self, PyObject *args)
{

	leveldb_iter_destroy(self->_iterator);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef LevelDB_methods[] = {
	{(char*)"Put",       (PyCFunction)LevelDB_Put,       METH_KEYWORDS, (char*)"add a key/value pair to database, with an optional synchronous disk write" },
	{(char*)"Get",       (PyCFunction)LevelDB_Get,       METH_KEYWORDS, (char*)"get a value from the database" },
	{(char*)"Delete",    (PyCFunction)LevelDB_Delete,    METH_KEYWORDS, (char*)"delete a value in the database" },
	{(char*)"Write",    (PyCFunction)LevelDB_Write,    METH_KEYWORDS, (char*)"apply a writebatch in database" },
	{(char*)"Property",    (PyCFunction)LevelDB_Property,    METH_KEYWORDS, (char*)"get a property value" },
	{(char*)"RepairDB",    (PyCFunction)LevelDB_RepairDB,    METH_KEYWORDS, (char*)"repair database" },
	{(char*)"Close",    (PyCFunction)LevelDB_Close,    METH_KEYWORDS, (char*)"close database" },
	{NULL}
};

static PyMethodDef WriteBatch_methods[] = {
	{(char*)"Put",    (PyCFunction)WriteBatch_Put,    METH_VARARGS, (char*)"add a put operation to batch" },
	{(char*)"Delete", (PyCFunction)WriteBatch_Delete, METH_VARARGS, (char*)"add a delete operation to batch" },
	{(char*)"Clear",    (PyCFunction)WriteBatch_Clear,    METH_VARARGS, (char*)"clear batch" },
	{(char*)"Release",    (PyCFunction)WriteBatch_Release,    METH_VARARGS, (char*)"release a batch" },
	{NULL}
};

static PyMethodDef Snapshot_methods[] = {
	{(char*)"Set",    (PyCFunction)Snapshot_Set,    METH_VARARGS, (char*)"set snapshot" },
	{(char*)"Reset", (PyCFunction)Snapshot_Reset, METH_VARARGS, (char*)"reset snapshot to the current state" },
	{(char*)"Release",    (PyCFunction)Snapshot_Release,    METH_VARARGS, (char*)"release snapshot" },
	{NULL}
};


static PyMethodDef Iterator_methods[] = {
	{(char*)"Validate",    (PyCFunction)Iterator_Valid,    METH_KEYWORDS, (char*)"validate iterator" },
	{(char*)"First",    (PyCFunction)Iterator_Seek_To_First,    METH_KEYWORDS, (char*)"seek to first" },
	{(char*)"Last",    (PyCFunction)Iterator_Seek_To_Last,    METH_KEYWORDS, (char*)"seek to last" },
	{(char*)"Seek",    (PyCFunction)Iterator_Seek,    METH_KEYWORDS, (char*)"iterator seek" },
	{(char*)"Next",    (PyCFunction)Iterator_Next,    METH_KEYWORDS, (char*)"iterator to next" },
	{(char*)"Prev",    (PyCFunction)Iterator_Prev,    METH_KEYWORDS, (char*)"iterator to prev" },
	{(char*)"Key",    (PyCFunction)Iterator_Key,    METH_KEYWORDS, (char*)"get key throuth current iterator" },
	{(char*)"Value",    (PyCFunction)Iterator_Value,    METH_KEYWORDS, (char*)"get value throuth current iterator" },
	{(char*)"GetError",    (PyCFunction)Iterator_Get_Error,    METH_KEYWORDS, (char*)"get iterator error" },
	{(char*)"Destroy",    (PyCFunction)Iterator_Destroy,    METH_KEYWORDS, (char*)"destroy iterator" },
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


PyTypeObject WriteBatchType = {
	PyObject_HEAD_INIT(NULL)
	0,                             /*ob_size*/
	(char*)"leveldb.WriteBatch",      /*tp_name*/
	sizeof(WriteBatch),             /*tp_basicsize*/
	0,                             /*tp_itemsize*/
	(destructor)WriteBatch_dealloc, /*tp_dealloc*/
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
	(char*)"leveldb WriteBatch",   /*tp_doc */
	0,                             /*tp_traverse */
	0,                             /*tp_clear */
	0,                             /*tp_richcompare */
	0,                             /*tp_weaklistoffset */
	0,                             /*tp_iter */
	0,                             /*tp_iternext */
	WriteBatch_methods,             /*tp_methods */
	0,                             /*tp_members */
	0,                             /*tp_getset */
	0,                             /*tp_base */
	0,                             /*tp_dict */
	0,                             /*tp_descr_get */
	0,                             /*tp_descr_set */
	0,                             /*tp_dictoffset */
	(initproc)WriteBatch_init,      /*tp_init */
	0,                             /*tp_alloc */
	WriteBatch_new,                 /*tp_new */
};

PyTypeObject SnapshotType = {
	PyObject_HEAD_INIT(NULL)
	0,                             /*ob_size*/
	(char*)"leveldb.Snapshot",      /*tp_name*/
	sizeof(Snapshot),             /*tp_basicsize*/
	0,                             /*tp_itemsize*/
	(destructor)Snapshot_dealloc, /*tp_dealloc*/
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
	(char*)"leveldb Snapshot",   /*tp_doc */
	0,                             /*tp_traverse */
	0,                             /*tp_clear */
	0,                             /*tp_richcompare */
	0,                             /*tp_weaklistoffset */
	0,                             /*tp_iter */
	0,                             /*tp_iternext */
	Snapshot_methods,             /*tp_methods */
	0,                             /*tp_members */
	0,                             /*tp_getset */
	0,                             /*tp_base */
	0,                             /*tp_dict */
	0,                             /*tp_descr_get */
	0,                             /*tp_descr_set */
	0,                             /*tp_dictoffset */
	(initproc)Snapshot_init,      /*tp_init */
	0,                             /*tp_alloc */
	Snapshot_new,                 /*tp_new */
};

PyTypeObject IteratorType = {
	PyObject_HEAD_INIT(NULL)
	0,                             /*ob_size*/
	(char*)"leveldb.Iterator",      /*tp_name*/
	sizeof(Iterator),             /*tp_basicsize*/
	0,                             /*tp_itemsize*/
	(destructor)Iterator_dealloc, /*tp_dealloc*/
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
	(char*)"leveldb Iterator",   /*tp_doc */
	0,                             /*tp_traverse */
	0,                             /*tp_clear */
	0,                             /*tp_richcompare */
	0,                             /*tp_weaklistoffset */
	0,                             /*tp_iter */
	0,                             /*tp_iternext */
	Iterator_methods,             /*tp_methods */
	0,                             /*tp_members */
	0,                             /*tp_getset */
	0,                             /*tp_base */
	0,                             /*tp_dict */
	0,                             /*tp_descr_get */
	0,                             /*tp_descr_set */
	0,                             /*tp_dictoffset */
	(initproc)Iterator_init,      /*tp_init */
	0,                             /*tp_alloc */
	Iterator_new,                 /*tp_new */
};
#define LevelDB_Check(op) PyObject_TypeCheck(op, &LevelDBType)
#define WriteBatch_Check(op) PyObject_TypeCheck(op, &WriteBatchType)
#define Snapshot_Check(op) PyObject_TypeCheck(op, &SnapshotType)
#define Iterator_Check(op) PyObject_TypeCheck(op, &IteratorType)
PyMODINIT_FUNC
initleveldb(void)
{
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
}
