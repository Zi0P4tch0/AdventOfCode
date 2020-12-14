#include <glib.h>

static void combinations_inner(const gint *array,
                               gint *tmp,
                               guint start,
                               guint end,
                               guint index,
                               guint k,
                               void (*on_found)(const gint*, guint),
                               guint n_run) {
    if (index == k) {
        on_found(tmp, n_run);
        return;
    }
    for (guint i=start; i<=end && end-i+1 >= k-index; i++){
        tmp[index] = array[i];
        combinations_inner(array, tmp, i+1, end, index+1, k, on_found, n_run);
    }
}

void combinations(const gint *array, guint size, guint k, void (*on_found)(const gint*, guint), guint n_run) {
    int tmp[k];
    combinations_inner(array, tmp, 0, size-1, 0, k, on_found, n_run);
}
