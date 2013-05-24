/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 */


//#include <stdio.h>
//#include <stdlib.h>
#include <pwd.h>
#include <shadow.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include <stdlib.h>
//#include <gtk/gtk.h>
//#include <string.h>
//#include <sys/stat.h>
//#include <crypt.h>
//#include <shadow.h>
//
//#define UPAP_AUTHNAK 1
//#define UPAP_AUTHACK 0
//#define USE_SHADOW
//char*	username;

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <sys/stat.h>


int checkPasswd(char* username,char* passwd)
{
	char	*result;
	spwd*	shadow_entry=NULL;
	setuid(0);
	shadow_entry=getspnam(username);
	if(shadow_entry==NULL)
		{
		printf(":(\n");
		return(1);
		}
	result=crypt(passwd,shadow_entry->sp_pwdp);

	return(strcmp(result,shadow_entry->sp_pwdp));
}

GtkWidget*	window=NULL;
GtkWidget*	nameEntry=NULL;
GtkWidget*	passEntry=NULL;

int			gargc;
char**		gargv;

void shutdown(GtkWidget* widget,gpointer data)
{
	gtk_main_quit();
}

//int runAsUser(void)
//{
//	int			ret;
//	int			j;
//	GString*	str=g_string_new(NULL);
//
//	for(j=1;j<argc;j++)
	//	{
		//	if(argv[j][0]!='-')
			//	g_string_append_printf(str," \"%s\"",argv[j]);
		//}

//	setresuid(0,0,0);
//	ret=system(str->str);
//	return(ret);
//}

void doButton(GtkWidget* widget,gpointer data)
{
	FILE*	fp;
	char	buffer[64];
	int		retval;

	printf("button %i\n",(int)(bool)data);
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
					if(checkPasswd((char*)gtk_entry_get_text((GtkEntry*)nameEntry),(char*)gtk_entry_get_text((GtkEntry*)passEntry))==0)
						printf("user and pass ok\n");
					else
						printf("user and pass :(:(\n");
				}
		}
}

int main(int argc,char **argv)
{


//	struct spwd* shadow_entry;
//	shadow_entry=getspnam("keithhedger");
//	printf("%s \n",shadow_entry->sp_namp);
//	return 0;


	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	buttonok;
	GtkWidget*	button;

	gargc=argc;
	gargv=argv;

	gtk_init(&argc,&argv);
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)window,"GtkSu");
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);

	vbox=gtk_vbox_new(false,0);
	nameEntry=gtk_entry_new();
	g_signal_connect_after(G_OBJECT(nameEntry),"activate",G_CALLBACK(doButton),(void*)true);
	passEntry=gtk_entry_new();
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

	gtk_widget_set_can_default(buttonok,true);
	gtk_widget_grab_default(buttonok);
	gtk_widget_show_all(window);


	gtk_main();
}



