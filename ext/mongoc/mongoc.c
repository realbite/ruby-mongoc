#include <ruby.h>
#include <ruby/version.h>
#include <mongo.h>

VALUE mMongoc; /* Top level module */
VALUE cClient;
VALUE cCursor;
VALUE eMongoError;

// free function for Client
static void client_free(void *p) {
	mongo_destroy(p);
}

// allocate  function for Client
VALUE client_alloc(VALUE klass) {
	mongo *conn;
	return Data_Make_Struct(klass, mongo, 0, client_free, conn);
}

// free function for Cursor
static void cursor_free(void *p) {
	mongo_cursor_destroy(p);
}

static VALUE client_initialize(VALUE self, VALUE host, VALUE port,
		VALUE options) {
	mongo *conn;

	Data_Get_Struct(self, mongo, conn);
	int status = mongo_client(conn, RSTRING_PTR(host), NUM2INT(port));

	if (status != MONGO_OK) {
		switch (conn->err) {
		case MONGO_CONN_NO_SOCKET:
			rb_raise(eMongoError, "no socket");
		case MONGO_CONN_FAIL:
			rb_raise(eMongoError, "connection failed");
		case MONGO_CONN_NOT_MASTER:
			rb_raise(eMongoError, "not master");
		}
	}
	return self;
}

/*
 static VALUE client_all_json_docs(VALUE self, VALUE table) {
 mongo *conn;
 VALUE list;
 list = rb_ary_new();

 Data_Get_Struct(self, mongo, conn);
 mongo_cursor cursor[1];

 mongo_cursor_init(cursor, conn, StringValuePtr(table));

 while (rb_thread_blocking_region(mongo_cursor_next, cursor, RUBY_UBF_IO,0) == MONGO_OK) {

 char * str;

 bson *current;
 current = &cursor->current;

 str = bson_as_json(current, NULL);

 VALUE bson_data = rb_str_new2(str);
 bson_free(str);
 rb_ary_push(list, bson_data);
 }

 mongo_cursor_destroy(cursor);

 return list;
 }
 */

static VALUE get_cursor_next(void *cursor) {
	return INT2NUM(mongo_cursor_next((mongo_cursor *) cursor));
}

/*
static VALUE xxx_count(VALUE table){
	mongo *conn;
	Data_Get_Struct(self, mongo, conn);
	double count;
 count =  mongo_count( conn, const char *db, const char *coll,
                                 const bson *query );
}
*/

/*
*  void * rb_thread_call_without_gvl (void *(*func)(void *)       , void *data1, rb_unblock_function_t *ubf, void *data2)
*  rb_thread_blocking_region         (rb_blocking_function_t *func, void *data1, rb_unblock_function_t *ubf, void *data2));
*/

#if (defined RUBY_API_VERSION_CODE) && (RUBY_API_VERSION_CODE>= 20100 )

static VALUE client_all_bson_docs(VALUE self, VALUE table) {
	mongo *conn;
	VALUE list;
	list = rb_ary_new();

	Data_Get_Struct(self, mongo, conn);
	mongo_cursor cursor[1];

	mongo_cursor_init(cursor, conn, StringValuePtr(table));

	while (rb_thread_call_without_gvl(get_cursor_next, cursor, RUBY_UBF_IO, 0)
			== INT2NUM(MONGO_OK)) {
		char * buffer;

		bson *current;
		current = &cursor->current;

		VALUE bson_data = rb_str_new(current->data, current->dataSize);

		rb_ary_push(list, bson_data);
	}

	mongo_cursor_destroy(cursor);

	return list;
}


#else
static VALUE client_all_bson_docs(VALUE self, VALUE table) {
	mongo *conn;
	VALUE list;
	list = rb_ary_new();

	Data_Get_Struct(self, mongo, conn);
	mongo_cursor cursor[1];

	mongo_cursor_init(cursor, conn, StringValuePtr(table));
	while (rb_thread_blocking_region(get_cursor_next, cursor, RUBY_UBF_IO, 0)
			== INT2NUM(MONGO_OK)) {
		char * buffer;

		bson *current;
		current = &cursor->current;

		VALUE bson_data = rb_str_new(current->data, current->dataSize);

		rb_ary_push(list, bson_data);
	}

	mongo_cursor_destroy(cursor);

	return list;
}
#endif

static void string_to_bson(VALUE str, bson* query) {
	int status;

	status = bson_init_finished_data_with_copy(query, StringValuePtr(str));
	if (status == BSON_ERROR) {
		rb_raise(eMongoError, "error creating BSON");
	}
}

static VALUE client_insert(VALUE self, VALUE table, VALUE doc) {
	mongo *conn;
	Data_Get_Struct(self, mongo, conn);

	bson query[1];
	string_to_bson(doc, query);
	int status;
	status = mongo_insert(conn, StringValuePtr(table), query, 0);
	bson_destroy(query);
	if (status != MONGO_OK) {
			rb_raise(eMongoError, "error inserting document");
	}
	return Qtrue;
}

static VALUE drop_collection(VALUE self, VALUE table, VALUE collection) {
	mongo *conn;
	Data_Get_Struct(self, mongo, conn);
	int status;
	bson query[1];
	status = mongo_cmd_drop_collection(conn, StringValuePtr(table),
			StringValuePtr(collection), query);

	if (status != MONGO_OK) {
		rb_raise(eMongoError, "error dropping table");
	}
	VALUE result = rb_str_new(query->data, query->dataSize);
	bson_destroy(query);
	return result;
}

static VALUE drop_database(VALUE self, VALUE collection) {
	    mongo *conn;
		Data_Get_Struct(self, mongo, conn);
		int status;
		status = mongo_cmd_drop_db( conn,  StringValuePtr(collection) );
		if (status != MONGO_OK) {
			rb_raise(eMongoError, "error dropping database");
		}
		return Qtrue;
}

#if (defined RUBY_API_VERSION_CODE) && (RUBY_API_VERSION_CODE>= 20100 )

static VALUE client_find(VALUE self, VALUE table, VALUE str) {
	mongo *conn;
	VALUE list;
	list = rb_ary_new();

	bson query[1];
	string_to_bson(str, query);

	Data_Get_Struct(self, mongo, conn);
	mongo_cursor cursor[1];

	mongo_cursor_init(cursor, conn, StringValuePtr(table));
	mongo_cursor_set_query(cursor, query);
	while (rb_thread_call_without_gvl(get_cursor_next, cursor, RUBY_UBF_IO, 0)
			== INT2NUM(MONGO_OK)) {
		char * buffer;

		bson *current;
		current = &cursor->current;

		VALUE bson_data = rb_str_new(current->data, current->dataSize);

		rb_ary_push(list, bson_data);
	}

	bson_destroy(query);
	mongo_cursor_destroy(cursor);

	return list;
}
#else
static VALUE client_find(VALUE self, VALUE table, VALUE str) {
	mongo *conn;
	VALUE list;
	list = rb_ary_new();

	bson query[1];
	string_to_bson(str, query);

	Data_Get_Struct(self, mongo, conn);
	mongo_cursor cursor[1];

	mongo_cursor_init(cursor, conn, StringValuePtr(table));
	mongo_cursor_set_query(cursor, query);
	while (rb_thread_blocking_region(get_cursor_next, cursor, RUBY_UBF_IO, 0)
			== INT2NUM(MONGO_OK)) {
		char * buffer;

		bson *current;
		current = &cursor->current;

		VALUE bson_data = rb_str_new(current->data, current->dataSize);

		rb_ary_push(list, bson_data);
	}

	bson_destroy(query);
	mongo_cursor_destroy(cursor);

	return list;
}
#endif
static VALUE client_update(VALUE self, VALUE table, VALUE cond, VALUE op) {
	mongo *conn;
	Data_Get_Struct(self, mongo, conn);

	bson query[1], b_op[1];
	string_to_bson(cond, query);
	string_to_bson(op, b_op);
	mongo_write_concern *custom_write_concern = 0;
	int flags = 0;
	int status;
	status = mongo_update(conn, StringValuePtr(table), query, b_op, flags,
			custom_write_concern);
	if (status != MONGO_OK) {
		rb_raise(eMongoError, "error updating document");
	}
	bson_destroy(query);
	bson_destroy(b_op);
	return Qnil;
}

static VALUE client_remove(VALUE self, VALUE table, VALUE cond) {
	mongo *conn;
	Data_Get_Struct(self, mongo, conn);
	bson query[1];
	string_to_bson(cond, query);
	mongo_write_concern *custom_write_concern = 0;
	int status;
	status = mongo_remove(conn, StringValuePtr(table), query,
			custom_write_concern);
	if (status != MONGO_OK) {
		rb_raise(eMongoError, "error removing document");
	}
	bson_destroy(query);
	return Qnil;
}

static VALUE compound_index(VALUE self, VALUE table, VALUE key, VALUE name, VALUE options) {
       mongo *conn;
       Data_Get_Struct(self, mongo, conn);

       bson b_key[1];
       bson b_out[1];
       string_to_bson(key, b_key);
       int status;
       int ttl = 0;
       status = mongo_create_index( conn, StringValuePtr(table), b_key,NIL_P(name) ? NULL : StringValuePtr(name), FIX2INT(options), ttl, b_out );
       if (status != MONGO_OK) {
    	   //rb_raise(eMongoError, "error creating index");
    	      	   return Qfalse;
       	}
       return Qtrue;
}

static VALUE simple_index(VALUE self, VALUE table, VALUE field, VALUE options) {
       mongo *conn;
       Data_Get_Struct(self, mongo, conn);

       bson b_out[1];

       bson_bool_t status;
       status = mongo_create_simple_index( conn, StringValuePtr(table), StringValuePtr(field), FIX2INT(options), b_out );
       if (!status) {
            //rb_raise(eMongoError, "error creating index");
    	   return Qfalse;
       }
       return Qtrue;
}

void Init_mongoc() {

	mMongoc = rb_define_module("Mongoc");

	cClient = rb_define_class_under(mMongoc, "Client", rb_cObject);
	cCursor = rb_define_class_under(mMongoc, "Cursor", rb_cObject);

	eMongoError = rb_define_class_under(mMongoc, "MongoError",
			rb_eStandardError);

	rb_define_const(mMongoc, "INDEX_UNIQUE", INT2NUM(MONGO_INDEX_UNIQUE));
	rb_define_const(mMongoc, "INDEX_DROP_DUPS", INT2NUM(MONGO_INDEX_DROP_DUPS));

	rb_define_alloc_func(cClient, client_alloc);
	rb_define_method(cClient, "initialize", client_initialize, 3);
	rb_define_method(cClient, "all_bson_docs", client_all_bson_docs, 1);
	rb_define_method(cClient, "insert_bson", client_insert, 2);
	rb_define_method(cClient, "find_bson", client_find, 2);
	rb_define_method(cClient, "drop_collection_bson", drop_collection, 2);
	rb_define_method(cClient, "update_bson", client_update, 3);
	rb_define_method(cClient, "remove_bson", client_remove, 2);
	rb_define_method(cClient, "drop_database", drop_database, 1);
	rb_define_method(cClient, "compound_index", compound_index, 4);
	rb_define_method(cClient, "simple_index", simple_index, 3);

}
