#include <gtk/gtk.h>

#include <stdint.h>

typedef int32_t int32;

static void
window_destroy(GtkWindow* window, gpointer user_data)
{
    g_print("Destroying window !\n");
    gtk_main_quit();
}

static void
btn_show_text_clicked(GtkWidget* btn, gpointer data)
{
    //do something
}

int main(int argc, char** argv)
{

    printf("It works\n");

    gtk_init(&argc, &argv);

    GtkWidget *window;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simple");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);
    gtk_widget_show(window);
    g_signal_connect (window, "destroy", G_CALLBACK (window_destroy), NULL);

    GtkWidget* btn_show_text;
    btn_show_text = gtk_button_new_with_label("f button");
    g_signal_connect(btn_show_text, "clicked", G_CALLBACK(btn_show_text_clicked), 0);

    GtkWidget* text_view;
    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), TRUE);
    char* some_text = "#include <vlib.h>\nint main()\n{\n}\n";
    gtk_text_buffer_set_text(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view)),
        some_text, -1
    );

    GtkWidget *vertical_box;
    vertical_box = gtk_box_new(TRUE, 10);
    gtk_widget_show(vertical_box);
    gtk_box_pack_start(GTK_BOX(vertical_box), btn_show_text, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vertical_box), text_view, FALSE, TRUE, 0);


    gtk_container_add(GTK_CONTAINER(window), vertical_box);

    gtk_widget_show_all(window);
	gtk_main ();

    return 0;
}