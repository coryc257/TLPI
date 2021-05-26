#ifndef SETFATTR_GUI_H
#define SETFATTR_GUI_H


#include <gtk/gtk.h>

#ifndef MAX_XATTR_BUF_LEN
#define MAX_XATTR_BUF_LEN 1024
#endif

#ifndef NAME_GEN_START
#define NAME_GEN_START 1000
#endif

typedef struct x_setfattr_attribute {
	int ordinal_position;
	int name_default;
	int fresh;
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

static void btn_new__onClick();
static void display_attr(X_SET_ATTR *attr);
static void save_attr(X_SET_ATTR *attr);
static void delete_attr(X_SET_ATTR *attr);
static void load_file(char *file);
static void show_error_message(char *section, char *format, ...);
static void btn_item_clicked(GtkWidget *sender, gpointer *handle);
static void btn_save_clicked(GtkWidget *sender, gpointer *handle);
static void btn_delete_clicked(GtkWidget *sender, gpointer *handle);
static void ui_new_attr(int i);
static X_SET_ATTR *add_attr(char *name, char * value, int i);
static void window_main_show(GtkWidget *sender, gpointer *data);
static void __init__(char *xfile);

#endif
