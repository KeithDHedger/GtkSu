/*
 *
 * K.D.Hedger 2012-2014 <kdhedger68713@gmail.com>
 *
 * All code is suppled 'as is' use it at your own peril!
 * Released under GPLv3
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
 
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <getopt.h>

#include "config.h"

#define		MYEMAIL "kdhedger68713@gmail.com"
#define		NOSHADOWUSER -1

char*		user;
char*		passwd;

#ifndef _USEQT5_
#include <gtk/gtk.h>
GtkWidget*	window=NULL;
GtkWidget*	nameEntry=NULL;
GtkWidget*	passEntry=NULL;

#else
#include <glib.h>

#include <QtWidgets>
#include <QObject>
QApplication*	holdapp;
QWidget*		mainWindow;
#endif


char**		gargv;
GString*	commandStr=g_string_new(NULL);

char*		whereFrom;
char*		hashedPass=NULL;
char*		userName=NULL;
char*		bodyMessage=NULL;

GString*	runOptions=g_string_new(NULL);

int			returnValFromApp=-1;

void shutItDown(void)
{
#ifdef _USEQT5_
//	return(WEXITSTATUS(returnValFromApp));
	return(holdapp->exit(WEXITSTATUS(returnValFromApp)));
#else
	gtk_main_quit();
#endif
}

struct option long_options[]=
	{
		{"version",0,0,'v'},
		{"user",1,0,'u'},
		{"message",1,0,'m'},
		{"help",0,0,'h'},
		{0, 0, 0, 0}
	};

int runAsUser(int theuid,char*user,char* hashedpass)
{
	GString*	str=g_string_new(NULL);

	g_string_append_printf(str,"%s/gtksuwrap %i '%s' '%s' %s",whereFrom,theuid,user,hashedpass,commandStr->str);
#ifndef _USEQT5_
	gtk_widget_hide(window);
	while(gtk_events_pending())
		gtk_main_iteration_do(false);
#else
	mainWindow->hide();
#endif
	returnValFromApp=system(str->str);

	g_string_free(str,true);

	return(0);
}

void doErrorMessage(const char* message,const char* data,const char* secondmessage)
{
	char*	vmessage;

	asprintf(&vmessage,"%s %s\n%s",message,data,secondmessage);
#ifndef _USEQT5_
	GtkWidget*	dialog;
	dialog=gtk_message_dialog_new((GtkWindow*)window,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,"%s %s\n",message,data);
	gtk_message_dialog_format_secondary_text((GtkMessageDialog*)dialog,"%s\n",secondmessage);
	gtk_dialog_run((GtkDialog*)dialog);
	gtk_widget_destroy(dialog);
#else
	QMessageBox		msgBox;

	msgBox.setText(vmessage);
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.exec();

#endif
	free(vmessage);
}


#ifndef _USEQT5_
void doButton(GtkWidget* widget,gpointer data)
{
	FILE*		fp;
	char		buffer[256];
	int			retval;
	char*		command;
	char*		resulthash=NULL;
	int			uid;
	int			itworked;

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
					errno=0;
					buffer[0]=0;
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

							if((resulthash!=NULL) && (strcmp(hashedPass,resulthash)==0))
								{
									itworked=runAsUser(uid,(char*)gtk_entry_get_text((GtkEntry*)nameEntry),resulthash);
									if(itworked==0)
										shutItDown();
									else
										gtk_widget_show_all(window);
								}
							else
								{
									doErrorMessage("Could not run ",gargv[1],"Username and/or Password incorrect");
								}
						}
					return;
				}
			else
				doErrorMessage("Unknown User ",gtk_entry_get_text((GtkEntry*)nameEntry),"");
		}
}
#endif
void printHelp(void)
{
	printf("GtkSu Version %s \nCopyright K.D.Hedger 2013, %s\n",VERSION,MYEMAIL);
	printf("Usage: gtksu [OPTION] [--] <command>\n");
	printf("Run a command as another user\n");
	printf("-u, --user USER			Runs the command as the given user\n");
	printf("-m, --message MESG		Change default message in ktsuss window\n");

}

void getPath(void)
{
	char	arg1[32];
	char	exepath[PATH_MAX+1]={0};

	sprintf(arg1,"/proc/%d/exe",getpid());
	readlink(arg1,exepath,1024);
	whereFrom=g_path_get_dirname(exepath);
}

#ifdef _USEQT5_
QLineEdit*		nameBox;
QLineEdit*		passBox;

#endif

void doGoForIt(void)
{
//	printf("name=%s\n pass=%s\n",user,passwd);

	FILE*		fp;
	char		buffer[256];
	int			retval;
	char*		command;
	char*		resulthash=NULL;
	int			uid;
	int			itworked;

	sprintf(buffer,"id -u %s 2>/dev/null",user);
	fp=popen(buffer, "r");
	fgets(buffer,64,fp);
	buffer[strlen(buffer)-1]=0;
	retval=pclose(fp);
	if(retval==0)
		{
			uid=atoi(buffer);
			errno=0;
			buffer[0]=0;
			asprintf(&command,"%s/gtksuwrap gethash %s",whereFrom,user);
			fp=popen(command,"r");
			g_free(command);
			if(fp!=NULL)
				{
					fgets(buffer,255,fp);
					buffer[strlen(buffer)-1]=0;
					pclose(fp);
					asprintf(&hashedPass,"%s",buffer);
					resulthash=crypt(passwd,hashedPass);

					if((resulthash!=NULL) && (strcmp(hashedPass,resulthash)==0))
						{
							itworked=runAsUser(uid,user,resulthash);
									if(itworked==0)
										shutItDown();
									//	shutItDown();
									//else
									//	gtk_widget_show_all(window);
						}
					else
						{
							doErrorMessage("Could not run ",gargv[1],"Username and/or Password incorrect");
						}
				}
			return;
		}
	else
		doErrorMessage("Unknown User ",user,"");

}

void doApply(void)
{
#ifdef _USEQT5_
	user=strdup(nameBox->text().toUtf8().constData());
	passwd=strdup(passBox->text().toUtf8().constData());
#else
	user=strdup(gtk_entry_get_text((GtkEntry*)nameEntry));
	passwd=strdup(gtk_entry_get_text((GtkEntry*)passEntry));
#endif
	doGoForIt();
}

int main(int argc,char **argv)
{
	int c;
	int option_index=0;

	while (1)
		{
			option_index=0;
			c=getopt_long_only(argc,argv,"u:m:v?h",long_options,&option_index);

			if (c==-1)
				break;

			switch (c)
				{
					case '?':
					case 'h':
						printHelp();
						return 0;
						break;
			
					case 'u':
						userName=optarg;
						break;

					case 'v':
						printf("GtkSu Version %s \nCopyright K.D.Hedger 2013-2014, %s\n",VERSION,MYEMAIL);
						return 0;
						break;

					case 'm':
						bodyMessage=optarg;
						break;

					default:
						printf ("?? Unknown argument ??\n");
						break;
			}
		}

	gargv=argv;
	getPath();

	for(int j=optind;j<argc;j++)
		g_string_append_printf(commandStr," \"%s\"",argv[j]);

#ifdef _USEQT5_
	QApplication	app(argc, argv);
//	app=new QApplication(argc, argv);
	holdapp=&app;
	QVBoxLayout*	vlayout=new QVBoxLayout;
	QHBoxLayout*	hlayout=new QHBoxLayout;
	QPushButton*	cancelButton=new QPushButton("&Cancel");
	QPushButton*	okButton=new QPushButton("&Apply");
	QWidget*		hbox=new QWidget;

	mainWindow=new QWidget;

	nameBox=new QLineEdit;
	passBox=new QLineEdit;
	nameBox->setText("root");
	okButton->setDefault(true);

	vlayout->addWidget(new QLabel("Name"));
	vlayout->addWidget(nameBox);
	vlayout->addWidget(new QLabel("Password"));
	vlayout->addWidget(passBox);
	passBox->setEchoMode(QLineEdit::Password);

	hbox->setLayout(hlayout);
	hlayout->addWidget(cancelButton);
	hlayout->addWidget(okButton);

	vlayout->addWidget(hbox);

	QObject::connect(cancelButton,SIGNAL(clicked()),qApp,SLOT(quit()));
	QObject::connect(okButton,&QPushButton::clicked,doApply );

	mainWindow->setLayout(vlayout);
	mainWindow->show();

	return app.exec();
#else
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	buttonok;
	GtkWidget*	button;

	gtk_init(&argc,&argv);

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)window,"GtkSu");
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);

	vbox=gtk_vbox_new(false,0);
	nameEntry=gtk_entry_new();
	g_signal_connect_after(G_OBJECT(nameEntry),"activate",G_CALLBACK(doButton),(void*)true);

	if(userName!=NULL)
		{
			gtk_entry_set_text((GtkEntry*)nameEntry,userName);
			gtk_widget_hide(nameEntry);
		}
	else
		{
			gtk_entry_set_text((GtkEntry*)nameEntry,"root");
			gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new("User Name"),false,true,0);
		}

	passEntry=gtk_entry_new();
	gtk_entry_set_visibility((GtkEntry*)passEntry,false);
	g_signal_connect_after(G_OBJECT(passEntry),"activate",G_CALLBACK(doButton),(void*)true);

	gtk_box_pack_start(GTK_BOX(vbox),nameEntry,false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new("Password"),false,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),passEntry,false,true,0);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,2);

	hbox=gtk_hbutton_box_new();
	gtk_button_box_set_layout((GtkButtonBox*)hbox,GTK_BUTTONBOX_SPREAD);
	
	buttonok=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_container_add(GTK_CONTAINER(hbox),buttonok);
//	g_signal_connect(G_OBJECT(buttonok),"clicked",G_CALLBACK(doButton),(void*)true);
	g_signal_connect(G_OBJECT(buttonok),"clicked",G_CALLBACK(doApply),NULL);

	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(""),true,true,0);

	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_container_add(GTK_CONTAINER(hbox),button);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doButton),(void*)false);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,4);
	gtk_container_add(GTK_CONTAINER(window),vbox);

	gtk_widget_show_all(window);

	if(userName!=NULL)
		gtk_widget_hide(nameEntry);

	gtk_widget_grab_focus(passEntry);
	gtk_main();
#endif

	return(WEXITSTATUS(returnValFromApp));
}



