#include "gtk4-layer-shell.h"
#include <gtk/gtk.h>

void fancy_set_anchor_top(GtkWindow *win) {
  // LEFT, RIGHT, TOP, BOTTOM
  const gboolean anchors[] = {FALSE, FALSE, TRUE, FALSE};
  for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
    gtk_layer_set_anchor(win, i, anchors[i]);
  }
};
void fancy_set_anchor_bottom(GtkWindow *win) {
  // LEFT, RIGHT, TOP, BOTTOM
  const gboolean anchors[] = {FALSE, FALSE, FALSE, TRUE};
  for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
    gtk_layer_set_anchor(win, i, anchors[i]);
  }
};
void fancy_set_anchor_left(GtkWindow *win) {
  // LEFT, RIGHT, TOP, BOTTOM
  const gboolean anchors[] = {TRUE, FALSE, FALSE, FALSE};
  for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
    gtk_layer_set_anchor(win, i, anchors[i]);
  }
};
void fancy_set_anchor_right(GtkWindow *win) {
  // LEFT, RIGHT, TOP, BOTTOM
  const gboolean anchors[] = {FALSE, TRUE, FALSE, FALSE};
  for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
    gtk_layer_set_anchor(win, i, anchors[i]);
  }
};
