# -*- encoding: utf-8 -*-

require 'rake'

Gem::Specification.new do |s|
  s.name = 'mongoc'
  s.version = '0.0.5'
  s.platform = Gem::Platform::RUBY
  s.authors = ['Clive Andrews']
  s.email = ['gems@realitybites.eu']
  s.homepage = 'http://demichef.nl/'
  s.summary = 'Basic Ruby Bindings for MongoDB C Driver'
  s.description = 'Ruby Bindings to the Mongo C Driver and a very simple Document mappings offering a spped advantage over the ruby mongo driver.'
  s.licenses = ["Apache-2.0"]
  s.required_ruby_version = '>=1.9.2'
  
  s.add_dependency 'bson', '~>1.12'
  s.add_dependency 'bson_ext', '~>1.12'
  
  s.extensions = ["ext/mongoc/extconf.rb"]
  s.files = FileList['lib/**/*.rb'].to_a
  s.files << "ext/mongoc/mongoc.c" 
  s.files += FileList['ext/mongo-c-driver-0.8.1/src/*.[h|c]'].to_a
  s.files << "ext/mongo-c-driver-0.8.1/Makefile" 
  s.extra_rdoc_files = ['README']
  s.require_paths = ['lib','ext']
end
