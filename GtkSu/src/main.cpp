/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <sys/stat.h>

#include <pwd.h>
#include <shadow.h>
#include <crypt.h>

GtkWidget*	window=NULL;
GtkWidget*	nameEntry=NULL;
GtkWidget*	passEntry=NULL;

int		gargc;
char**	gargv;
char*	whereFrom;
char*	hashedPass=NULL;

void shutdown(GtkWidget* widget,gpointer data)
{
	gtk_main_quit();
}

int runAsUser(int theuid,char*user,char* hashedpass)
{
	GString*	str=g_string_new(NULL);
	bool		lastwasarg=false;

	g_string_append_printf(str,"%s/gtksuwrap %i '%s' '%s'",whereFrom,theuid,user,hashedpass);

	for(int j=1;j<gargc;j++)
		{
			if(gargv[j][0]=='-')
				lastwasarg=true;
			else
				{
					if(lastwasarg==true)
						lastwasarg=false;
					else
						g_string_append_printf(str," \"%s\"",gargv[j]);
				}
		}

	system(str->str);
	g_string_free(str,true);
	shutdown(NULL,NULL);
	return(0);
}

void doButton(GtkWidget* widget,gpointer data)
{
	FILE*	fp;
	char	buffer[256];
	int		retval;
	char*	command;
	char*	resulthash=NULL;
	int		uid;
	int		itworked;

	if((bool)data==false)
		shutdown(NULL,NULL);
	else
		{
			sprintf(buffer,"id -u %s 2>/dev/null",gtk_entry_get_text((GtkEntry*)nameEntry));
			fp=popen(buffer, "r");
			fgets(buffer,64,fp);
			buffer[strlen(buffer)-1]=0;
			retval=pclose(fp);
			if(retval==0)
				{
					uid=atoi(buffer);
					asprintf(&command,"%s/gtksuwrap gethash %s",whereFrom,(char*)gtk_entry_get_text((GtkEntry*)nameEntry));
					fp=popen(command,"r");
					g_free(command);
					if(fp!=NULL)
						{
							fgets(buffer,255,fp);
							buffer[strlen(buffer)-1]=0;
							pclose(fp);
							asprintf(&hashedPass,"%s",buffer);
							resulthash=crypt((char*)gtk_entry_get_text((GtkEntry*)passEntry),hashedPass);

							if(strcmp(hashedPass,resulthash)==0)
								{
									itworked=runAsUser(uid,(char*)gtk_entry_get_text((GtkEntry*)nameEntry),resulthash);
									if(itworked==0)
										shutdown(NULL,NULL);
									else
										gtk_widget_show_all(window);
								}
						}
					return;
				}
			else
				printf("Unknown User\n");
		}
}

void getPath( )
{
	char	arg1[20];
	char	exepath[PATH_MAX+1]={0};

	sprintf(arg1,"/proc/%d/exe",getpid());
	readlink( arg1,exepath,1024 );
	whereFrom=g_path_get_dirname(exepath);
}

int main(int argc,char **argv)
{
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	buttonok;
	GtkWidget*	button;

	gargc=argc;
	gargv=argv;
	getPath();

	gtk_init(&argc,&argv);

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)window,"GtkSu");
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);

	vbox=gtk_vbox_new(false,0);
	nameEntry=gtk_entry_new();
	gtk_entry_set_text((GtkEntry*)nameEntry,"root");
	g_signal_connect_after(G_OBJECT(nameEntry),"activate",G_CALLBACK(doButton),(void*)true);
	passEntry=gtk_entry_new();
	gtk_entry_set_visibility((GtkEntry*)passEntry,false);
	g_signal_connect_after(G_OBJECT(passEntry),"activate",G_CALLBACK(doButton),(void*)true);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new("User Name"),false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),nameEntry,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new("Password"),false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),passEntry,false,true,0);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,2);

	hbox=gtk_hbutton_box_new();
	gtk_button_box_set_layout((GtkButtonBox*)hbox,GTK_BUTTONBOX_SPREAD);
	
	buttonok=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_container_add(GTK_CONTAINER(hbox),buttonok);
	g_signal_connect(G_OBJECT(buttonok),"clicked",G_CALLBACK(doButton),(void*)true);

	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(""),true,true,0);

	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_container_add(GTK_CONTAINER(hbox),button);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doButton),(void*)false);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,4);
	gtk_container_add(GTK_CONTAINER(window),vbox);

	for(int j=1;j<argc;j++)
		{
			if(argv[j][0]=='-')
				{
					if(argv[j][1]=='u')
						gtk_entry_set_text((GtkEntry*)nameEntry,argv[j+1]);
					if(argv[j][1]=='m')
						gtk_window_set_title((GtkWindow*)window,argv[j+1]);
				}
		}

	gtk_widget_show_all(window);
	gtk_widget_grab_focus(passEntry);
	gtk_main();

	return(0);
}



