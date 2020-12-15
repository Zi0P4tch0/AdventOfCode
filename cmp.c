#include <glib.h>

gint cmp_guint(gconstpointer a, gconstpointer b) {
    guint uint_a = *((guint *) a);
    guint uint_b = *((guint *) b);

    if (uint_a < uint_b)
        return -1;
    else if (uint_a > uint_b)
        return 1;
    else
        return 0;
}
