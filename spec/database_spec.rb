require 'spec_helper'


module Mongoc::Resource 
  describe Database do
    
    it "should insert items into a table" do
      
      expect(Database.all_docs("foo").length).to eql(0)
      
      1000.times do |i|
         id = BSON::ObjectId.new
         doc = {"_id"=>id, "name"=>"item_#{i}", "count"=>i}
         Database.insert("foo",doc)
      end
      
      expect(Database.all_docs("foo").length).to eql(1000)
      
      puts Database.all_docs("foo")[-1].inspect
      
    end
  end

end
