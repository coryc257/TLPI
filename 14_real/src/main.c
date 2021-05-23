#include "../lib/list_functions.h"
#include "../lib/random.h"
#include "../lib/io.h"
#include "tlpi_hdr.h"


static AUTO_SORTED_LIST *list;

int
main(int argc, char *argv[])
{
	char *directory;
	if(argc != 2)
		usageErr("%s <directory>",argv[0]);

	directory = argv[1];

	for (int i = 1; i <= 10000; i++){
		ADD_LIST_ITEM(list,
				SET_ITEM(NEW_LIST_ITEM(), GENERATE_RANDOM(), directory));
	}

	PRINT_ASCENDING(list);
	LIST_GENERATE_FILES_RANDOM(list->link);
	LIST_DELETE_FILES_ASCENDING(list);
}
