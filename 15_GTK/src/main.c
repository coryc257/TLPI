/*
 * main.c
 *
 *  Created on: May 23, 2021
 *      Author: cory
 *      LICENSE: LGPL thingy
 */


#include "tlpi_hdr.h"
#include <gtk/gtk.h>

#include "../lib/file.h"




static GtkWidget *window;
static FILEINFO *fileinfo;
/// SECTION FORM ELEMENTS START
static GtkWidget *file_name_display;
static GtkCheckButton *ck_a;//Append Only
static GtkCheckButton *ck_A;//No Atime Updates
static GtkCheckButton *ck_F;//Case Insensitive Directory Lookups
static GtkCheckButton *ck_c;//Compressed
static GtkCheckButton *ck_D;//Synchronous Directory Updates
static GtkCheckButton *ck_d;//No Dump
static GtkCheckButton *ck_e;//Extent Format
static GtkCheckButton *ck_i;//Immutable
static GtkCheckButton *ck_C;//No Copy On Write
static GtkCheckButton *ck_s;//Secure Deletion
static GtkCheckButton *ck_u;//Undeletable
static GtkCheckButton *ck_S;//Synchronous Updates
static GtkCheckButton *ck_t;//No Tail Merging
static GtkCheckButton *ck_T;//Top of Directory Hierarchy
static GtkCheckButton *ck_P;//Project Hierarchy
static GtkCheckButton *ck_j;//Data Journaling

static GtkButton *cancel;
static GtkButton *save;

static void
__event__cancel__click()
{
	gtk_main_quit();
}

static void
__event__save__click()
{
#define set_fi_val(field,item) {field = (gtk_toggle_button_get_active(item) ? 1 : 0);}
	// TODO SAVE LOGIC
	set_fi_val(fileinfo->ck_a,ck_a);set_fi_val(fileinfo->ck_A,ck_A);
	set_fi_val(fileinfo->ck_c,ck_c);set_fi_val(fileinfo->ck_D,ck_D);
	set_fi_val(fileinfo->ck_d,ck_d);set_fi_val(fileinfo->ck_e,ck_e);
	set_fi_val(fileinfo->ck_i,ck_i);set_fi_val(fileinfo->ck_C,ck_C);
	set_fi_val(fileinfo->ck_s,ck_s);set_fi_val(fileinfo->ck_u,ck_u);
	set_fi_val(fileinfo->ck_S,ck_S);set_fi_val(fileinfo->ck_t,ck_t);
	set_fi_val(fileinfo->ck_T,ck_T);set_fi_val(fileinfo->ck_P,ck_P);
	              set_fi_val(fileinfo->ck_j,ck_j);

	save_file_attr(fileinfo);

	gtk_main_quit();
}

static void
__init__(int argc, char *argv[])
{
	GtkBuilder *builder;
	gtk_init(&argc, &argv);
	builder = gtk_builder_new_from_file("ui/chattr_gui.glade");
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
	gtk_builder_connect_signals(builder, NULL);

	// GET WIDGETS
	file_name_display = GTK_WIDGET(gtk_builder_get_object(builder, "file_name_display"));
	ck_a = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_a"));
	ck_A = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_A"));
	ck_F = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_F"));
	ck_c = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_c"));
	ck_D = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_D"));
	ck_d = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_d"));
	ck_e = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_e"));
	ck_i = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_i"));
	ck_C = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_C"));
	ck_s = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_s"));
	ck_u = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_u"));
	ck_S = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_S"));
	ck_t = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_t"));
	ck_T = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_T"));
	ck_P = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_P"));
	ck_j = (GtkCheckButton*)GTK_WIDGET(gtk_builder_get_object(builder, "ck_j"));

	cancel = (GtkButton*)GTK_WIDGET(gtk_builder_get_object(builder, "bt_cancel"));
	save = (GtkButton*)GTK_WIDGET(gtk_builder_get_object(builder, "bt_save"));
	g_signal_connect(G_OBJECT(cancel),
			"clicked", G_CALLBACK(__event__cancel__click), NULL);
	g_signal_connect(G_OBJECT(save),
			"clicked", G_CALLBACK(__event__save__click), NULL);

	g_object_unref(builder);
	gtk_widget_show(window);
	gtk_widget_hide(ck_F);
}

static void
set_form_info()
{
#define set_flag_val(item,field) if (field == 1) \
	{gtk_toggle_button_set_active(item, TRUE);} else \
	{gtk_toggle_button_set_active(item, FALSE);}
	gtk_entry_set_text((GtkEntry*)file_name_display, fileinfo->file_name);
	set_flag_val(ck_a,fileinfo->ck_a);	set_flag_val(ck_A,fileinfo->ck_A);
	set_flag_val(ck_F,fileinfo->ck_F);	set_flag_val(ck_c,fileinfo->ck_c);
	set_flag_val(ck_D,fileinfo->ck_D);	set_flag_val(ck_e,fileinfo->ck_e);
	set_flag_val(ck_i,fileinfo->ck_i);	set_flag_val(ck_C,fileinfo->ck_C);
	set_flag_val(ck_s,fileinfo->ck_s);	set_flag_val(ck_u,fileinfo->ck_u);
	set_flag_val(ck_a,fileinfo->ck_a);	set_flag_val(ck_S,fileinfo->ck_S);
	set_flag_val(ck_t,fileinfo->ck_t);	set_flag_val(ck_T,fileinfo->ck_T);
	set_flag_val(ck_P,fileinfo->ck_P);	set_flag_val(ck_j,fileinfo->ck_j);
}

/// SECTION FORM ELEMENTS STOP

int
main(int argc, char *argv[] )
{
	if (argc != 2)
		usageErr("%s <file>\n", argv[0]);

	__init__(argc, argv);
	get_file_info(argv[1],&fileinfo);
	set_form_info();
    gtk_main ();

    return 0;
}


