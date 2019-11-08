#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef int32_t int32;

GtkWidget *g_window;
GtkWidget *g_editor_text_view;
GtkWidget *g_chooser;
char* g_editor_text = NULL;

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

static void
btn_file_read_clicked(GtkWidget* btn, gpointer data)
{
    //do something
    if (g_editor_text != NULL)
    {
        free(g_editor_text);
    }
    char* filename = 
        gtk_file_chooser_get_filename(
            GTK_FILE_CHOOSER(g_chooser));
    g_editor_text = file_read(filename);
    g_print("file content: %s\n", g_editor_text);

     gtk_text_buffer_set_text(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_editor_text_view)),
        g_editor_text, -1);
}

static void 
btn_file_save_clicked(GtkWidget* btn, gpointer data)
{
    g_print("call: btn_file_save_clicked\n");
    char* filename = 
        gtk_file_chooser_get_filename(
            GTK_FILE_CHOOSER(g_chooser));
    GtkTextBuffer* text_buffer = 
        gtk_text_view_get_buffer(
            GTK_TEXT_VIEW(g_editor_text_view));
    GtkTextIter start;
    GtkTextIter end;
    gtk_text_buffer_get_bounds(text_buffer, &start, &end);

    char* text = 
        gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);
    
    g_print("text: %s\n", text);
    file_write(filename, text);
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
                    btn_file_save_clicked(NULL,NULL);
                }
                break;
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

int main(int argc, char** argv)
{
    g_print("It works\n");

    gtk_init(&argc, &argv);
    
    g_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(g_window), "Simple");
    gtk_window_set_default_size(GTK_WINDOW(g_window), 400, 400);
    gtk_widget_show(g_window);
    g_signal_connect(g_window, "destroy", G_CALLBACK (window_destroy), NULL);

    g_chooser = gtk_file_chooser_button_new(
        "choose a file to read", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_file_chooser_set_current_folder(
        GTK_FILE_CHOOSER(g_chooser), 
        "/home/bies/Documents/programming/c/gtkTextEditor");
    GtkFileFilter* filter;
    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "All Files");
    gtk_file_filter_add_pattern(filter, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(g_chooser), filter);

    GtkWidget* btn_file_read;
    btn_file_read = gtk_button_new_with_label("read file");
    g_signal_connect(btn_file_read, "clicked",
        G_CALLBACK(btn_file_read_clicked), NULL);

    GtkWidget *btn_file_save;
    btn_file_save = gtk_button_new_with_label("save");
    g_signal_connect(btn_file_save, "clicked",
        G_CALLBACK(btn_file_save_clicked), NULL);

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
    gtk_scrolled_window_set_min_content_width(scrolled_window, 400);
    gtk_scrolled_window_set_min_content_height(scrolled_window, 400);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), g_editor_text_view);

    GtkWidget *vertical_box;
    vertical_box = gtk_box_new(TRUE, 10);
    gtk_widget_show(vertical_box);
    gtk_box_pack_start(GTK_BOX(vertical_box), g_chooser, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vertical_box), btn_file_read, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vertical_box), btn_file_save, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vertical_box), scrolled_window, FALSE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(g_window), vertical_box);

    gtk_widget_show_all(g_window);
	gtk_main();

    return 0;
}