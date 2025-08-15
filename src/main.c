#include "gtk4-layer-shell.h"
#include "ipc.h"
#include "listener.h"
#include "logger.h"
#include "ui_utils.h"
#include <gtk/gtk.h>
#include <msgpack.h>
#include <msgpack/sbuffer.h>
#include <msgpack/unpack.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static FancySocket *fs = NULL;
void handle_sigint(int sig) {
  (void)sig;
  if (fs) {
    fancy_socket_destroy(fs);
    printf("\nFancy exited like a boss.\n");
    fs = NULL;
  }
  _exit(0);
}

void *listen_thread(void *arg) {
  FancySocket *fs = (FancySocket *)arg;
  int status = fancy_socket_listener(fs);
  return NULL;
}

const char *ui =
    "<interface>"
    "  <object class=\"GtkWindow\" id=\"notification_window\">"
    "    <property name=\"default_width\">500</property>"
    "    <property name=\"default_height\">200</property>"
    "    <child>"
    "      <object class=\"GtkBox\" id=\"notif_box\">"
    "        <property name=\"orientation\">vertical</property>"
    "        <child>"
    "          <object class=\"GtkLabel\" id=\"notif_label\">"
    "            <property name=\"label\">You called me?</property>"
    "          </object>"
    "        </child>"
    "        <child>"
    "          <object class=\"GtkButton\" id=\"notif_button\">"
    "            <property name=\"label\">Click Me</property>"
    "            <signal name=\"clicked\" handler=\"on_button_clicked\"/>"
    "          </object>"
    "        </child>"
    "      </object>"
    "    </child>"
    "  </object>"
    "  <object class=\"GtkWindow\" id=\"test\">"
    "    <property name=\"default_width\">500</property>"
    "    <property name=\"default_height\">300</property>"
    "    <child>"
    "      <object class=\"GtkBox\" id=\"test_box\">"
    "        <property name=\"orientation\">vertical</property>"
    "        <child>"
    "          <object class=\"GtkLabel\" id=\"testy\">"
    "            <property name=\"label\">No i didn't</property>"
    "          </object>"
    "        </child>"
    "        <child>"
    "          <object class=\"GtkButton\" id=\"test_button\">"
    "            <property name=\"label\">Me neither</property>"
    "            <signal name=\"clicked\" handler=\"on_button_clicked\"/>"
    "          </object>"
    "        </child>"
    "      </object>"
    "    </child>"
    "  </object>"
    "</interface>";

void on_button_clicked(GtkButton *btn, gpointer data) {
  g_print("test button clicked\n");
}

void connect_buttons_recursive(GtkWidget *widget) {
  if (GTK_IS_BUTTON(widget)) {
    g_signal_connect(widget, "clicked", G_CALLBACK(on_button_clicked), NULL);
  }

  GtkWidget *child = gtk_widget_get_first_child(widget);
  while (child) {
    connect_buttons_recursive(child);
    child = gtk_widget_get_next_sibling(child);
  }
}
static void activate(GtkApplication *app, void *_data) {
  (void)_data;

  GtkBuilder *builder = gtk_builder_new();
  GtkBuilderScope *cscope = gtk_builder_cscope_new();
  gtk_builder_set_scope(builder, GTK_BUILDER_SCOPE(cscope));
  gtk_builder_cscope_add_callback_symbol(GTK_BUILDER_CSCOPE(cscope),
                                         "on_button_clicked",
                                         G_CALLBACK(on_button_clicked));

  GError *error = NULL;
  if (!gtk_builder_add_from_string(builder, ui, -1, &error)) {
    g_printerr("Error loading UI: %s\n", error->message);
    g_error_free(error);
    return;
  }

  GSList *objects = gtk_builder_get_objects(builder);
  for (GSList *l = objects; l != NULL; l = l->next) {
    if (!GTK_IS_WINDOW(l->data))
      continue;
    GtkWindow *win = GTK_WINDOW(l->data);

    gtk_window_set_application(GTK_WINDOW(win), app);

    // Apply Layer Shell
    gtk_layer_init_for_window(GTK_WINDOW(win));
    gtk_layer_set_layer(GTK_WINDOW(win), GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_auto_exclusive_zone_enable(GTK_WINDOW(win));
    fancy_set_anchor_left(win);

    gtk_window_present(win);
  }
}

int main(int argc, char **argv) {
  logger_init(stdout, stderr, LOGGY_DEBUG);
  signal(SIGINT, handle_sigint);
  fs = fancy_socket_create();
  int fsos = fancy_socket_open(fs);
  if (fsos < 0) {
    fprintf(stderr, "Failed to open fancy socket error code: %i", fsos);
    exit(1);
  }
  pthread_t thread;
  if (pthread_create(&thread, NULL, listen_thread, fs) != 0) {
    perror("pthread_create");
    exit(1);
  };
  pthread_join(thread, NULL);
  //
  // GtkApplication *app = gtk_application_new("com.github.qulxizer.fancy",
  //                                           G_APPLICATION_DEFAULT_FLAGS);
  // g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  // int status = g_application_run(G_APPLICATION(app), argc, argv);
  // g_object_unref(app);
  // return status;
  // char buf[1024];
  // FancyEvent fe;
  // fe.event_name = "test";
  // fe.widget_id = "ez";
  // fe.event_type = EVENT_CLICKED;
  // size_t n = fancy_encode_event(&buf, 1024, &fe);
  //
  // printf("%zu\n", n);
  //

  return 0;
}
