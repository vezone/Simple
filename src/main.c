#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//delete this in future

typedef int32_t int32;

GtkWidget *g_window;
GtkWidget *g_editor_text_view;
char* g_editor_text = NULL;
GtkWidget* g_file_chooser;
GtkWidget* g_file_chooser_save;
GtkFileFilter* g_file_chooser_filter;
GtkFileChooserAction g_file_chooser_action_open = GTK_FILE_CHOOSER_ACTION_OPEN;
GtkFileChooserAction g_file_chooser_action_save = GTK_FILE_CHOOSER_ACTION_SAVE;

GtkWidget* menu_bar;
GtkWidget* menu_item_file;
GtkWidget* file_menu;
GtkWidget* menu_item_new;
GtkWidget* menu_item_open;
GtkWidget* menu_item_save;
GtkWidget* menu_item_save_as;

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

int32 vstring_length(char* string)
{
    int32 length;
    for (length = 0; string[length] != '\0'; length++);
    return length;
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

static void
window_destroy(GtkWindow* window, gpointer user_data)
{
    g_print("Destroying window !\n");
    gtk_main_quit();
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
                    g_print("Ctrl + D\n");
                    GtkTextBuffer* text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_editor_text_view));
                    gtk_text_buffer_insert_at_cursor(text_buffer, constant_for_editor, 4);
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
menu_bar_callback(GtkWidget* widget, gpointer data)
{
    //rework it with pointer compare
    if ((int64_t*)widget == (int64_t*)menu_item_new)
    {
        g_print("New clicked!\n");
    }
    else if ((int64_t*)widget == (int64_t*)menu_item_open)
    {
        g_print("Open clicked!\n");
        if (g_editor_text != NULL)
        {
            free(g_editor_text);
        }
        
        g_file_chooser = gtk_file_chooser_dialog_new(
            "Open File", GTK_WINDOW(g_window), g_file_chooser_action_open, 
            "Cancel", GTK_RESPONSE_CANCEL,
            "Open", GTK_RESPONSE_ACCEPT,
            NULL);

        int result = gtk_dialog_run(GTK_DIALOG(g_file_chooser));
        if (result == GTK_RESPONSE_ACCEPT)
        {
            GtkFileChooser* chooser = GTK_FILE_CHOOSER(g_file_chooser);
            
            char* filename = 
            gtk_file_chooser_get_filename(chooser);
            if (filename == NULL)
            {
                filename = "/home/bies/Documents/programming/c/gtkTextEditor/default.c";    
            }
            
            g_editor_text = file_read(filename);
            g_print("file content: %s\n", g_editor_text);

            gtk_text_buffer_set_text(
                gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_editor_text_view)),
                g_editor_text, -1);   

            g_free(filename);   
        }
        
        gtk_widget_destroy(g_file_chooser);  
        
    }
    else if ((int64_t*)widget == (int64_t*)menu_item_save)
    {
        g_print("call: btn_file_save_clicked\n");
        char* filename; 
        char* text;
        GtkTextBuffer* text_buffer; 
        GtkTextIter start;
        GtkTextIter end;

        filename = gtk_file_chooser_get_filename(
                GTK_FILE_CHOOSER(g_file_chooser_save));
        text_buffer = gtk_text_view_get_buffer(
                GTK_TEXT_VIEW(g_editor_text_view));
        gtk_text_buffer_get_bounds(text_buffer, &start, &end);

        text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);
        
        g_print("text: %s\n", text);
        file_write(filename, text);
        
        g_free(filename);
    }
    else if ((int64_t*)widget == (int64_t*)menu_item_save_as)
    {
        g_print("Save as clicked!\n");
        char* text;
        int result; 
        //dialog need to be set every time
        g_file_chooser_save = gtk_file_chooser_dialog_new(
            "Save", GTK_WINDOW(g_window), g_file_chooser_action_save, 
            "Cancel", GTK_RESPONSE_CANCEL,
            "Save", GTK_RESPONSE_ACCEPT,
            NULL);

        result = gtk_dialog_run(GTK_DIALOG(g_file_chooser_save));
        if (result == GTK_RESPONSE_ACCEPT)
        {
            char* text;
            char* filename;
            GtkFileChooser* chooser;
            GtkTextBuffer* buffer;
            GtkTextIter start;
            GtkTextIter end;
        
            chooser = GTK_FILE_CHOOSER(g_file_chooser_save);
            filename = gtk_file_chooser_get_filename(chooser);
            buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_editor_text_view));
            
            gtk_text_buffer_get_bounds(buffer, &start, &end);

            if (filename == NULL)
            {
                filename = "/home/bies/Documents/programming/c/gtkTextEditor/default.c";    
            }
            
            text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
            
            g_print("text: %s\n", text);
            file_write(filename, text);

            g_free(filename);
        }
        gtk_widget_destroy(g_file_chooser_save);
    }
}

int main(int argc, char** argv)
{
    g_print("It works\n");

    gtk_init(&argc, &argv);
    
    g_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(g_window), "Simple");
    gtk_window_set_default_size(GTK_WINDOW(g_window), 400, 400);
    gtk_widget_show(g_window);
    g_signal_connect(g_window, "destroy", G_CALLBACK (window_destroy), NULL);
    
    menu_bar = gtk_menu_bar_new();

    file_menu = gtk_menu_new();
    menu_item_file = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item_file), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item_file);

    menu_item_new = gtk_menu_item_new_with_label("New");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item_new);
    g_signal_connect(menu_item_new, "activate", G_CALLBACK(menu_bar_callback), NULL);
    
    menu_item_open = gtk_menu_item_new_with_label("Open");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item_open);
    g_signal_connect(menu_item_open, "activate", G_CALLBACK(menu_bar_callback), NULL);

    menu_item_save = gtk_menu_item_new_with_label("Save");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item_save);
    g_signal_connect(menu_item_save, "activate", G_CALLBACK(menu_bar_callback), NULL);

    menu_item_save_as = gtk_menu_item_new_with_label("Save as");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item_save_as);
    g_signal_connect(menu_item_save_as, "activate", G_CALLBACK(menu_bar_callback), NULL);

    //
    g_file_chooser_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(g_file_chooser_filter, "All Files");
    gtk_file_filter_add_pattern(g_file_chooser_filter, "*");
    //gtk_file_chooser_set_current_folder(
    //    GTK_FILE_CHOOSER(g_chooser), g_main_directory_path);
    //gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(g_chooser), g_file_chooser_filter);

    g_editor_text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(g_editor_text_view), TRUE);
    gtk_text_buffer_set_text(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_editor_text_view)),
        g_editor_text, -1);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(g_editor_text_view), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(g_editor_text_view), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(g_editor_text_view), 10);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(g_editor_text_view), 10);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(g_editor_text_view), TRUE);
    gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(g_editor_text_view), TRUE);
    g_signal_connect(GTK_TEXT_VIEW(g_editor_text_view), "key-press-event",
        G_CALLBACK(on_text_view_pressed_event), NULL);

    GtkWidget *scrolled_window;
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scrolled_window), 1000);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), 600);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), g_editor_text_view);

    GtkWidget *vertical_box;
    vertical_box = gtk_box_new(TRUE, 10);
    gtk_box_set_child_packing(GTK_BOX(vertical_box), g_editor_text_view, 0, 1, 0, GTK_PACK_START);
    gtk_widget_show(vertical_box);
    gtk_box_pack_start(GTK_BOX(vertical_box), menu_bar, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vertical_box), scrolled_window, FALSE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(g_window), vertical_box);

    gtk_widget_show_all(g_window);
	gtk_main();

    //i am not sure, should we free memory from
    gtk_widget_destroy(g_file_chooser);

    return 0;
}