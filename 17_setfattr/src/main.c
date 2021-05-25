/*
 * main.c
 *
 *  Created on: May 24, 2021
 *      Author: cory
 */

#include <gtk/gtk.h>
#include "../lib/setfatt_gui.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/xattr.h>


static GtkWidget *window;
static GtkWidget *left_list_box;
static GtkWidget *btn_new_attr, *btn_delete, *btn_save;
static GtkWidget *txt_attribute_name, *txt_attribute_value;
static X_SET_HEAD *x_attrs;
static int num_attr;
static int name_gen;
static int current_index;
static gulong save_handle;
static gulong delete_handle;
static int sd_handle;

static void btn_new__onClick();
static void display_attr(X_SET_ATTR *attr);
static void save_attr(X_SET_ATTR *attr);
static void delete_attr(X_SET_ATTR *attr);
static void load_file(char *file);
static char *file;

static void
btn_item_clicked(GtkWidget *sender, gpointer *handle)
{
	display_attr((X_SET_ATTR*)handle);
}

static void
btn_save_clicked(GtkWidget *sender, gpointer *handle)
{
	save_attr((X_SET_ATTR*)handle);
}

static void
btn_delete_clicked(GtkWidget *sender, gpointer *handle)
{
	delete_attr((X_SET_ATTR*)handle);
}

static void
display_attr(X_SET_ATTR *attr)
{
	if (sd_handle)
	{
		sd_handle = 0;
		g_signal_handler_disconnect(G_OBJECT(btn_save),save_handle);
		g_signal_handler_disconnect(G_OBJECT(btn_delete),delete_handle);
	}
	gtk_entry_set_text((GtkEntry*)txt_attribute_name, (gchar *)attr->name);
	gtk_entry_set_text((GtkEntry*)txt_attribute_value, (gchar *)attr->value);
	gtk_widget_activate(GTK_WIDGET(attr->row));
	save_handle = g_signal_connect(G_OBJECT(btn_save), "clicked", G_CALLBACK(btn_save_clicked), attr);
	delete_handle = g_signal_connect(G_OBJECT(btn_delete), "clicked", G_CALLBACK(btn_delete_clicked), attr);
	sd_handle = 1;
}

static void
ui_new_attr(int i)
{
	X_SET_ATTR *new_attr;
	if ((new_attr = malloc(sizeof(X_SET_ATTR))) == NULL)
		gtk_main_quit();

	current_index = num_attr;

	memset(new_attr->name,0,MAX_XATTR_BUF_LEN);
	memset(new_attr->value,0,MAX_XATTR_BUF_LEN);

	new_attr->button = GTK_WIDGET(gtk_button_new());
	new_attr->row = GTK_WIDGET(gtk_list_box_row_new());
	new_attr->ordinal_position = num_attr;
	new_attr->name_default = name_gen++;

	sprintf(new_attr->name, "attr_%d", new_attr->name_default);

	g_signal_connect(G_OBJECT(new_attr->button), "clicked", G_CALLBACK(btn_item_clicked), (gpointer)new_attr);

	gtk_container_add(GTK_CONTAINER(new_attr->row),GTK_WIDGET(new_attr->button));
	gtk_list_box_insert(GTK_LIST_BOX(left_list_box),GTK_WIDGET(new_attr->row),(gint)i);
	gtk_widget_show(new_attr->button);
	gtk_widget_show(new_attr->row);
	display_attr(new_attr);
}

static void
add_attr(char *name, char * value, int i)
{
	if(strlen(name)>999 || strlen(value)>999)
	{
		//TODO
		return;
	}
	X_SET_ATTR *new_attr;
	if ((new_attr = malloc(sizeof(X_SET_ATTR))) == NULL)
		gtk_main_quit();

	current_index = num_attr;

	memset(new_attr->name,0,MAX_XATTR_BUF_LEN);
	memset(new_attr->value,0,MAX_XATTR_BUF_LEN);

	new_attr->button = GTK_WIDGET(gtk_button_new());
	new_attr->row = GTK_WIDGET(gtk_list_box_row_new());
	new_attr->ordinal_position = num_attr;
	new_attr->name_default = name_gen++;

	sprintf(new_attr->name, "%s", name);
	sprintf(new_attr->value, "%s", value);

	gtk_button_set_label(GTK_BUTTON((new_attr->button)), (gchar *)(new_attr->name));

	g_signal_connect(G_OBJECT(new_attr->button), "clicked", G_CALLBACK(btn_item_clicked), (gpointer)new_attr);

	display_attr(new_attr);
	gtk_container_add(GTK_CONTAINER(new_attr->row),GTK_WIDGET(new_attr->button));
	gtk_list_box_insert(GTK_LIST_BOX(left_list_box),GTK_WIDGET(new_attr->row),i);
	gtk_widget_show(new_attr->button);
	gtk_widget_show(new_attr->row);
}

static void
window_main_show(GtkWidget *sender, gpointer *data)
{
	load_file(file);
}

static void
__init__(char *xfile)
{
	name_gen = 1000;
	GtkBuilder *builder;
	builder = gtk_builder_new_from_file("ui/setfattr.glade");
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
	gtk_builder_connect_signals(builder, NULL);

	left_list_box = GTK_WIDGET(gtk_builder_get_object(builder, "left_list_box"));
	btn_new_attr = GTK_WIDGET(gtk_builder_get_object(builder, "btn_new_attr"));
	txt_attribute_value = GTK_WIDGET(gtk_builder_get_object(builder, "txt_attribute_value"));
	txt_attribute_name = GTK_WIDGET(gtk_builder_get_object(builder, "txt_attribute_name"));
	btn_save = GTK_WIDGET(gtk_builder_get_object(builder, "btn_save"));
	btn_delete = GTK_WIDGET(gtk_builder_get_object(builder, "btn_delete"));
	num_attr = 0;

	x_attrs = malloc(sizeof(X_SET_HEAD));
	x_attrs->first = NULL;
	x_attrs->last = NULL;
	x_attrs->count = 0;

	g_signal_connect(G_OBJECT(btn_new_attr), "clicked", G_CALLBACK(btn_new__onClick), NULL);
	g_signal_connect(G_OBJECT(window), "show", G_CALLBACK(window_main_show), NULL);

	file = xfile;
	sd_handle = 0;


	g_object_unref(builder);
	gtk_widget_show(window);
}

int
main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "%s <file>\n", argv[0]);
		return -1;
	}
	gtk_init(&argc, &argv);
	__init__(argv[1]);
	gtk_main();
}

void btn_new__onClick()
{
	ui_new_attr(++num_attr);
}

static void
load_file(char *file)
{
	char *list = NULL;
	char *value = NULL;
	ssize_t req_size, sec_size, real_size;
	ssize_t offset = 0;
	x_attrs->fd = open(file, O_RDONLY);
	if(x_attrs->fd == -1)
	{
		fprintf(stderr, "open file: %s\n", file);
		exit(EXIT_FAILURE);
	}

	list = NULL;
	value = NULL;

	sec_size = 1;
	req_size = 0;
	while(1)
	{
		sec_size = flistxattr(x_attrs->fd, list, req_size);
		if(sec_size == req_size)
			break;

		req_size = sec_size;

		if (list != NULL)
			free(list);

		list = malloc(sizeof(char)+sizeof(char)*req_size);
		if (list == NULL)
			exit(EXIT_FAILURE);

		printf("Attempting Read\n");
	}

	real_size = sec_size;

	while (offset < real_size)
	{
		sec_size = 1;
		req_size = 0;
		for (int j = 0; j < 5; j++)
		{
			sec_size = fgetxattr(x_attrs->fd, &list[offset], value, req_size);
			if (sec_size == req_size)
			{
				value[req_size] = '\0';
				add_attr(&list[offset], value, ++num_attr);
				break;
			}
			req_size = sec_size;
			if (value != NULL)
				free(value);
			value = malloc(sizeof(char)+(sizeof(char)*req_size));
			if (value == NULL)
				exit(EXIT_FAILURE);

		}

		offset += strlen(&list[offset])+1;
	}
}

static void
save_attr(X_SET_ATTR *attr)
{
	gchar *name;
	gchar *value;
	name = (gchar *)gtk_entry_get_text(GTK_ENTRY(txt_attribute_name));
	value = (gchar *)gtk_entry_get_text(GTK_ENTRY(txt_attribute_value));
	if(strlen(name)>999 || strlen(value)>999)
	{
		return;
	}

	if ( strcmp(name,attr->name) != 0)
	{
		fremovexattr(x_attrs->fd, attr->name);
	}

	gtk_button_set_label(GTK_BUTTON(attr->button), name);
	memset(attr->name,0,MAX_XATTR_BUF_LEN);
	memset(attr->value,0,MAX_XATTR_BUF_LEN);
	sprintf(attr->name, "%s", name);
	sprintf(attr->value, "%s", value);

	fsetxattr(x_attrs->fd, name, value, strlen(value), 0);
}

static void
delete_attr(X_SET_ATTR *attr)
{
	if(fremovexattr(x_attrs->fd, attr->name) == -1)
	{
		fprintf(stderr, "fremovexattr:%s\n", attr->name);
		return;
	}

	gtk_container_remove(GTK_CONTAINER(left_list_box), GTK_WIDGET(attr->row));
	free(attr);

}

