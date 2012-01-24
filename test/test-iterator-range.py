
import datetime
import uuid
import leveldb

def main():
    n=100
    print("%s: write %d items to db.."%(datetime.datetime.now(),n))
    
    db = leveldb.LevelDB("./leveldb.db")
    for i in range(n):
        db.Put(str(i), "this is item %d"%(i))
        
    print("%s: lookup items from map between range key_from to key_to..."%(datetime.datetime.now()))
    print list(db.RangeIter(key_from = '80', key_to = '90'))
    print("%s: ok"%(datetime.datetime.now()))
    return 0

if __name__ == '__main__':
    main()

