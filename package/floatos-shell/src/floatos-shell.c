#include <gtk/gtk.h>
#include <glib-unix.h>
#include <signal.h>
#include <time.h>

typedef struct {
    GtkWidget *panel;
    GtkWidget *dock;
    GtkWidget *launcher_popover;
    GtkWidget *quick_popover;
    GtkWidget *clock_label;
    GtkWidget *network_label;
    GtkWidget *launcher_search;
} FloatUI;

typedef struct {
    const gchar *name;
    const gchar *icon;
    const gchar *command;
    gboolean settings;
    GtkWidget *button;
} AppItem;

static FloatUI ui;

static AppItem apps[] = {
    {"Files", "system-file-manager", "pcmanfm", FALSE, NULL},
    {"Web Browser", "web-browser", "dillo", FALSE, NULL},
    {"Terminal", "utilities-terminal", "xterm", FALSE, NULL},
    {"Settings", "preferences-system", NULL, TRUE, NULL},
    {"Network", "network-wireless", "xterm -e connmanctl", FALSE, NULL},
    {"Sound", "audio-volume-high", "xterm -e alsamixer", FALSE, NULL},
    {"About FloatOS", "help-about", "xterm -e cat /usr/share/floatos/README.txt", FALSE, NULL}
};

static void launch_command(const gchar *command)
{
    GError *error = NULL;
    if (!command || !*command) return;
    if (!g_spawn_command_line_async(command, &error)) {
        g_warning("Could not launch '%s': %s", command, error->message);
        g_clear_error(&error);
    }
}

static GtkWidget *make_icon_button(const gchar *icon, gint size, const gchar *tip)
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *image = gtk_image_new_from_icon_name(icon, GTK_ICON_SIZE_BUTTON);
    gtk_image_set_pixel_size(GTK_IMAGE(image), size);
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    gtk_button_set_focus_on_click(GTK_BUTTON(button), FALSE);
    gtk_widget_set_tooltip_text(button, tip);
    gtk_container_add(GTK_CONTAINER(button), image);
    return button;
}

static GtkWidget *make_action_button(const gchar *icon, const gchar *title,
                                     const gchar *subtitle, const gchar *command)
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    GtkWidget *image = gtk_image_new_from_icon_name(icon, GTK_ICON_SIZE_DIALOG);
    GtkWidget *copy = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    GtkWidget *heading = gtk_label_new(title);
    GtkWidget *detail = gtk_label_new(subtitle);
    gtk_widget_set_halign(heading, GTK_ALIGN_START);
    gtk_widget_set_halign(detail, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(heading), "action-title");
    gtk_style_context_add_class(gtk_widget_get_style_context(detail), "action-detail");
    gtk_box_pack_start(GTK_BOX(copy), heading, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(copy), detail, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row), image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row), copy, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(button), row);
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    gtk_widget_set_halign(button, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(button, TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(button), "setting-action");
    if (command) g_signal_connect_swapped(button, "clicked", G_CALLBACK(launch_command), (gpointer)command);
    return button;
}

static GtkWidget *make_settings_page(const gchar *title, const gchar *description,
                                     const gchar *icon, const gchar *action_title,
                                     const gchar *action_detail, const gchar *command)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    GtkWidget *hero = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    GtkWidget *image = gtk_image_new_from_icon_name(icon, GTK_ICON_SIZE_DIALOG);
    GtkWidget *titles = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *heading = gtk_label_new(title);
    GtkWidget *detail = gtk_label_new(description);
    GtkWidget *card = make_action_button(icon, action_title, action_detail, command);

    gtk_widget_set_halign(heading, GTK_ALIGN_START);
    gtk_widget_set_halign(detail, GTK_ALIGN_START);
    gtk_label_set_line_wrap(GTK_LABEL(detail), TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(heading), "page-title");
    gtk_style_context_add_class(gtk_widget_get_style_context(detail), "page-description");
    gtk_box_pack_start(GTK_BOX(titles), heading, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(titles), detail, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hero), image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hero), titles, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(page), hero, FALSE, FALSE, 8);
    gtk_box_pack_start(GTK_BOX(page), card, FALSE, FALSE, 0);
    gtk_widget_set_margin_start(page, 24);
    gtk_widget_set_margin_end(page, 24);
    gtk_widget_set_margin_top(page, 22);
    gtk_widget_set_margin_bottom(page, 22);
    return page;
}

static void switch_settings_page(GtkButton *button, gpointer data)
{
    GtkStack *stack = GTK_STACK(data);
    const gchar *name = g_object_get_data(G_OBJECT(button), "page-name");
    if (name) gtk_stack_set_visible_child_name(stack, name);
}

static void open_settings(void)
{
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *header = gtk_header_bar_new();
    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 7);
    GtkWidget *stack = gtk_stack_new();
    GtkWidget *buttons[4];
    const gchar *names[] = {"display", "network", "sound", "about"};
    const gchar *labels[] = {"Display", "Network", "Sound", "About"};
    const gchar *icons[] = {"video-display", "network-wireless", "audio-volume-high", "help-about"};

    gtk_window_set_title(GTK_WINDOW(window), "FloatOS Settings");
    gtk_window_set_default_size(GTK_WINDOW(window), 820, 540);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(window), header);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "Settings");
    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(header), "Make FloatOS feel like yours");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_widget_set_name(window, "settings-window");

    gtk_style_context_add_class(gtk_widget_get_style_context(outer), "settings-root");
    gtk_style_context_add_class(gtk_widget_get_style_context(sidebar), "settings-sidebar");
    gtk_style_context_add_class(gtk_widget_get_style_context(stack), "settings-stack");
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 240);

    for (gint i = 0; i < 4; ++i) {
        buttons[i] = gtk_button_new_with_label(labels[i]);
        GtkWidget *image = gtk_image_new_from_icon_name(icons[i], GTK_ICON_SIZE_BUTTON);
        gtk_button_set_image(GTK_BUTTON(buttons[i]), image);
        gtk_button_set_always_show_image(GTK_BUTTON(buttons[i]), TRUE);
        gtk_button_set_relief(GTK_BUTTON(buttons[i]), GTK_RELIEF_NONE);
        gtk_widget_set_halign(buttons[i], GTK_ALIGN_FILL);
        gtk_widget_set_size_request(buttons[i], 168, 42);
        gtk_style_context_add_class(gtk_widget_get_style_context(buttons[i]), "settings-nav");
        g_object_set_data(G_OBJECT(buttons[i]), "page-name", (gpointer)names[i]);
        g_signal_connect(buttons[i], "clicked", G_CALLBACK(switch_settings_page), stack);
        gtk_box_pack_start(GTK_BOX(sidebar), buttons[i], FALSE, FALSE, 0);
    }

    gtk_stack_add_named(GTK_STACK(stack),
        make_settings_page("Display & Appearance",
            "Tune the screen resolution and desktop look. The FloatOS palette is charcoal, white and lime.",
            "video-display", "Display tools",
            "View available screen modes and adjust the display with XRandR.",
            "xterm -e xrandr"),
        "display");
    gtk_stack_add_named(GTK_STACK(stack),
        make_settings_page("Network",
            "Manage wired and wireless connections through the lightweight ConnMan service.",
            "network-wireless", "Open network controls",
            "Open the ConnMan console to scan, connect and inspect Wi-Fi services.",
            "xterm -e connmanctl"),
        "network");
    gtk_stack_add_named(GTK_STACK(stack),
        make_settings_page("Sound",
            "Control the audio mixer and hardware volume levels.",
            "audio-volume-high", "Open sound mixer",
            "Launch ALSA mixer controls for the detected audio device.",
            "xterm -e alsamixer"),
        "sound");
    gtk_stack_add_named(GTK_STACK(stack),
        make_settings_page("About FloatOS",
            "An independent x86-64 Linux system assembled from upstream source with Buildroot.",
            "floatos-symbolic", "System information",
            "View the FloatOS system information and live-session notes.",
            "xterm -e cat /usr/share/floatos/README.txt"),
        "about");

    gtk_widget_set_margin_start(sidebar, 18);
    gtk_widget_set_margin_end(sidebar, 14);
    gtk_widget_set_margin_top(sidebar, 22);
    gtk_widget_set_margin_bottom(sidebar, 22);
    gtk_widget_set_margin_top(stack, 0);
    gtk_widget_set_hexpand(stack, TRUE);
    gtk_widget_set_vexpand(stack, TRUE);
    gtk_box_pack_start(GTK_BOX(body), sidebar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(body), stack, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(outer), body, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), outer);
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "display");
    gtk_widget_show_all(window);
    gtk_window_present(GTK_WINDOW(window));
}

static void app_clicked(GtkButton *button, gpointer data)
{
    AppItem *app = (AppItem *)data;
    (void)button;
    if (app->settings) open_settings();
    else launch_command(app->command);
    if (ui.launcher_popover) gtk_popover_popdown(GTK_POPOVER(ui.launcher_popover));
}

static void filter_apps(GtkSearchEntry *entry, gpointer data)
{
    (void)data;
    const gchar *raw = gtk_entry_get_text(GTK_ENTRY(entry));
    gchar *query = g_utf8_strdown(raw, -1);
    for (guint i = 0; i < G_N_ELEMENTS(apps); ++i) {
        gchar *name = g_utf8_strdown(apps[i].name, -1);
        gtk_widget_set_visible(apps[i].button,
            !query[0] || g_strstr_len(name, -1, query) != NULL);
        g_free(name);
    }
    g_free(query);
}

static void toggle_launcher(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    if (!ui.launcher_popover) return;
    if (gtk_widget_get_visible(ui.launcher_popover))
        gtk_popover_popdown(GTK_POPOVER(ui.launcher_popover));
    else {
        gtk_widget_show_all(ui.launcher_popover);
        gtk_popover_popup(GTK_POPOVER(ui.launcher_popover));
        if (ui.launcher_search) gtk_widget_grab_focus(ui.launcher_search);
    }
}

static gboolean handle_usr1(gpointer data)
{
    (void)data;
    toggle_launcher(NULL, NULL);
    return G_SOURCE_CONTINUE;
}

static void toggle_quick_settings(GtkWidget *widget, gpointer data)
{
    (void)widget; (void)data;
    if (!ui.quick_popover) return;
    if (gtk_widget_get_visible(ui.quick_popover))
        gtk_popover_popdown(GTK_POPOVER(ui.quick_popover));
    else {
        gtk_widget_show_all(ui.quick_popover);
        gtk_popover_popup(GTK_POPOVER(ui.quick_popover));
    }
}

static GtkWidget *build_launcher(GtkWidget *anchor)
{
    GtkWidget *popover = gtk_popover_new(anchor);
    GtkWidget *reveal = gtk_revealer_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    GtkWidget *heading_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *mark = gtk_label_new("F");
    GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    GtkWidget *title = gtk_label_new("FloatOS");
    GtkWidget *subtitle = gtk_label_new("Your space. Your workflow.");
    GtkWidget *search = gtk_search_entry_new();
    GtkWidget *grid = gtk_grid_new();

    ui.launcher_search = search;
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    gtk_popover_set_modal(GTK_POPOVER(popover), FALSE);
    gtk_style_context_add_class(gtk_widget_get_style_context(popover), "launcher-popover");
    gtk_style_context_add_class(gtk_widget_get_style_context(box), "launcher-content");
    gtk_style_context_add_class(gtk_widget_get_style_context(mark), "brand-mark-large");
    gtk_style_context_add_class(gtk_widget_get_style_context(title), "launcher-title");
    gtk_style_context_add_class(gtk_widget_get_style_context(subtitle), "launcher-subtitle");
    gtk_style_context_add_class(gtk_widget_get_style_context(search), "launcher-search");
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_widget_set_halign(subtitle, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(title_box), title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(title_box), subtitle, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(heading_row), mark, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(heading_row), title_box, TRUE, TRUE, 0);

    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    for (guint i = 0; i < G_N_ELEMENTS(apps); ++i) {
        GtkWidget *button = gtk_button_new();
        GtkWidget *tile = gtk_box_new(GTK_ORIENTATION_VERTICAL, 7);
        GtkWidget *image = gtk_image_new_from_icon_name(apps[i].icon, GTK_ICON_SIZE_DIALOG);
        GtkWidget *label = gtk_label_new(apps[i].name);
        gtk_image_set_pixel_size(GTK_IMAGE(image), 30);
        gtk_style_context_add_class(gtk_widget_get_style_context(button), "app-tile");
        gtk_style_context_add_class(gtk_widget_get_style_context(label), "app-tile-label");
        gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
        gtk_box_pack_start(GTK_BOX(tile), image, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(tile), label, FALSE, FALSE, 0);
        gtk_container_add(GTK_CONTAINER(button), tile);
        gtk_widget_set_size_request(button, 112, 88);
        g_signal_connect(button, "clicked", G_CALLBACK(app_clicked), &apps[i]);
        gtk_grid_attach(GTK_GRID(grid), button, i % 4, i / 4, 1, 1);
        apps[i].button = button;
    }
    g_signal_connect(search, "search-changed", G_CALLBACK(filter_apps), NULL);
    gtk_widget_set_margin_start(box, 18);
    gtk_widget_set_margin_end(box, 18);
    gtk_widget_set_margin_top(box, 18);
    gtk_widget_set_margin_bottom(box, 18);
    gtk_box_pack_start(GTK_BOX(box), heading_row, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), search, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), grid, FALSE, FALSE, 0);
    gtk_revealer_set_transition_type(GTK_REVEALER(reveal), GTK_REVEALER_TRANSITION_TYPE_SLIDE_DOWN);
    gtk_revealer_set_transition_duration(GTK_REVEALER(reveal), 220);
    gtk_container_add(GTK_CONTAINER(reveal), box);
    gtk_container_add(GTK_CONTAINER(popover), reveal);
    gtk_widget_show_all(popover);
    gtk_revealer_set_reveal_child(GTK_REVEALER(reveal), TRUE);
    gtk_popover_popdown(GTK_POPOVER(popover));
    return popover;
}

static void power_dialog_response(GtkDialog *dialog, gint response, gpointer data)
{
    (void)data;
    if (response == 10) launch_command("reboot");
    if (response == 11) launch_command("poweroff");
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void show_power_dialog(GtkButton *button, gpointer data)
{
    (void)button; (void)data;
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Power options");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
        "Choose what FloatOS should do with this live session.");
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Restart", 10);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Shut Down", 11);
    g_signal_connect(dialog, "response", G_CALLBACK(power_dialog_response), NULL);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    gtk_widget_show(dialog);
}

static GtkWidget *build_quick_settings(GtkWidget *anchor)
{
    GtkWidget *popover = gtk_popover_new(anchor);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *title = gtk_label_new("Quick settings");
    GtkWidget *grid = gtk_grid_new();
    const gchar *icons[] = {"network-wireless", "audio-volume-high", "video-display", "preferences-system"};
    const gchar *labels[] = {"Wi-Fi & Network", "Sound", "Display", "All Settings"};
    const gchar *commands[] = {
        "xterm -e connmanctl", "xterm -e alsamixer", "xterm -e xrandr", NULL
    };
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    gtk_popover_set_modal(GTK_POPOVER(popover), FALSE);
    gtk_style_context_add_class(gtk_widget_get_style_context(popover), "quick-popover");
    gtk_style_context_add_class(gtk_widget_get_style_context(title), "quick-title");
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    for (gint i = 0; i < 4; ++i) {
        GtkWidget *button = gtk_button_new();
        GtkWidget *tile = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        GtkWidget *image = gtk_image_new_from_icon_name(icons[i], GTK_ICON_SIZE_BUTTON);
        GtkWidget *label = gtk_label_new(labels[i]);
        gtk_style_context_add_class(gtk_widget_get_style_context(button), "quick-tile");
        gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
        gtk_box_pack_start(GTK_BOX(tile), image, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(tile), label, FALSE, FALSE, 0);
        gtk_container_add(GTK_CONTAINER(button), tile);
        gtk_widget_set_size_request(button, 178, 52);
        if (i == 3) g_signal_connect_swapped(button, "clicked", G_CALLBACK(open_settings), NULL);
        else g_signal_connect_swapped(button, "clicked", G_CALLBACK(launch_command), (gpointer)commands[i]);
        gtk_grid_attach(GTK_GRID(grid), button, i % 2, i / 2, 1, 1);
    }
    GtkWidget *power = make_action_button("system-shutdown", "Power options",
        "Shut down or restart this live session", NULL);
    g_signal_connect(power, "clicked", G_CALLBACK(show_power_dialog), NULL);
    gtk_box_pack_start(GTK_BOX(box), title, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(box), grid, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), power, FALSE, FALSE, 0);
    gtk_widget_set_margin_start(box, 15);
    gtk_widget_set_margin_end(box, 15);
    gtk_widget_set_margin_top(box, 15);
    gtk_widget_set_margin_bottom(box, 15);
    gtk_container_add(GTK_CONTAINER(popover), box);
    gtk_widget_show_all(popover);
    gtk_popover_popdown(GTK_POPOVER(popover));
    return popover;
}

static gboolean refresh_status(gpointer data)
{
    static guint status_tick = 0;
    (void)data;
    GDateTime *now = g_date_time_new_now_local();
    gchar *clock = g_date_time_format(now, "%a  %d %b     %H:%M");
    gtk_label_set_text(GTK_LABEL(ui.clock_label), clock);
    g_free(clock);
    g_date_time_unref(now);

    if ((status_tick++ % 10) == 0) {
        gchar *output = NULL;
        if (g_spawn_command_line_sync("connmanctl state", &output, NULL, NULL, NULL) &&
            output && (g_strrstr(output, "online") || g_strrstr(output, "ready")))
            gtk_label_set_text(GTK_LABEL(ui.network_label), "Online");
        else
            gtk_label_set_text(GTK_LABEL(ui.network_label), "Offline");
        g_free(output);
    }
    return G_SOURCE_CONTINUE;
}

static void create_panel(void)
{
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *brand = gtk_button_new();
    GtkWidget *brand_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 9);
    GtkWidget *mark = gtk_label_new("F");
    GtkWidget *name = gtk_label_new("FloatOS");
    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *status = gtk_button_new();
    GtkWidget *status_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *network_icon = gtk_image_new_from_icon_name("network-wireless-symbolic", GTK_ICON_SIZE_BUTTON);

    ui.panel = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_type_hint(GTK_WINDOW(ui.panel), GDK_WINDOW_TYPE_HINT_DOCK);
    gtk_window_set_decorated(GTK_WINDOW(ui.panel), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(ui.panel), TRUE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(ui.panel), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(ui.panel), TRUE);
    gtk_window_stick(GTK_WINDOW(ui.panel));
    gtk_window_set_default_size(GTK_WINDOW(ui.panel), gdk_screen_get_width(gdk_screen_get_default()), 46);
    gtk_style_context_add_class(gtk_widget_get_style_context(ui.panel), "top-panel");
    gtk_style_context_add_class(gtk_widget_get_style_context(mark), "brand-mark");
    gtk_style_context_add_class(gtk_widget_get_style_context(name), "brand-name");
    gtk_button_set_relief(GTK_BUTTON(brand), GTK_RELIEF_NONE);
    gtk_style_context_add_class(gtk_widget_get_style_context(brand), "brand-button");
    gtk_box_pack_start(GTK_BOX(brand_row), mark, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(brand_row), name, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(brand), brand_row);
    g_signal_connect(brand, "clicked", G_CALLBACK(toggle_launcher), NULL);

    ui.network_label = gtk_label_new("Checking");
    ui.clock_label = gtk_label_new("--:--");
    gtk_style_context_add_class(gtk_widget_get_style_context(status), "status-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(ui.network_label), "status-text");
    gtk_style_context_add_class(gtk_widget_get_style_context(ui.clock_label), "clock-text");
    gtk_button_set_relief(GTK_BUTTON(status), GTK_RELIEF_NONE);
    gtk_box_pack_start(GTK_BOX(status_row), network_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(status_row), ui.network_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(status_row), ui.clock_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(status), status_row);
    g_signal_connect(status, "clicked", G_CALLBACK(toggle_quick_settings), NULL);

    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_box_pack_start(GTK_BOX(row), brand, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row), spacer, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(row), status, FALSE, FALSE, 0);
    gtk_widget_set_margin_start(row, 16);
    gtk_widget_set_margin_end(row, 16);
    gtk_widget_set_margin_top(row, 4);
    gtk_widget_set_margin_bottom(row, 4);
    gtk_container_add(GTK_CONTAINER(ui.panel), row);

    ui.launcher_popover = build_launcher(brand);
    ui.quick_popover = build_quick_settings(status);
}

typedef struct { const gchar *icon; const gchar *tip; const gchar *command; gboolean settings; gboolean launcher; } DockItem;

static void dock_clicked(GtkButton *button, gpointer data)
{
    DockItem *item = (DockItem *)data;
    (void)button;
    if (item->launcher) toggle_launcher(NULL, NULL);
    else if (item->settings) open_settings();
    else launch_command(item->command);
}

static void create_dock(void)
{
    static DockItem items[] = {
        {"view-app-grid-symbolic", "Applications", NULL, FALSE, TRUE},
        {"system-file-manager", "Files", "pcmanfm", FALSE, FALSE},
        {"web-browser", "Web Browser", "dillo", FALSE, FALSE},
        {"utilities-terminal", "Terminal", "xterm", FALSE, FALSE},
        {"preferences-system", "Settings", NULL, TRUE, FALSE}
    };
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    ui.dock = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_type_hint(GTK_WINDOW(ui.dock), GDK_WINDOW_TYPE_HINT_DOCK);
    gtk_window_set_decorated(GTK_WINDOW(ui.dock), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(ui.dock), TRUE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(ui.dock), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(ui.dock), TRUE);
    gtk_window_stick(GTK_WINDOW(ui.dock));
    gtk_window_set_default_size(GTK_WINDOW(ui.dock), 430, 78);
    gtk_style_context_add_class(gtk_widget_get_style_context(ui.dock), "floating-dock");
    for (guint i = 0; i < G_N_ELEMENTS(items); ++i) {
        GtkWidget *button = make_icon_button(items[i].icon, 30, items[i].tip);
        gtk_style_context_add_class(gtk_widget_get_style_context(button), "dock-button");
        g_signal_connect(button, "clicked", G_CALLBACK(dock_clicked), &items[i]);
        gtk_box_pack_start(GTK_BOX(row), button, FALSE, FALSE, 0);
    }
    gtk_widget_set_margin_start(row, 15);
    gtk_widget_set_margin_end(row, 15);
    gtk_widget_set_margin_top(row, 9);
    gtk_widget_set_margin_bottom(row, 9);
    gtk_container_add(GTK_CONTAINER(ui.dock), row);
}

static void load_theme(void)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;
    if (gtk_css_provider_load_from_path(provider, "/usr/share/floatos/theme.css", &error)) {
        gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
            GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    } else {
        g_warning("Could not load FloatOS theme: %s", error->message);
        g_clear_error(&error);
    }
    g_object_unref(provider);
}

int main(int argc, char **argv)
{
    if (argc > 1 && g_strcmp0(argv[1], "--toggle-launcher") == 0) {
        launch_command("killall -s USR1 floatos-shell");
        return 0;
    }

    gtk_init(&argc, &argv);
    g_set_application_name("FloatOS");
    load_theme();
    create_panel();
    create_dock();
    gtk_widget_show_all(ui.panel);
    gtk_widget_show_all(ui.dock);

    gint screen_width = gdk_screen_get_width(gdk_screen_get_default());
    gint screen_height = gdk_screen_get_height(gdk_screen_get_default());
    gtk_window_move(GTK_WINDOW(ui.panel), 0, 0);
    gtk_window_move(GTK_WINDOW(ui.dock), (screen_width - 430) / 2, screen_height - 100);
    refresh_status(NULL);
    g_timeout_add_seconds(1, refresh_status, NULL);
    g_unix_signal_add(SIGUSR1, handle_usr1, NULL);
    gtk_main();
    return 0;
}