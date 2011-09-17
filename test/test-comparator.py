import leveldb
def destructor():
	pass

def compare(a, b):
	if (a > b):
		return -1
	if (a < b):
		return 1
	if (a == b):
		return 0;

def name():
	return "hello, python"


cmp = leveldb.Comparator(destructor, compare, name)
db = leveldb.LevelDB(filename="/tmp/leveldb-dummy", comparator=cmp)
db.Put("hello", "world")
db.Put("1", "1111")
db.Put("2", "2222")
print db.Get("2")
print db.Get("hello")
