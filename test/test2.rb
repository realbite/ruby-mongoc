$: << 'ext/mongoc'

require 'mongoc'

client = Mongoc::Client.new("127.0.0.1",27017,{})

puts "client opened :#{client.inspect}"

now = Time.now
list = client.all_json_docs("speed_test.test")




puts "list length = #{list.length} #{(Time.now-now)*1000}"

puts list.first

#puts "item=#{item.inspect} // bson length=#{list.first.length}"