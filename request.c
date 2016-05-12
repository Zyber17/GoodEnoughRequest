#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "request.h"

Request getPost() { // Wrapepr for handling a POST request
	char *conLen = getenv("CONTENT_LENGTH"); // Store CONTENT_LENGTH
	Request req; // Make a new request
	if (conLen != NULL) { // If CONTENT_LENGTH exists, then there was a post request made
		long int len = (long int)strtol(conLen, NULL, 10); // Convert CONTENT_LENGTH to a long int
		char *data = malloc(len + 1); // Make a storage space for the post reuqst
		fgets(data, len + 1, stdin); // Read in the data
		data[len] = '\0'; // Make sure the last character is a null character
		req = tokenize(data, len); // Tokenize it
	} else { // If CONTENT_LENGTH was null, there wasn't a post request
		req.itemCount = 0; // So say there were no items in the request
	}
	return req; // Return the request
}

Request getGet() { // Wrapper for handling a GET request
	char *data = getenv("QUERY_STRING"); // Store QUERY_STRING
	Request req; // Make a new request
	if (data != NULL) {
		long int len = (long int)strlen(data); // Get the length of the data
		req = tokenize(data, len); // Tokenize it
	} else { // If QUERY_STRING was null, there wasn't a post request
		req.itemCount = 0; // So say there were no items in the request
	}
	return req; // Return the request
}

Request tokenize(char *data, long int count) { // The function that actually does most of the work tokenizing the data
	// Get number of arguments

	short int seenTag = 0;
	int tags = 0;
	int vals = 0;
	for (int i = 0; i < count; i++) {
		if (data[i] == '=' && !seenTag) { // format is tag=val&tag=val, so looking for strings before = gives us the tags
			seenTag = 1;
			tags++;
		}
		else if (data[i] == '&' && seenTag) { // looking for strings before & and after = gives us the vals
			seenTag = 0;
			vals++;
		} else {
			// error
		}
	}
	vals++; // Last get val doesn't trigger, so we do it manually
	// if (tags != vals) { // Something bad happened, the number of tags should be the same as the number of vals
	// 	// error
	// }

	// End: Get number of arguments
	// Get length of each argument

	int len = 0;
	seenTag = 0;
	// Crate arrays to store the length of teach tag/val in order
	int *tagLen = calloc(tags, sizeof(int));
	int *valLen = calloc(vals, sizeof(int));
	int currentTag = 0;
	int currentVal = 0;
	for (int i = 0; i < count; i++) {
		if (data[i] != '=' && data[i] != '&') { // i.e. if the current character is a character in the current tag/val
			if (data[i] == '%') {
			 	len--; // If we encounter a %, it's the trigger for an ASCII encoding, we know of the %AB string, it's only going to be one character in the actual string when we store the strings, so reducing len by one here will increase by two for AB, making %AB count overall as 1 character (1 = -1 + 2)
			} else {
				len++;
			}
		// Handle the cases when we reach the end of a tag/val
		} else if (data[i] == '=' && !seenTag) {
			seenTag = 1;
			tagLen[currentTag] = len + 1;
			currentTag++;
			len = 0;
		} else if (data[i] == '&' && seenTag) {
			seenTag = 0;
			valLen[currentVal] = len + 1;
			currentVal++;
			len = 0;
		}
		// else { // Probably caused by the same thing that caused tags != vals
		// 	// error
		// }
	}
	valLen[currentVal] = len + 1; // Last doesn't trigger, so we do it manually again

	// End: Get length of each argument
	// Construct Request return

	Request req;
	req.itemCount = (tags + vals + 1)/2; // In theiry, tags == vals, always, but averaging the two feels safer.
	req.items = (Item*)calloc(req.itemCount, sizeof(Item));	// Make an array of items the size of the number of items we have
	int k = 0; // k is the offset from beginning of data to the end of most recent string processed below
	for (int i = 0; i < req.itemCount; i++) {
		Item consItem;
		consItem.tagLen = tagLen[i];
		consItem.valLen = valLen[i];

		consItem.tag = (char*)malloc(consItem.tagLen); // Using the lengths calculated earlier, malloc the right size for the tag string
		for (int j = 0; j < consItem.tagLen - 1; j++) { // Store the string in the item
			if (data[j + k] == '+') { // Handle Space encoding
				consItem.tag[j] = ' ';
			} else if (data[j + k] == '%') { // Handle ASCII encoding
				char tmp[3];
				tmp[0] = data[j + k + 1];
				tmp[1] = data[j + k + 2];
				tmp[2] = '\0';
				short int ascii = strtol(tmp, NULL, 16);
				consItem.tag[j] = (char)ascii;
				k += 2;
			} else {
				consItem.tag[j] = data[j + k];
			}
		}
		consItem.tag[consItem.tagLen - 1] = '\0'; // Make sure the last character of the string is the null character
		k += consItem.tagLen; // Add the length of the last string to the offset


		consItem.val = (char*)malloc(consItem.valLen); // Using the lengths calculated earlier, malloc the right size for the cal string
		for (int j = 0; j < consItem.valLen - 1; j++) { // Store the string in the item
			if (data[j + k] == '+') { // Handle Space encoding
				consItem.val[j] = ' ';
			} else if (data[j + k] == '%') {
				char tmp[3];
				tmp[0] = data[j + k + 1];
				tmp[1] = data[j + k + 2];
				tmp[2] = '\0';
				short int ascii = strtol(tmp, NULL, 16);
				consItem.val[j] = (char)ascii;
				k += 2;
			} else { // Handle ASCII encoding
				consItem.val[j] = data[j + k];
			}
		}

		consItem.val[consItem.valLen - 1] = '\0'; // Make sure the last character of the string is the null character
		k += consItem.valLen; // Add the length of the last string to the offset

		// Don't count the trailing null charatcer in the string length actually returned to the user
		consItem.tagLen--;
		consItem.valLen--;

		req.items[i] = consItem;

	}

	// End: Construct 'return' array

	// Free all the things
	free(tagLen);
	free(valLen);

	return req; // Return the request
}
