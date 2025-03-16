/***************************************************************************
 *   Author Alan Crispin                                                   *
 *   crispinalan@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation.                                         *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include <gtk/gtk.h>
#include <ctype.h>

#include "dictionary.h"
#include "diphone.h"

static GMutex lock;

static void callbk_preferences(GSimpleAction* action, GVariant *parameter,gpointer user_data);

static void callbk_open_file(GSimpleAction* action, GVariant *parameter, gpointer user_data);
static void callbk_quit(GSimpleAction* action,G_GNUC_UNUSED GVariant *parameter, gpointer user_data);
static void callbk_about(GSimpleAction* action, GVariant *parameter, gpointer user_data);

static void callbk_speak(GSimpleAction * action,G_GNUC_UNUSED GVariant *parameter, gpointer user_data);

static void task_callbk(GObject *gobject,GAsyncResult *res,  gpointer  user_data);

unsigned char *rawcat(unsigned char *arrys[], unsigned int arry_size[], int arry_count);
unsigned int get_merge_size(unsigned int sizes_arry[], int arry_size);

static void play_audio_async (GTask *task,
                          gpointer object,
                          gpointer task_data,
                          GCancellable *cancellable);

static int m_talk_rate=19000;
gboolean m_talking=FALSE;


//--------------------------------------------------------------------
// Removers (unwanted characters}
//--------------------------------------------------------------------

static char *remove_commas(const char *text)
{
	GString *str;
	const char *p;
	str = g_string_new("");
	p = text;
	while (*p)
	{
		gunichar cp = g_utf8_get_char(p);
		if (cp != ',')
		{ 
			g_string_append_unichar(str, *p);
		} // if
		++p;
	}
	return g_string_free(str, FALSE);
}

static char *remove_fullstop(const char *text)
{
	GString *str;
	const char *p;
	str = g_string_new("");
	p = text;
	while (*p)
	{
		gunichar cp = g_utf8_get_char(p);
		if (cp != '.')
		{ 
			g_string_append_unichar(str, *p);
		} // if
		++p;
	}
	return g_string_free(str, FALSE);
}
static char* remove_semicolons (const char *text)
{
	GString *str;
	const char *p;
	str = g_string_new ("");
	p = text;
	while (*p)
	{
	gunichar cp = g_utf8_get_char(p);
	if ( cp != ';' ){ 
	g_string_append_unichar (str, *p);
	}//if
	++p;
	}
	return g_string_free (str, FALSE);
}

static char* remove_question_marks (const char *text)
{
	GString *str;
	const char *p;
	str = g_string_new ("");
	p = text;
	while (*p)
	{
	gunichar cp = g_utf8_get_char(p);
	if ( cp != '?' ){ 
	g_string_append_unichar (str, *p);
	}//if
	++p;
	}
	return g_string_free (str, FALSE);
}

static char* remove_explanation_marks (const char *text)
{
	GString *str;
	const char *p;
	str = g_string_new ("");
	p = text;
	while (*p)
	{
	gunichar cp = g_utf8_get_char(p);
	if ( cp != '!' ){ 
	g_string_append_unichar (str, *p);
	}//if
	++p;
	}
	return g_string_free (str, FALSE);
}

static char* remove_punctuations(const char *text)
{
	GString *str;
	const char *p;
	str = g_string_new ("");
	p = text;
	while (*p)
	{
	gunichar cp = g_utf8_get_char(p);
	if ( cp != '\'' ){ //remove all apostrophes
	g_string_append_unichar (str, *p);
	}//if
	++p;
	}
	return g_string_free (str, FALSE);
}

static char* replace_hypens(const char *text)
{
	GString *str;
	const char *p;
	str = g_string_new ("");
	p = text;
	gint i=0;
	while (*p)
	{
	gunichar cp = g_utf8_get_char(p);
	if ( cp != '-' ){ //append
	g_string_append_unichar (str, *p);
	}//if
	if ( cp == '-' ){ //replace			
	g_string_insert_unichar (str,i,' ');
	}//if	
	++p;
	++i;
	}
	return g_string_free (str, FALSE);
}

static char* replace_newlines(const char *text)
{
	GString *str;
	const char *p;
	str = g_string_new ("");
	p = text;
	gint i=0;
	while (*p)
	{
	gunichar cp = g_utf8_get_char(p);
	if ( cp != '\n' ){ //append
	g_string_append_unichar (str, *p);
	}//if
	if ( cp == '\n' ){ //replace			
	g_string_insert_unichar (str,i,' ');
	}//if	
	++p;
	++i;
	}
	return g_string_free (str, FALSE);
}

//======================================================================

char *trim_whitespace(char *s) 
{
  // returns a pointer to the (shifted) trimmed string
  char *original = s;
  size_t len = 0;

  while (isspace((unsigned char) *s)) {
    s++;
  } 
  if (*s) {
    char *p = s;
    while (*p) p++;
    while (isspace((unsigned char) *(--p)));
    p[1] = '\0';
    // len = (size_t) (p - s);   // older errant code
    len = (size_t) (p - s + 1);  // Thanks to @theriver
  }

  return (s == original) ? s : memmove(original, s, len + 1);
}


//======================================================================
// Speaking
//======================================================================

static void task_callbk(GObject *gobject,GAsyncResult *res,  gpointer  user_data)
{		
	//the task callbk function is called back when the 
	//play_audio_async function has completed
	//m_talking is reset to false so that we can speak again
		
    m_talking=FALSE; 
    //g_print("gtask finished reset m_talking = %d\n",m_talking);	
    
}

//======================================================================
static void play_audio_async (GTask *task,
                          gpointer object,
                          gpointer task_data,
                          GCancellable *cancellable)
{
   
    m_talking=TRUE; //stop any new speaking 
    char* raw_file_path =task_data;    
    gchar *m_sample_rate_str = g_strdup_printf("%i", m_talk_rate); 
    gchar *sample_rate_str ="-r ";    
    sample_rate_str= g_strconcat(sample_rate_str,m_sample_rate_str, NULL);     
    gchar * command_str ="aplay -c 1 -f S16_LE";
    //gchar * command_str ="aplay -c 1 -f U8";
    command_str =g_strconcat(command_str," ",sample_rate_str, " ", raw_file_path, NULL);     
    system(command_str);   
   
    //m_talking=FALSE;   
    g_task_return_boolean(task, TRUE);
}

//======================================================================
// Concatentation
//======================================================================

unsigned char *rawcat(unsigned char *arrys[], unsigned int arry_size[], int arry_count) {
		
	
	if (arry_count<2) return NULL;	
	
	unsigned int  total_samples=0;
	for (int c = 0; c < arry_count; c++) 
	{  
    unsigned int count =arry_size[c]; 
    total_samples=total_samples+count;	
    }        
	unsigned char *data = (unsigned char*)malloc(total_samples * sizeof(unsigned char));
	
	unsigned int offset=0;
	for(int k=0; k<arry_count; k++)
	{
		//loop through each arry	
		for(int i = 0; i < arry_size[k]; i++) 
		{		
		data[i+offset]=arrys[k][i];
		}		
		offset =offset+arry_size[k];
	}//k kount
	return data;
}
//=====================================================================
unsigned int get_merge_size(unsigned int sizes_arry[], int arry_size){
	
	unsigned int total_samples=0;
	for (int i = 0; i < arry_size; i++) 
	{  
    unsigned int count =sizes_arry[i]; 
    total_samples=total_samples+count;	
    }
	return total_samples;
}
//======================================================================
// Speak
//======================================================================

static void callbk_speak(GSimpleAction * action,G_GNUC_UNUSED GVariant *parameter, gpointer user_data)
{
	
	GtkWindow *window =user_data;
	
	GtkEntryBuffer *buffer_text; 	 
	GtkWidget *entry_text = g_object_get_data(G_OBJECT(window), "window-text-entry-key");
	buffer_text = gtk_entry_get_buffer (GTK_ENTRY(entry_text));	
	const char* speak_text= gtk_entry_buffer_get_text (buffer_text);
		
	char* copy_str = g_strdup(speak_text);//duplicate
	speak_text=trim_whitespace(copy_str);	
		
	const char *text =g_ascii_strdown(speak_text, -1);
	g_print("entered text = %s\n", text);
	
	
	//very simple input text processing
	text= replace_newlines(text);
	text = remove_semicolons(text);
	text = remove_commas(text);
	text = remove_fullstop(text);
	text = remove_question_marks(text);	
	text= remove_explanation_marks(text);
	text =remove_punctuations(text);
	text = replace_hypens(text); //replace hypens with space
	
	g_print("processed text = %s\n", text);
	
	gchar** words;	
	GList *diphone_list = NULL;			
	words = g_strsplit (text, " ", 0); //split on space	
	int j=0; //count			   
	while(words[j] != NULL)
	{	
	g_print("word =%s\n",words[j]);
	diphone_list =g_list_concat(diphone_list,word_to_diphones(words[j]));
	//diphone_list =g_list_concat(diphone_list, word_to_diphones("pau"));
	j++;
	} //while loop words
		
	gpointer diphone_list_pointer;
	gchar* diphone_str;		
	gint diphone_number  =g_list_length(diphone_list);
	g_print("diphone_number = %i\n", diphone_number);
	//create diphone array using list length
	unsigned char *diphone_arrays[diphone_number]; 
	unsigned int diphone_arrays_sizes[diphone_number];
		
	//load diphone arrays
	for(int i=0; i <diphone_number; i++)
	{
		diphone_list_pointer=g_list_nth_data(diphone_list,i);
		diphone_str=(gchar *)diphone_list_pointer;					
		g_print("diphone = %s\n",diphone_str);		
		diphone_arrays[i] = load_diphone_arry(diphone_str);
		g_print("diphone length = %i\n",load_diphone_len(diphone_str));
		diphone_arrays_sizes[i]=load_diphone_len(diphone_str);		
	}	
	
	//concatenate using raw cat
	unsigned char *data = rawcat(diphone_arrays, diphone_arrays_sizes, diphone_number);	
	unsigned int data_len = get_merge_size(diphone_arrays_sizes,diphone_number);	
    
	gchar* raw_file_path ="/tmp/talkout.raw";
	FILE* f = fopen(raw_file_path, "w");
    fwrite(data, data_len, 1, f);
    fclose(f); 
    
	GTask* task = g_task_new(NULL, NULL, task_callbk, NULL);
	g_task_set_task_data (task,raw_file_path,NULL);
    g_task_run_in_thread(task, play_audio_async);     
    g_object_unref(task);
	
	//clean up 
	g_list_free(diphone_list);	
	free(data);	
	
	
}

//======================================================================
// callbk preferences
//======================================================================
static void callbk_set_preferences(GtkButton *button, gpointer  user_data)
{
	
	GtkWidget *dialog = user_data; //should user data be the dialog

	GtkWidget *spin_button_sample_rate = g_object_get_data(G_OBJECT(button), "spin-sample-rate-key");
	m_talk_rate = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button_sample_rate));
	
	gtk_window_destroy(GTK_WINDOW(dialog));
	
}

static void callbk_preferences(GSimpleAction* action, GVariant *parameter,gpointer user_data)
{	
	//g_print("Preferences");
	GtkWidget *window =user_data;
	
	GtkWidget *dialog;
	GtkWidget *box;
	GtkWidget *button_set;
	
	GtkWidget *label_sample_rate;
	GtkWidget *spin_button_sample_rate;
	GtkWidget *box_sample_rate;
		
	dialog =gtk_window_new(); //gtk_dialog_new_with_buttons to be deprecated gtk4.10

	gtk_window_set_title (GTK_WINDOW (dialog), "Preferences");
	gtk_window_set_default_size(GTK_WINDOW(dialog),350,100);

	box =gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	gtk_window_set_child (GTK_WINDOW (dialog), box);

	button_set = gtk_button_new_with_label ("Set Preferences");
	g_signal_connect (button_set, "clicked", G_CALLBACK (callbk_set_preferences), dialog);
	
	
	//sample rate
	GtkAdjustment *adjustment_sample_rate;
	// value,lower,upper,step_increment,page_increment,page_size
	adjustment_sample_rate = gtk_adjustment_new(14000.00, 4000.00, 20000.00, 1000.0, 1000.0, 0.0);
	// start time spin
	label_sample_rate = gtk_label_new("Voice Sampling Rate ");
	spin_button_sample_rate = gtk_spin_button_new(adjustment_sample_rate, 500, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button_sample_rate), m_talk_rate);
	box_sample_rate = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	gtk_box_append(GTK_BOX(box_sample_rate), label_sample_rate);
	gtk_box_append(GTK_BOX(box_sample_rate), spin_button_sample_rate);

	//data setters	
	g_object_set_data(G_OBJECT(button_set), "spin-sample-rate-key", spin_button_sample_rate);
	
	gtk_box_append(GTK_BOX(box), box_sample_rate);	
	gtk_box_append(GTK_BOX(box), button_set);
	gtk_window_present (GTK_WINDOW (dialog));	
	
}

//======================================================================
// callbk clear entry
//======================================================================

static void callbk_clear(GtkButton *button, gpointer  user_data)
{		
	GtkWindow *window =user_data;
	g_print("Clear called\n");			
	
	GtkWidget *entry_text = g_object_get_data(G_OBJECT(window), "window-text-entry-key");
	GtkEntryBuffer *buffer_text =gtk_entry_buffer_new ("",-1);
	gtk_entry_set_buffer (GTK_ENTRY(entry_text), buffer_text);
}

//======================================================================
// callbk quit
//======================================================================

static void callbk_quit(GSimpleAction * action,	G_GNUC_UNUSED GVariant *parameter, gpointer user_data)
{
	g_application_quit(G_APPLICATION(user_data));		
}
//======================================================================
// callbk about
//======================================================================
static void callbk_about(GSimpleAction* action, GVariant *parameter, gpointer user_data)
{
	GtkWidget *window = user_data;
	const gchar *authors[] = {"Alan Crispin", NULL};
	GtkWidget *about_dialog;
	about_dialog = gtk_about_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(about_dialog),GTK_WINDOW(window));
	gtk_widget_set_size_request(about_dialog, 200,200);
    gtk_window_set_modal(GTK_WINDOW(about_dialog),TRUE);
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "Diphone Talker");
	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG(about_dialog), "0.2");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog),"Copyright © 2025");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog),"Diphone Speech Synthesizer");
	gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG(about_dialog), GTK_LICENSE_LGPL_2_1);
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dialog),"https://github.com/crispinprojects/");
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about_dialog),"Project Website");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), authors);
	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(about_dialog), "x-office-calendar");
	gtk_widget_set_visible (about_dialog, TRUE);	
}

//======================================================================
// activate
//======================================================================

static void activate (GtkApplication* app, gpointer user_data)
{
	GtkWidget *window;			
	GtkWidget *vbox;  
	GtkWidget *label_text;  
	GtkEntryBuffer *buffer_text; 
	GtkWidget *entry_text;
	
	GtkWidget *header;
	GtkWidget *button_speak;	
	GtkWidget *button_clear;	
	GtkWidget *menu_button;
	
	//GMenu *menu;
	GMenu *menu;
	GMenu *section;	
					
	//setup window
	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "Speech Synthesizer");
	gtk_window_set_default_size (GTK_WINDOW (window),550,160);
	header = gtk_header_bar_new ();  
    gtk_window_set_titlebar (GTK_WINDOW (window), header);
	
	button_speak = gtk_button_new_with_label ("Speak");
    g_signal_connect (button_speak, "clicked", G_CALLBACK (callbk_speak), window);
    gtk_header_bar_pack_start(GTK_HEADER_BAR (header), button_speak);
    
    //button clear
	button_clear = gtk_button_new_with_label ("Clear");	
	gtk_widget_set_tooltip_text(button_clear, "Clear Text");
	g_signal_connect (button_clear, "clicked", G_CALLBACK (callbk_clear), window); 
	gtk_header_bar_pack_start(GTK_HEADER_BAR (header), button_clear);
	
	
	vbox =gtk_box_new(GTK_ORIENTATION_VERTICAL,1);  
	gtk_window_set_child (GTK_WINDOW (window), vbox);
	
	//create label
	label_text =gtk_label_new("Enter text ");
	    
	entry_text =gtk_entry_new(); 	
	gtk_widget_set_hexpand (GTK_WIDGET (entry_text), true);
	buffer_text =gtk_entry_buffer_new ("I am a speech synthesizer for speaking dates",-1);
	gtk_entry_set_buffer (GTK_ENTRY(entry_text), buffer_text);
				
	gtk_box_append(GTK_BOX(vbox), label_text); 
    gtk_box_append(GTK_BOX(vbox), entry_text);
	
	//actions
	GSimpleAction *quit_action;	
	quit_action=g_simple_action_new("quit",NULL); //app.quit
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(quit_action)); //make visible	
	g_signal_connect(quit_action, "activate",  G_CALLBACK(callbk_quit), app);

	GSimpleAction *speak_action;	
	speak_action=g_simple_action_new("speak",NULL); //app.speak
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(speak_action)); //make visible	
	g_signal_connect(speak_action, "activate",  G_CALLBACK(callbk_speak), window);
			
	GSimpleAction *preferences_action;
	preferences_action=g_simple_action_new("preferences",NULL); //app.preferences
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(preferences_action)); //make visible
	g_signal_connect(preferences_action, "activate",  G_CALLBACK(callbk_preferences), window);
	
	GSimpleAction *about_action;
	about_action=g_simple_action_new("about",NULL); //app.about
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(about_action)); //make visible
	g_signal_connect(about_action, "activate",  G_CALLBACK(callbk_about), window);
	
    //create menu
    menu = g_menu_new();
	section = g_menu_new ();
	g_menu_append (section, "Speak", "app.speak"); 	
	g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
	g_object_unref (section);		
	section = g_menu_new ();
	g_menu_append (section, "Preferences", "app.preferences"); 	
	g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
	g_object_unref (section);		
	section = g_menu_new (); 
	g_menu_append (section, "About", "app.about"); 		
	g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
	g_object_unref (section);	
	section = g_menu_new ();
	g_menu_append (section, "Quit", "app.quit");
	g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
	g_object_unref (section);
	
	//menu button
	menu_button = gtk_menu_button_new();
	gtk_widget_set_tooltip_text(menu_button, "Menu");
	gtk_menu_button_set_icon_name (GTK_MENU_BUTTON (menu_button),"open-menu-symbolic"); 		
	gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (menu_button), G_MENU_MODEL(menu));
	gtk_header_bar_pack_end(GTK_HEADER_BAR (header), menu_button);
			
	g_object_set_data(G_OBJECT(window), "window-text-entry-key",entry_text); 
	
	gtk_window_present(GTK_WINDOW (window));    //use present not show with gtk4	
}
//======================================================================
int main (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.diphonetalker", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
