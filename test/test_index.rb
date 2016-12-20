$: << 'ext/mongoc'
$: << './lib' << './ext'

require 'bson'
require 'mongoc'

client = Mongoc::Client.new("127.0.0.1",27017,{})


ITABLE = "speed_test.itest"
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

# create a compound index 

client.ensureIndex(ITABLE,{"name"=>1},{:name=>'xxxx_1', :unique=>true})

#query = {"name"=>1}
#options = Mongoc::INDEX_UNIQUE | Mongoc::INDEX_DROP_DUPS
#ret = client.compound_index(ITABLE,BSON.serialize(query).to_s,"iname",options)
#puts "RRRRRRRRRRR #{ret}"
#ret = client.compound_index(ITABLE,BSON.serialize(query).to_s,"iname",options)
#puts "RRRRRRRRRRR #{ret}"
#
client.insert(ITABLE,{:name=>"bob", :profession=>"builder"})
client.insert(ITABLE,{:name=>"bob", :profession=>"carpenter"}) rescue puts "XXXXXXX"
puts "-----------------------"
puts client.find(ITABLE,{}).inspect

