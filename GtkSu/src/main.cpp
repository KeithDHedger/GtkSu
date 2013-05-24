/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>

GtkWidget*	window=NULL;
GtkWidget*	nameEntry=NULL;
GtkWidget*	passEntry=NULL;

void shutdown(GtkWidget* widget,gpointer data)
{
	gtk_main_quit();
}

void doButton(GtkWidget* widget,gpointer data)
{
	printf("button %i\n",(int)(bool)data);
}

int main(int argc,char **argv)
{
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	button;

	gtk_init(&argc,&argv);
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)window,"GtkSu");
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);

	vbox=gtk_vbox_new(true,0);
	nameEntry=gtk_entry_new();
	passEntry=gtk_entry_new();

	gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new("User Name"),false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),nameEntry,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new("Password"),false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),passEntry,false,true,0);

	hbox=gtk_hbox_new(true,0);

	button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),button,false,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doButton),(void*)true);

	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,false,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doButton),(void*)false);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	gtk_widget_show_all(window);
	gtk_main();
}



