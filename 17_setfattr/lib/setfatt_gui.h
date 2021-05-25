#ifndef SETFATTR_GUI_H
#define SETFATTR_GUI_H

#include <gtk/gtk.h>

#define MAX_XATTR_BUF_LEN 1024

typedef struct x_setfattr_attribute {
	int ordinal_position;
	int name_default;
	GtkWidget *row;
	GtkWidget *button;
	char name[MAX_XATTR_BUF_LEN];
	char value[MAX_XATTR_BUF_LEN];
} X_SET_ATTR;

typedef struct x_setfattr_attribute_list {
	struct x_setfattr_attribute_list *next;
	struct x_setfattr_attribute_list *prev;
	X_SET_ATTR *attr;

} X_SET_LIST;

typedef struct x_setfattr_head {
	X_SET_LIST *first;
	X_SET_LIST *last;
	int count;
	int fd;
} X_SET_HEAD;

#endif
