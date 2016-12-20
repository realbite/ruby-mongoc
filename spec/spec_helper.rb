$:.unshift 'lib'
$:.unshift 'ext'

# require gem

require 'mongoc/resource'

# configure rspec

RSpec.configure do  |c|
 
  c.before(:all) do
    Mongoc::Resource::Database.configure(:host=>'localhost', :database=>'mongoc_test')
  end
  
  c.before(:each) do
    Mongoc::Resource::Database.drop_test_database!
  end
  
end

