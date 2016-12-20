$: << 'ext/mongoc'
$: << './lib' << './ext'

require 'bson'
require 'mongoc'

client = Mongoc::Client.new("127.0.0.1",27017,{})

puts "client opened :#{client.inspect}"

now = Time.now
list = client.all_bson_docs("speed_test.test")
list.map!{|i| BSON.deserialize i}



puts "list length = #{list.length} #{(Time.now-now)*1000}"

puts list.first

ITABLE = "speed_test.itest"

#puts "item=#{item.inspect} // bson length=#{list.first.length}"

client.drop_collection("speed_test","itest") rescue nil

client.insert(ITABLE,{:name=>"bob", :profession=>"builder"})

doc = client.find(ITABLE,{:name=>"bob"}).first

doc["name"] = "kate"
id = doc["_id"]
client.update(ITABLE,{:_id=>id},doc)
doc = client.find(ITABLE,{})

puts "DOC=#{doc.inspect}"

client.remove(ITABLE,{:_id=>id})

list = client.find(ITABLE,{})

puts "AFTER REMOVE LENGTH = #{list.length}"