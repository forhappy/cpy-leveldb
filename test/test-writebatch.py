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
