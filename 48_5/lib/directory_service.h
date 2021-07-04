/*
 * directory_service.h
 *
 *  Created on: Jul 4, 2021
 *      Author: cory
 */

#ifndef LIB_DIRECTORY_SERVICE_H_
#define LIB_DIRECTORY_SERVICE_H_

#include <sys/types.h>
#define A_BOOL
#include "tlpi_hdr.h"


typedef struct directory_listing {
	/*
	 * 0 means slot open
	 * 1 means slot in use
	 * -1 means storage_location refers to another directory listing file with an addition 500 keys
	 */
	int in_use;
	// NOT NULL TERMINATED
	char key[500];
	char value[1500];
	char overflow[44]; // FUTURE USE

	// IDEALLY:
	/*
	// Shared Memory Key of key value
	key_t storage_location;
	// Offset of 1024 bytes to find value entry
	int storage_offset;
	*/
} DIRECTORY_LISTING;

typedef struct value_entry {
	char value[1024];
} VALUE_ENTRY;

typedef struct base_info {
	key_t file_key;
	key_t sync_key;
	key_t list_key;

	int file_id;
	int sync_id;
	int list_id;
} BASE_INFO;

void init_directory_server(const char *server_root);
int add_entry(const char *server_root, const char *key, const char *value);

#endif /* LIB_DIRECTORY_SERVICE_H_ */
