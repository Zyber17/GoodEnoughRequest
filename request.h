typedef struct Item {
	char *tag;
	int	tagLen;
	char *val;
	int valLen;
} Item;

typedef struct Request {
	int itemCount;
	Item *items;
} Request;

Request getPost();
Request getGet();
