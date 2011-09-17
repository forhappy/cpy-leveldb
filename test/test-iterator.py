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


print "Creating iterator"

iter = leveldb.Iterator(db);
print "Seek to First."
iter.First()
print "\n"

print "Print iterator's key"
print iter.Key()
print "\n"

print "Print iterator's value"
print iter.Value()
print "\n"

print "Seek to next"
iter.Next()
print "\n"

print "Print iterator's key again"
print iter.Key()
print "\n"

print "Print iterator's value again"
print iter.Value()
print "\n"

print "Seek to Last"
iter.Last()
print "\n"

print "Print iterator's key again"
print iter.Key()
print "\n"

print "Print iterator's value again"
print iter.Value()
print "\n"

