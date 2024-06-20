/*
 *
 * ©K. D. Hedger. Sun 27 Sep 20:10:42 BST 2015 keithdhedger@gmail.com

 * This file (main.cpp) is part of GtkSu.

 * GtkSu is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * GtkSu is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with GtkSu.  If not, see <http://www.gnu.org/licenses/>.
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
#include "internet.h"

#define		NOSHADOWUSER -1
#define		GTKPADDING 4
char*		user;
char*		passwd;

#ifndef _USEQT_
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

void shutItDown(gpointer x,gpointer y)
{
#ifdef _USEQT_
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
#ifndef _USEQT_
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
#ifndef _USEQT_
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

void printHelp(void)
{
	printf("GtkSu Version %s \nCopyright K.D.Hedger 2013-2019, %s\n",VERSION,MYEMAIL);
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

#ifdef _USEQT_
QLineEdit*		nameBox;
QLineEdit*		passBox;

#endif

void doGoForIt(void)
{
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
										shutItDown(NULL,NULL);
						}
					else
						{
							doErrorMessage("Could not run ",commandStr->str,"Username and/or Password incorrect");
						}
				}
			return;
		}
	else
		doErrorMessage("Unknown User ",user,"");

}

void doApply(void)
{
#ifdef _USEQT_
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
						printf("GtkSu Version %s \n%s, %s\n",VERSION,COPYRITE,MYEMAIL);
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

#ifdef _USEQT_
	QApplication	app(argc, argv);

	QVBoxLayout*	vlayout=new QVBoxLayout;
	QHBoxLayout*	hlayout;
	QPushButton*	cancelButton=new QPushButton("&Cancel");
	QPushButton*	okButton=new QPushButton("&Apply");
	QWidget*		hbox;
	QLabel*			label;

	holdapp=&app;
	mainWindow=new QWidget;
	nameBox=new QLineEdit;
	passBox=new QLineEdit;
	nameBox->setText("root");

	if(userName!=NULL)
		{
			nameBox->setText(userName);
			nameBox->hide();
		}
	else
		nameBox->setText("root");

	if(bodyMessage!=NULL)
			label=new QLabel(bodyMessage);
	else
		label=new QLabel("Please enter the desired username and password:");

	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox=new QWidget;
	hbox->setLayout(hlayout);
	label->setAlignment(Qt::AlignCenter);
	hlayout->addWidget(label);
	vlayout->addWidget(hbox);

	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox=new QWidget;
	hbox->setLayout(hlayout);
	if(userName==NULL)
		{
			hlayout->addWidget(new QLabel("User Name"),Qt::AlignLeft);
			hlayout->addWidget(nameBox,Qt::AlignRight);
			vlayout->addWidget(hbox);
		}

	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox=new QWidget;
	hbox->setLayout(hlayout);
	hlayout->addWidget(new QLabel("Password"),Qt::AlignLeft);
	hlayout->addWidget(passBox,Qt::AlignRight);
	passBox->setEchoMode(QLineEdit::Password);
	vlayout->addWidget(hbox);

	hbox=new QWidget;
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox->setLayout(hlayout);
	hlayout->addWidget(cancelButton);
	hlayout->addStretch(0);
	hlayout->addWidget(okButton);

	vlayout->addWidget(hbox);

	QObject::connect(cancelButton,SIGNAL(clicked()),qApp,SLOT(quit()));
	QObject::connect(okButton,&QPushButton::clicked,doApply );
	QObject::connect(passBox,&QLineEdit::returnPressed,doApply );

	mainWindow->setLayout(vlayout);

	passBox->setFocus();
	mainWindow->show();

	app.exec();

#else
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	buttonok;
	GtkWidget*	button;

	gtk_init(&argc,&argv);

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)window,"GtkSu");
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutItDown),NULL);

	vbox=gtk_vbox_new(false,0);
	nameEntry=gtk_entry_new();
	g_signal_connect_after(G_OBJECT(nameEntry),"activate",G_CALLBACK(doApply),NULL);

	if(bodyMessage!=NULL)
		gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new(bodyMessage),false,true,GTKPADDING);
	else
		gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new("Please enter the desired username and password:"),false,true,GTKPADDING);

	if(userName!=NULL)
		{
			gtk_entry_set_text((GtkEntry*)nameEntry,userName);
			gtk_widget_hide(nameEntry);
		}
	else
		{
			hbox=gtk_hbox_new(false,0);
			gtk_entry_set_text((GtkEntry*)nameEntry,"root");
			gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("User Name\t"),false,true,0);
			gtk_box_pack_start(GTK_BOX(hbox),nameEntry,true,true,0);
			gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,GTKPADDING);
		}

	passEntry=gtk_entry_new();
	gtk_entry_set_visibility((GtkEntry*)passEntry,false);
	g_signal_connect_after(G_OBJECT(passEntry),"activate",G_CALLBACK(doApply),NULL);

	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Password\t"),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),passEntry,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,GTKPADDING);

	hbox=gtk_hbutton_box_new();
	gtk_button_box_set_layout((GtkButtonBox*)hbox,GTK_BUTTONBOX_SPREAD);
	
	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_container_add(GTK_CONTAINER(hbox),button);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(shutItDown),NULL);

	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(""),true,true,0);

	buttonok=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_container_add(GTK_CONTAINER(hbox),buttonok);
	g_signal_connect(G_OBJECT(buttonok),"clicked",G_CALLBACK(doApply),NULL);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,GTKPADDING);
	gtk_container_add(GTK_CONTAINER(window),vbox);

	gtk_widget_show_all(window);

	if(userName!=NULL)
		gtk_widget_hide(nameEntry);

	gtk_widget_grab_focus(passEntry);
	gtk_main();
#endif

	return(WEXITSTATUS(returnValFromApp));
}



