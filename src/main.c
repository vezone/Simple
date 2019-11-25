#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 600

typedef int32_t int32;
typedef int64_t int64;

struct FileInfo;

typedef struct FileInfo {
    char* filepath;
    char* filename;
    int tab_number;
    struct FileInfo* next;
} FileInfo;
FileInfo* g_file_data = NULL;

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
    GtkWidget* notebook;
    simple_menu_bar* menu_bar;
    GtkWidget* vertical_box;
    GtkWidget* window;
} simple;

//func pointers
char* file_read(const char* path);
int32 vstring_length(const char* string);
void file_write(const char* path, char* data);
const char* file_get_name(filepath);

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

static FileInfo* 
file_info_get(int page_number)
{
    FileInfo* ptr = g_file_data;
    for (; ptr != NULL ;)
    {
        if (ptr->tab_number == page_number)
        {
            return ptr;
        }
        else
        {
            ptr = ptr->next;
        }
    }
    return NULL;
}

static void
file_info_add(int page_number, const char* filepath)
{
    g_print("in func\n");
    if (g_file_data == NULL)
    {
        g_print("g_file_data == NULL\n");
        g_file_data = malloc(sizeof(FileInfo));
        g_file_data->filepath = filepath;
        g_file_data->filename = file_get_name(filepath);
        g_file_data->tab_number = page_number;
        g_file_data->next = NULL;
    }
    else 
    {
        FileInfo* ptr = g_file_data;
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = malloc(sizeof(FileInfo));
        ptr->next->filepath = filepath;
        ptr->next->filename = file_get_name(filepath);
        ptr->next->tab_number = page_number;
        ptr->next->next = NULL;
    }
}

//work in progress
static void
file_info_remove(int32 tab_number)
{
    if (g_file_data != NULL)
    {
        FileInfo* ptr = g_file_data;
        while (ptr->next != NULL)
        {
            if (ptr->tab_number == tab_number)
            {
                FileInfo* temp = ptr->next;
                if (ptr) { free(ptr); }
                ptr = temp;
            }
            else if (ptr->next != NULL && ptr->next->tab_number == tab_number)
            {

            }
            ptr = ptr->next;
        }
    }
}

simple* app;

const char* g_main_directory_path = "/home/bies/Documents/programming/c/gtkTextEditor";
const char* constant_for_editor = "    ";

char* file_read(const char* path)
{
    long file_length;
    char* file_content;
    FILE* file;
    file = fopen(path, "rb");
    if (file != NULL)
    {
        fseek(file, 0, 2);
        file_length = ftell(file);
        fseek(file, 0, 0);
        file_content = malloc((file_length + 1) * sizeof(char));
        fread(file_content, sizeof(char), file_length, file);
        file_content[file_length] = '\0';
        fclose(file);
        g_print("Read file successfully!\n");
    }
    else
    {
        file_content = 0;
        g_print("Can't open a file!\n");
    }
    return file_content;
}

int32 vstring_length(const char* string)
{
    int32 length;
    for (length = 0; string[length] != '\0'; length++);
    return length;
}

int32 vstring_compare(const char* left, const char* right)
{
    int32 left_length = vstring_length(left);
    int32 right_length = vstring_length(right);

    if ((left_length == 0) || (right_length == 0))
    {
        return 0;
    }
    else if (left_length != right_length)
    {
        return 0;
    }
    else 
    {
        for (right_length = 0; right_length > left_length; right_length++)
        {
            if (left[right_length] != right[right_length])
            {
                return 0;
            }
        }
    }

    return 1;
}

void file_write(const char* path, char* data)
{
    g_print("call: file_write\n");
    FILE* file;
    file = fopen(path, "wb");
    if (file != NULL)
    {
        if (data != NULL)
        {
            int32 data_length = vstring_length(data);
            fwrite(data, sizeof(char), data_length, file);
            g_print("Write to file successfully!\n");
        }
        fclose(file);
    }
    else
    {
        g_print("Can't open a file!\n");
    }
}

const char* file_get_name(const char* filepath)
{
    int32 filepath_length = vstring_length(filepath);
    int32 last_address = 0;

    for (int32 i = 0; i < filepath_length; i++)
    {
        char el = filepath[i];

        if (el == '/')
        {
            last_address = i + 1;
        }
    }

    int32 new_length = filepath_length - last_address;
    char* filename = malloc((new_length + 1) * sizeof(char));
    for (int32 i = last_address; i < filepath_length; i++)
    {
        filename[i - last_address] = filepath[i];
    }
    filename[new_length] = '\0';
    g_print("filename = %s\n", filename);

    return filename;
}

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

    app->vertical_box = gtk_box_new(TRUE, 10);
    gtk_widget_show(app->vertical_box);
    gtk_box_pack_start(GTK_BOX(app->vertical_box), app->menu_bar->menu, FALSE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(app->vertical_box), app->notebook, FALSE, TRUE, 0);

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
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), WINDOW_HEIGHT);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook),
        scrolled_window, horizontal_bar);    
}

static gboolean
on_text_view_pressed_event(GtkWidget* text_view, GdkEventKey* event, gpointer data)
{
    //GTK_WIDGET_CLASS(text_view)->button_press_event(text_view, event);
    if (event->type == GDK_KEY_PRESS)
    {
        switch (event->keyval)
        {
            case GDK_KEY_s:
            case GDK_KEY_S: {
                if (event->state & GDK_CONTROL_MASK)
                {
                    g_print("Ctrl + S\n");
                }
                break;
            }

            case GDK_KEY_d:
            case GDK_KEY_D: {
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
                //GtkTextBuffer* text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_editor_text_view));
                //gtk_text_buffer_insert_at_cursor(text_buffer, constant_for_editor, 4);
            }

            case GDK_KEY_Tab: {
                //GtkTextBuffer* text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_editor_text_view));
                //gtk_text_buffer_insert_at_cursor(text_buffer, constant_for_editor, 4);
                break;
            }
        }
    }

    return FALSE;
}

static void
menu_item_new_callback()
{
    g_print("New clicked!\n");
    simple_notebook_add_page(app, "Untitled");
    gtk_widget_show_all(app->window);
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
            
        const char* filename = file_get_name(filepath);
        simple_notebook_add_page(app, filename);

        char* editor_text = file_read(filepath);
        g_print("file content: %s\n", editor_text);

        int pages_number = gtk_notebook_get_n_pages(GTK_NOTEBOOK(app->notebook))-1;
        g_print("gtk_notebook_get_n_pages: %d\n", pages_number);
        
        file_info_add(pages_number, filepath);
        GtkWidget* page = gtk_notebook_get_nth_page(
            GTK_NOTEBOOK(app->notebook), pages_number);
        GtkWidget* textview = notebook_page_get_textview(page);
        if (textview != NULL)
        {
            gtk_text_buffer_set_text(
                gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)),
                editor_text, -1);   
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
        g_print("For some reason page is Container\n");
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

int main(int argc, char** argv)
{
    g_print("It works\n");
    gtk_init(&argc, &argv);
    
    app = simple_new();
    //simple_notebook_add_page(app, "Untitled");
    
    //g_print("res = %d\n", res);

    gtk_widget_show_all(app->window);
	gtk_main();

    return 0;
}