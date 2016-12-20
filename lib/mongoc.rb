require 'mongoc/mongoc'
require 'bson'

unless  defined? BSON.serialize
  def BSON.serialize(val)
    val.to_bson
  end
end

unless  defined? BSON.deserialize
  def BSON.deserialize(val)
    BSON::Document.from_bson(BSON::ByteBuffer.new(val))
  end
end

class Mongoc::Client
  
  def all_docs(table)
    all_bson_docs(table).map{|i| BSON.deserialize i}
  end
  
  def find(table,query)
    find_bson(table,BSON.serialize(query).to_s).map{|i| BSON.deserialize i}
  end
  
  def insert(table,doc)
    insert_bson(table,BSON.serialize(doc).to_s)
    true
  end
  
  def drop_collection(db,collection)
    BSON.deserialize drop_collection_bson(db,collection)
  end
  
  def remove(table,cond)
    remove_bson(table,BSON.serialize(cond).to_s)
  end
  
  def update(table,cond,op)
    update_bson(table,BSON.serialize(cond).to_s,BSON.serialize(op).to_s)
  end
  
  def ensureIndex(table,keys,options={})
    name   = options[:name] || options["name"]
    params = 0
    params |= Mongoc::INDEX_UNIQUE if options[:unique] || options["unique"]
    params |= Mongoc::INDEX_DROP_DUPS    if options[:dropDups] || options["dropDups"]
    
    compound_index(table,BSON.serialize(keys).to_s,name,params)
  end
end
