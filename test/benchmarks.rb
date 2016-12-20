$:.unshift 'lib'
$:.unshift 'ext'

require 'mongoc/resource'
require 'mongo'



Mongoc::Resource::Database.configure(:host=>'localhost', :database=>'mongoc_test')
Mongoc::Resource::Database.drop_test_database!

count = (ARGV[0] && ARGV[0].to_i) || 100
puts "count = #{count}"
table = "foo"

t = Time.now
count.times do |i|
         id = BSON::ObjectId.new
         doc = {"_id"=>id, "name"=>"item_#{i}", "count"=>i}
         Mongoc::Resource::Database.insert(table,doc)
end
len = -1
Mongoc::Resource::Database.all_docs(table)
diff = Time.now - t
puts "mongoc length=#{len}, time=#{diff}"

Mongoc::Resource::Database.drop_test_database!
#---------------------------------------------------------
Mongo::Logger.logger.level = ::Logger::FATAL
client = Mongo::Client.new([ '127.0.0.1:27017' ], :database => 'mongoc_test', :connect => :direct)
collection = client[table]

t = Time.now
count.times do |i|
         id = BSON::ObjectId.new
         doc = {"_id"=>id, "name"=>"item_#{i}", "count"=>i}
         collection.insert_one(doc)
end
len = -1
collection.find #.to_a.length
diff = Time.now - t
puts "mongo  length=#{len}, time=#{diff}"


#--------------------------------------------------------

# find a record
t = Time.now
count.times do |i|
         doc = Mongoc::Resource::Database.find(table,{"count"=>i}).first
         raise "error" unless doc && doc["count"] == i
end
diff = Time.now - t
puts "mongoc find records, time=#{diff}"

#--------------------------------------------------------

# find a record
t = Time.now
count.times do |i|
         doc = collection.find({"count"=>i}).first
         raise "error" unless doc && doc["count"] == i
end
diff = Time.now - t
puts "mongo  find records, time=#{diff}"

