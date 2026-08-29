#include "cra_assert.h"
#include "cra_filedir.h"

typedef struct
{
    char  full[256];
    char *path;
    char *file;
} Data;

void
test_dirname_basename(void)
{
    char *path, *file;
    char  path1[256], path2[256];
    Data  data[] = {
        { "/path/to", "/path", "to" },
        { "/path/", "/", "path" },
        { "/path", "/", "path" },
        { "path/", ".", "path" },
#ifdef CRA_OS_WIN
        { "\\path\\to", "\\path", "to" },
        { "\\path\\", "\\", "path" },
        { "\\path", "\\", "path" },
        { "path\\", ".", "path" },
#else
        { "\\path\\to", ".", "\\path\\to" },
        { "\\path\\", ".", "\\path\\" },
        { "\\path", ".", "\\path" },
        { "path\\", ".", "path\\" },
#endif
        { "path", ".", "path" },
        { "..", ".", ".." },
        { ".", ".", "." },
        { "...", ".", "..." },
        { "....", ".", "...." },
        { "/path//to//", "/path", "to" },
        { "/path///to///", "/path", "to" },
        { "/path////to////", "/path", "to" },
#ifdef CRA_OS_WIN
        { "\\path\\\\to\\\\", "\\path", "to" },
        { "\\path\\\\\\to\\\\\\", "\\path", "to" },
        { "\\path\\\\\\\\to\\\\\\\\", "\\path", "to" },
#else
        { "\\path\\\\to\\\\", ".", "\\path\\\\to\\\\" },
        { "\\path\\\\\\to\\\\\\", ".", "\\path\\\\\\to\\\\\\" },
        { "\\path\\\\\\\\to\\\\\\\\", ".", "\\path\\\\\\\\to\\\\\\\\" },
#endif
        { "/path/.to/file.txt", "/path/.to", "file.txt" },
        { "/path/.to/.file.txt", "/path/.to", ".file.txt" },
#ifdef CRA_OS_WIN
        { "C:\\path\\.to\\file.txt", "C:\\path\\.to", "file.txt" },
        { "C:\\path\\.to\\.file.txt", "C:\\path\\.to", ".file.txt" },
#else
        { "C:\\path\\.to\\file.txt", ".", "C:\\path\\.to\\file.txt" },
        { "C:\\path\\.to\\.file.txt", ".", "C:\\path\\.to\\.file.txt" },
#endif
    };

    printf("%-24s%-18s%-18s\n", "path", "dirname", "basename");
    path = cra_dirname("");
    file = cra_basename("");
    assert_always(strcmp(".", path) == 0);
    assert_always(strcmp(".", file) == 0);
    printf("%-24s%-18s%-18s\n", "(EMPTY)", path, file);
    path = cra_dirname(NULL);
    file = cra_basename(NULL);
    assert_always(strcmp(".", path) == 0);
    assert_always(strcmp(".", file) == 0);
    printf("%-24s%-18s%-18s\n", "(NULL)", path, file);
    for (int i = 0; i < (int)CRA_NARRAY(data); i++)
    {
        memcpy(path1, data[i].full, sizeof(path1));
        memcpy(path2, data[i].full, sizeof(path2));
        path = cra_dirname(path1);
        file = cra_basename(path2);
        assert_always(strcmp(data[i].path, path) == 0);
        assert_always(strcmp(data[i].file, file) == 0);
        printf("%-24s%-18s%-18s\n", data[i].full, path, file);
    }
}

int
main(void)
{
    test_dirname_basename();
    return 0;
}
