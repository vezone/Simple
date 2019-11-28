#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

//TODO:
// * [check] работают ли new, open, save, save_as функции
// * [write] remove func for FileInfo
// * [rename] FileInfo to file_info
// * [write] func for hz_bar_close_btn
// * [write] save_as logic for "Untitled.c"
// * [create] flexible tab (tab=4, tab=8)
// * [create] editor->setting
// * [create] color theme (cairo, gdk)
// * [create] autocomplete
// * [create] status bar
// * [create] label in hz_bar for file with changes
// * [maybe create] buttons for new, open, save, save_as
// * [create] shortcuts for new, open, save, save_as
// * [remove] all warnings
// * [create] status bar
// * [create] arrow  logic 
// * [create] arrow + shift selection logic


//opt
// * [free] get_iter

//textview -> sourceview

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 600

typedef struct simple_menu_bar 
{
    GtkWidget* menu;
    
    GtkWidget* file_menu;
    GtkWidget* menu_item_file;
    GtkWidget* menu_item_new;
    GtkWidget* menu_item_open;
    GtkWidget* menu_item_save;
    GtkWidget* menu_item_save_as;

    GtkWidget* editor_menu;
    GtkWidget* menu_item_edit;
    GtkWidget* menu_item_settings;
} simple_menu_bar;

typedef struct simple_file 
{
    char* filename;
    char* path;
    int tab_number;
} simple_file;

typedef struct simple 
{
    simple_menu_bar* menu_bar;
    GtkWidget* notebook;
    GtkWidget* statusbar;
    GtkWidget* vertical_box;
    GtkWidget* window;
} simple;

//func pointers
static void window_destroy(GtkWindow* window, gpointer user_data);
static simple* simple_new();
static simple_menu_bar* simple_menu_bar_new();
static GtkWidget* simple_notebook_new();
static void simple_notebook_add_page(simple *app, const char* page_name);
static gboolean on_text_view_pressed_event(GtkWidget* text_view, GdkEventKey* event, gpointer data);
static void menu_bar_callback(GtkWidget* widget, gpointer data);
static void notebook_callback(GtkNotebook* notebook, GtkWidget*page, guint page_num, gpointer data);
static void notebook_tab_close_button_callback(GtkWidget* button, gpointer data);
static GtkWidget* notebook_page_get_textview(GtkWidget* page);
static GtkTextView* notebook_get_current_textview(GtkWidget* notebook);
static GtkTextIter* textview_get_cursor_iter(GtkTextView* textview);
static void statusbar_set_info(GtkWidget* statusbar, GtkTextView* textview, GtkTextBuffer* textbuffer);

simple* app;

const char* g_main_directory_path = "/home/bies/Documents/programming/c/gtkTextEditor";
const char* constant_for_editor = "    ";
char buffer_insert_text[1];


static void
window_destroy(GtkWindow* window, gpointer user_data)
{
    g_print("Destroying window !\n");
    gtk_main_quit();
}

static simple*
simple_new()
{
    simple* app = malloc(sizeof(simple));
    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    gtk_window_set_title(GTK_WINDOW(app->window), "Simple");
    gtk_window_set_default_size(GTK_WINDOW(app->window), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_gravity(GTK_WINDOW(app->window), GDK_GRAVITY_CENTER);
    g_signal_connect(app->window, "destroy", G_CALLBACK(window_destroy), NULL);
    
    app->menu_bar = simple_menu_bar_new();
    app->notebook = simple_notebook_new();

    app->statusbar = gtk_statusbar_new();
    guint context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(app->statusbar), "status");
    const char* status_string = g_strdup_printf("line %d, column %d", 0, 0);
    gtk_statusbar_push(GTK_STATUSBAR(app->statusbar), context_id, status_string);
    gtk_widget_show(app->statusbar);

    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox), app->statusbar, FALSE, FALSE, 0);

    app->vertical_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(app->vertical_box), app->menu_bar->menu, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(app->vertical_box), app->notebook, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(app->vertical_box), /*app->statusbar*/hbox, FALSE, FALSE, 0);
    gtk_widget_show(app->vertical_box);

    gtk_container_add(GTK_CONTAINER(app->window), app->vertical_box);

    return app;
}

static simple_menu_bar*
simple_menu_bar_new()
{
    simple_menu_bar* simple_bar = malloc(sizeof(simple_menu_bar));

    simple_bar->menu = gtk_menu_bar_new();
    simple_bar->file_menu = gtk_menu_new();
    simple_bar->editor_menu = gtk_menu_new();

    simple_bar->menu_item_file = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(simple_bar->menu_item_file), simple_bar->file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(simple_bar->menu), simple_bar->menu_item_file);

    simple_bar->menu_item_new = gtk_menu_item_new_with_label("New");
    gtk_menu_shell_append(GTK_MENU_SHELL(simple_bar->file_menu), simple_bar->menu_item_new);
    g_signal_connect(simple_bar->menu_item_new, "activate", G_CALLBACK(menu_bar_callback), NULL);
    
    simple_bar->menu_item_open = gtk_menu_item_new_with_label("Open");
    gtk_menu_shell_append(GTK_MENU_SHELL(simple_bar->file_menu), simple_bar->menu_item_open);
    g_signal_connect(simple_bar->menu_item_open, "activate", G_CALLBACK(menu_bar_callback), NULL);

    simple_bar->menu_item_save = gtk_menu_item_new_with_label("Save");
    gtk_menu_shell_append(GTK_MENU_SHELL(simple_bar->file_menu), simple_bar->menu_item_save);
    g_signal_connect(simple_bar->menu_item_save, "activate", G_CALLBACK(menu_bar_callback), NULL);

    simple_bar->menu_item_save_as = gtk_menu_item_new_with_label("Save as");
    gtk_menu_shell_append(GTK_MENU_SHELL(simple_bar->file_menu), simple_bar->menu_item_save_as);
    g_signal_connect(simple_bar->menu_item_save_as, "activate", G_CALLBACK(menu_bar_callback), NULL);

    simple_bar->menu_item_edit = gtk_menu_item_new_with_label("Editor");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(simple_bar->menu_item_edit), simple_bar->editor_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(simple_bar->menu), simple_bar->menu_item_edit);

    simple_bar->menu_item_settings = gtk_menu_item_new_with_label("Settings");
    gtk_menu_shell_append(GTK_MENU_SHELL(simple_bar->editor_menu), simple_bar->menu_item_settings);
    g_signal_connect(simple_bar->menu_item_settings, "activate", G_CALLBACK(menu_bar_callback), NULL);

    return simple_bar;
}

static GtkWidget*
simple_notebook_new()
{
    GtkWidget* notebook = gtk_notebook_new();
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
    //gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
    g_signal_connect(notebook, "switch-page", G_CALLBACK(notebook_callback), NULL);
    return notebook;
}

static void
simple_notebook_add_page(simple *app, const char* page_name)
{
    int* page_number = malloc(sizeof(int));
    GtkWidget *horizontal_bar, *tab_close_button, *tab_label;
    
    horizontal_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    tab_close_button = gtk_button_new_with_label("Close");
    *page_number = gtk_notebook_get_n_pages(GTK_NOTEBOOK(app->notebook));
    g_signal_connect(tab_close_button, "clicked", 
        G_CALLBACK(notebook_tab_close_button_callback), (void*)page_number);
    tab_label = gtk_label_new(page_name);
    gtk_widget_show(GTK_WIDGET(tab_close_button));
    gtk_widget_show(GTK_WIDGET(tab_label));
    gtk_widget_show(GTK_WIDGET(horizontal_bar));
    gtk_box_pack_start(GTK_BOX(horizontal_bar), tab_label, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(horizontal_bar), tab_close_button, FALSE, FALSE, 0);

    GtkWidget* text_view;
    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), TRUE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);
    gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(text_view), TRUE);
    g_signal_connect(GTK_TEXT_VIEW(text_view), "key-press-event",
        G_CALLBACK(on_text_view_pressed_event), NULL);
    gtk_text_buffer_set_text(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view)),
        "", -1);   

    GtkWidget *scrolled_window;
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scrolled_window), WINDOW_WIDTH);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), WINDOW_HEIGHT-300);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_widget_show(scrolled_window);

    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook),
        scrolled_window, horizontal_bar);    
}

static gboolean
on_text_view_pressed_event(GtkWidget* text_view, GdkEventKey* event, gpointer data)
{
    //GTK_WIDGET_CLASS(text_view)->button_press_event(text_view, event);
    GtkTextView* textview = NULL;
    GtkTextBuffer* textbuffer = NULL;

    if (event->type == GDK_KEY_PRESS)
    {
        //printting characters
        switch (event->keyval)
        {
            case 33 ... 125:
            {
                if (!(event->state & GDK_CONTROL_MASK))
                {
                    buffer_insert_text[0] = event->keyval;
                    GtkTextView* textview = notebook_get_current_textview(app->notebook);
                    GtkTextBuffer* text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                    gtk_text_buffer_insert_at_cursor(text_buffer, buffer_insert_text, 1);
                }
                break;
            }
        }

        switch (event->keyval)
        {
            case GDK_KEY_s:
            case GDK_KEY_S: {
                if (event->state & GDK_CONTROL_MASK)
                {
                    g_print("Ctrl + S\n");
                    //save logic;
                }
                break;
            }

            case GDK_KEY_d: 
            case GDK_KEY_D: 
            {
                if (event->state & GDK_CONTROL_MASK)
                {
                    //g_print("Ctrl + D\n");
                    //GtkTextBuffer* text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_editor_text_view));
                    //gtk_text_buffer_insert_at_cursor(text_buffer, constant_for_editor, 4);
                }
                break;
            }

            case GDK_KEY_Return:
            {
                textview = notebook_get_current_textview(app->notebook);
                textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                gtk_text_buffer_insert_at_cursor(textbuffer, "\n", 1);
                break;
            }

            //case GDK_KEY_KP_Space:
            case GDK_KEY_space:
            {
                GtkTextView* textview = notebook_get_current_textview(app->notebook);
                GtkTextBuffer* text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                gtk_text_buffer_insert_at_cursor(text_buffer, " ", 1);
                break;
            }

            case GDK_KEY_BackSpace:
            {
                //delete single character
                GtkTextView* textview = notebook_get_current_textview(app->notebook);
                GtkTextBuffer* textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                
                GtkTextIter* iter = textview_get_cursor_iter(textview);
                gtk_text_buffer_backspace(textbuffer, iter, FALSE, TRUE);

                break;
            }

            case GDK_KEY_Delete:
            {
                //delete single character
                GtkTextView* textview = notebook_get_current_textview(app->notebook);
                GtkTextBuffer* textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                
                GtkTextIter* iter = textview_get_cursor_iter(textview);
                GtkTextIter* end = gtk_text_iter_copy(iter);
                gtk_text_iter_forward_cursor_positions(end, 1);
                gtk_text_buffer_delete(textbuffer, iter, end);
                
                gtk_text_iter_free(end);
                break;
            }

            case GDK_KEY_Tab:
            {
                textview = notebook_get_current_textview(app->notebook);
                textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                gtk_text_buffer_insert_at_cursor(textbuffer, constant_for_editor, 4);
                break;
            }

            case GDK_KEY_Up:
            {
                g_print("up arrow\n");
                textview = notebook_get_current_textview(app->notebook);
                textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                
                GtkTextIter* iter = textview_get_cursor_iter(textview);
                gtk_text_iter_backward_line(iter);
                gtk_text_buffer_place_cursor(textbuffer, iter);

                break;
            }

            case GDK_KEY_Down:
            {
                g_print("down arrow\n");
                GtkTextView* textview = notebook_get_current_textview(app->notebook);
                GtkTextBuffer* textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                GtkTextIter* iter = textview_get_cursor_iter(textview);
                gtk_text_iter_forward_line(iter);
                gtk_text_buffer_place_cursor(textbuffer, iter);
                
                break;
            }

            case GDK_KEY_Left:
            {
                textview = notebook_get_current_textview(app->notebook);
                textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                GtkTextIter* iter = textview_get_cursor_iter(textview);

                if (event->state & GDK_KEY_Shift_L)
                {
                    gtk_text_iter_backward_word_start(iter);
                }
                else 
                {
                    gtk_text_iter_backward_cursor_positions(iter, 1);
                }
                gtk_text_buffer_place_cursor(textbuffer, iter);

                break;
            }

            case GDK_KEY_Right:
            {
                textview = notebook_get_current_textview(app->notebook);
                textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                GtkTextIter* iter = textview_get_cursor_iter(textview);
                
                if (event->state & GDK_KEY_Shift_L)
                {
                    gtk_text_iter_forward_word_end(iter);
                }
                else 
                {
                    gtk_text_iter_forward_cursor_positions(iter, 1);
                }
                gtk_text_buffer_place_cursor(textbuffer, iter);
                
                break;
            }

            default:{

                break;
            }
        }
    }


    if (textview == NULL || textbuffer == NULL)
    {
        textview = notebook_get_current_textview(app->notebook);
        textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    }
    
    statusbar_set_info(app->statusbar, textview, textbuffer);

    return TRUE;
}

static void
menu_item_new_callback()
{
    g_print("New clicked!\n");
    simple_notebook_add_page(app, "Untitled");
    int pages_number = gtk_notebook_get_n_pages(GTK_NOTEBOOK(app->notebook))-1;
    gtk_widget_show_all(app->window);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(app->notebook), pages_number);
}

static void 
menu_item_open_callback()
{
    g_print("Open clicked!\n");

    GtkFileFilter* file_chooser_filter;
    GtkWidget* open_file_chooser;
    
    file_chooser_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(file_chooser_filter, "All Files");
    gtk_file_filter_add_pattern(file_chooser_filter, "*");
    open_file_chooser = gtk_file_chooser_dialog_new(
        "Open File", GTK_WINDOW(app->window), GTK_FILE_CHOOSER_ACTION_OPEN, 
        "Cancel", GTK_RESPONSE_CANCEL,
        "Open", GTK_RESPONSE_ACCEPT,
        NULL);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(open_file_chooser), file_chooser_filter);

    int result = gtk_dialog_run(GTK_DIALOG(open_file_chooser));
    if (result == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(open_file_chooser);
        
        const char* filepath = 
        gtk_file_chooser_get_filename(chooser);
        if (filepath == NULL)
        {
            //TODO: remove this in future
            filepath = "/home/bies/Documents/programming/c/gtkTextEditor/default.c";    
        }
        g_print("filepath: %s\n", filepath);
            
        char* filename = file_get_name(filepath);
        simple_notebook_add_page(app, filename);

        char* editor_text = file_read(filepath);
        g_print("file content: %s\n", editor_text);

        int pages_number = gtk_notebook_get_n_pages(GTK_NOTEBOOK(app->notebook))-1;
        g_print("gtk_notebook_get_n_pages: %d\n", pages_number);
        
        file_info_add(pages_number, filepath);
        GtkWidget* page = gtk_notebook_get_nth_page(
            GTK_NOTEBOOK(app->notebook), pages_number);
        GtkWidget* textview = notebook_page_get_textview(page);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(app->notebook), pages_number);
        if (textview != NULL)
        {
            GtkTextBuffer* textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
            gtk_text_buffer_set_text(textbuffer, editor_text, -1);  
            statusbar_set_info(app->statusbar, GTK_TEXT_VIEW(textview), textbuffer); 
        }
        else 
        {
            g_print("textview == NULL\n");
        }

    }

    gtk_widget_show_all(app->window);    
    gtk_widget_destroy(open_file_chooser); 
}

static void
menu_item_save_callback()
{
    g_print("call: btn_file_save_clicked\n");
    char* filename; 
    char* text;
    GtkTextBuffer* textbuffer; 
    GtkTextIter start;
    GtkTextIter end;
    int32 page_number = gtk_notebook_get_current_page(GTK_NOTEBOOK(app->notebook));
    if (page_number != -1)
    {
        FileInfo* file_info = file_info_get(page_number);
        GtkWidget* page = gtk_notebook_get_nth_page(
                    GTK_NOTEBOOK(app->notebook), page_number);
        GtkTextView* textview = notebook_get_current_textview(app->notebook);
        
        if (textview)
        {
            textbuffer = gtk_text_view_get_buffer(textview);
            gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(textbuffer), &start, &end);
            if (textbuffer)
            {
                text = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
                g_print("text: %s\n", text);
                file_write(file_info->filepath, text);
            }
        }
    }
}

static void
menu_item_save_as_callback()
{
    g_print("Save as clicked!\n");
    char* text;
    int result; 
    //dialog need to be set every time
    GtkWidget* file_chooser_save;
    file_chooser_save = gtk_file_chooser_dialog_new(
        "Save", GTK_WINDOW(app->window), GTK_FILE_CHOOSER_ACTION_SAVE, 
        "Cancel", GTK_RESPONSE_CANCEL,
        "Save", GTK_RESPONSE_ACCEPT,
        NULL);
    result = gtk_dialog_run(GTK_DIALOG(file_chooser_save));
    if (result == GTK_RESPONSE_ACCEPT)
    {
        char* text;
        char* filename;
        GtkFileChooser* chooser;
        GtkTextView* textview;
        GtkTextBuffer* buffer;
        GtkTextIter start;
        GtkTextIter end;
    
        chooser = GTK_FILE_CHOOSER(file_chooser_save);
        filename = gtk_file_chooser_get_filename(chooser);
        textview = notebook_get_current_textview(app->notebook);
        buffer = gtk_text_view_get_buffer(textview);
        
        if (filename == NULL)
        {
            filename = "/home/bies/Documents/programming/c/gtkTextEditor/default.c";    
        }
        
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    
        g_print("text: %s\n", text);
        file_write(filename, text);
        g_free(filename);
    }
    gtk_widget_destroy(file_chooser_save);
}

static void
menu_bar_callback(GtkWidget* widget, gpointer data)
{
    //rework it with pointer compare
    if ((int64_t)widget == (int64_t)app->menu_bar->menu_item_new)
    {
        menu_item_new_callback();
    }
    else if ((int64_t)widget == (int64_t)app->menu_bar->menu_item_open)
    {
         menu_item_open_callback();
    }
    else if ((int64_t)widget == (int64_t)app->menu_bar->menu_item_save)
    {
        menu_item_save_callback();
    }
    else if ((int64_t)widget == (int64_t)app->menu_bar->menu_item_save_as)
    {
        menu_item_save_as_callback();
    }
    else if ((int64_t)widget == (int64_t)app->menu_bar->menu_item_settings)
    {
        g_print("Editor -> Settings\n");
    }
}

static void
notebook_callback(GtkNotebook* notebook, GtkWidget*page, guint page_num, gpointer data)
{
    int page_number = gtk_notebook_page_num(notebook, page);
    g_print("page num: %d\n", page_number);
}

static void 
notebook_tab_close_button_callback(GtkWidget* button, gpointer data)
{
    int* notebook_page_number = 
        //gtk_notebook_get_current_page(GTK_NOTEBOOK(g_notebook_editor));
        ((int*)data);
    g_print("page num: %d\n", *notebook_page_number);
}

static GtkWidget* 
notebook_page_get_textview(GtkWidget* page)
{
    if (GTK_IS_CONTAINER(page))
    {
        GList*children = gtk_container_get_children(GTK_CONTAINER(page));
        for (GList* iter = children; iter != NULL; iter = iter->next)
        {
            gpointer data = iter->data;
            GtkWidget* child = GTK_WIDGET(data);
            const char* child_name = gtk_widget_get_name(child);
            if (vstring_compare(child_name, "GtkTextView"))
            {
                return child;
            }
            g_print("child_name: %s\n", child_name);
        }

    }
    else if (GTK_IS_BIN(page))
    {
        //page == scrolled_window, scrolled_window == BIN
        g_print("page is bin [all correct]\n");
        GtkWidget* child = gtk_bin_get_child(GTK_BIN(page));
        const char* child_name = gtk_widget_get_name(child);
        g_print("child_name: %s\n", child_name);
    }
    else
    {
        g_print("default [error]\n");
    }

    return NULL;
}

static GtkTextView* 
notebook_get_current_textview(GtkWidget* notebook)
{
    int32 page_number = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    GtkWidget* page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page_number);
    GtkTextView* textview = GTK_TEXT_VIEW(notebook_page_get_textview(page));
    return textview;        
}

static GtkTextIter* 
textview_get_cursor_iter(GtkTextView* textview)
{
    GtkTextBuffer* textbuffer;
    GtkTextIter iter;
    GtkTextMark* mark;

    textbuffer = gtk_text_view_get_buffer(textview);
    mark = gtk_text_buffer_get_insert(textbuffer);
    gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, mark);

    return gtk_text_iter_copy(&iter);
}

static void
statusbar_set_info(GtkWidget* statusbar, GtkTextView* textview, GtkTextBuffer* textbuffer)
{
    int row, col;
    GtkTextIter iter;
    GtkTextMark* mark;

    mark = gtk_text_buffer_get_insert(textbuffer);
    gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, mark);
    row = gtk_text_iter_get_line(&iter) + 1;
    col = gtk_text_iter_get_line_offset(&iter) + 1;

    guint context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "status");
    const char* status_string = g_strdup_printf("line %d, column %d", row, col);
    gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id, status_string);
}

int main(int argc, char** argv)
{
    g_print("It works\n");
    gtk_init(&argc, &argv);
    
    app = simple_new();

    gtk_widget_show_all(app->window);
	gtk_main();

    file_info_free();

    return 0;
}