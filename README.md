# Good Enough Request

A C 'library' for handling GET and POST requests. It's not amazing, but it's good enough.


## Usage

Call `Request req = getGet();` or `getPost()` depending on the type of request you're expecting. The function will return a request struct; the struct has layout as follows

* `Request`
	* `int itemCount`
	* `Item *items`
		* `Item`
			* `char *tag`
			* `int tagLen`
			* `char *val`
			* `int valLen`


Where there are as many items as needed. As a note: If a request was not made, the functions will return a `Request` with `itemCount` of 0 and a null pointer for `items`. The library also handles converting from their ascii value to the actual character.

### Example

If the GET (or POST) request string was `q=example+get+requests+with+a+%25+sign!&sourceid=chrome&es_sm=91&ie=UTF-8`, the returned `Request` struct would look something like

* `Request`
	* `int itemCount = 4`
	* `Item *items`
		* `Item`
			* `char *tag = "q\0"`
			* `int tagLen = 2`
			* `char *val = "example get requests with a % sign!\0"`
			* `int valLen = 36`
		* `Item`
            * `char *tag = "sourceid\0"`
            * `int tagLen = 9`
            * `char *val = "chrome\0"`
            * `int valLen = 6`
		* `Item`
            * `char *tag = "es_sm\0"`
            * `int tagLen = 6`
            * `char *val = "91\0"`
            * `int valLen = 3`
		* `Item`
            * `char *tag = "ie\0"`
            * `int tagLen = 3`
            * `char *val = "UTF-8\0"`
            * `int valLen = 6`
