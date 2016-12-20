#include <stdio.h>
#include <mongo.h>

int main() {
	//mongo conn[1];
	mongo *conn;
	mongo_init( conn );

	int status = mongo_client(conn, "127.0.0.1", 27017);

	if (status != MONGO_OK) {
		switch (conn->err) {
		case MONGO_CONN_NO_SOCKET:
			printf("no socket\n");
			return 1;
		case MONGO_CONN_FAIL:
			printf("connection failed\n");
			return 1;
		case MONGO_CONN_NOT_MASTER:
			printf("not master\n");
			return 1;
		}
	}
	printf("connected to server\n");

	mongo_cursor cursor[1];
	//mongo_cursor *cursor;
	mongo_cursor_init(cursor, conn, "speed_test.test");

	printf("here\n");

	while (mongo_cursor_next(cursor) == MONGO_OK)
		bson_print(&cursor->current);

	mongo_cursor_destroy(cursor);

	mongo_destroy(conn);

	return 0;
}
