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
