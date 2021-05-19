/*
 * 4_10_2_cp.h
 *
 *  Created on: May 9, 2021
 *      Author: cory
 */

#ifndef HDR_4_10_2_CP
#define HDR_4_10_2_CP 1

typedef struct {
	char *input_file;
	char *output_file;

	int fd_input_file;
	int fd_output_file;
} FILE_OPTIONS;

typedef enum {
	CPFT_input,
	CPFT_output
} CP_FILE_TYPE;

// This parses input and opens the files
void parse_input(int argc, char *argv[]);
void check_for_file(const char *file_type);
void show_options(void);
void open_file(int *fd, int flags, int mode,  const char* file, const char* type);
void load_file(CP_FILE_TYPE type);
void init();
void copy_file();
void close_files();
#endif /* 4_10_2_CP_H_ */
