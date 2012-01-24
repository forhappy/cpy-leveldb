import leveldb
db = leveldb.LevelDB("/tmp/test-leveldb/")

db.Put("hello", "world")
db.Put("1", "111111")
db.Put("2", "222222")
db.Put("3", "333333")

print "Creating snapshot."
snap = leveldb.Snapshot(db)
print "\n"

print "Deleting key 1."
db.Delete("1")
print "\n"

print "Getting key 1."
print db.Get("1")
print "\n"

print "Applying snapshot."
snap.Set()
print "\n"

print "Getting key 1 again."
print db.Get("1")
print "\n"


print "Resetting snapshot."
snap.Reset()
print "\n"


print "Getting key 1 again."
print db.Get("1")
print "\n"
# Release Snapshot object.
snap.Release()
print "O.K."
