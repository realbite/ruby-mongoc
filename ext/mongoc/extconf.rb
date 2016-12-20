#!/usr/bin/env ruby
require 'mkmf'
$CFLAGS << ' --std=c99'

mongoc_dir  = "../mongo-c-driver-0.8.1"
sources_dir = mongoc_dir + "/src"

find_header("mongo.h",sources_dir)
find_header("bson.h",sources_dir)

# compile the driver here
Dir.chdir(mongoc_dir) do
    print "compiling mongo driver..."
    `make`
    puts "yes"
end

# check that our mongoc driver libraries exist..
find_library("mongoc","mongo_init",mongoc_dir)
find_library("bson","bson_init",mongoc_dir)

$LOCAL_LIBS << " #{mongoc_dir}/libbson.a"
$LOCAL_LIBS << " #{mongoc_dir}/libmongoc.a"

create_makefile('mongoc/mongoc')
