import leveldb
db = leveldb.LevelDB("/tmp/test-dead/")
batch = leveldb.WriteBatch()

for i in xrange(100000):
	batch.Put(str(i), "hello,hello,hello string %i" % i)

db.Write(batch)
'''
db.Put("1", "111")
db.Put("2", "222")
db.Put("3", "333")
db.Put("4", "444")
db.Put("a", "aaa")
db.Put("b", "bbb")
db.Put("c", "ccc")
db.Put("d", "ddd")
'''
print db.Get("88888")

print db.GetApproximateSizes(num_ranges=2, range_start_key=("1","50001"),
range_limit_key=("80000","99999"))


