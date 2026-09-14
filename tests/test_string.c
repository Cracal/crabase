#include "cra_string.h"
#include "cra_assert.h"
#include "cra_malloc.h"

void
test_string_view(void)
{
    // equals

    CraStringView sv1 = CRA_STRING_VIEW_INIT_CONST("hello");
    assert_always(sv1.size == sizeof("hello") - 1);
    assert_always(strncmp(sv1.data, "hello", sv1.size) == 0);

    CraStringView sv2 = CRA_STRING_VIEW_INIT_CONST("hello");
    assert_always(cra_string_view_equals(&sv1, &sv2));
    assert_always(cra_string_view_equals_cstr(&sv1, "hello", sizeof("hello") - 1));

    sv1 = cra_string_view_from_cstr("word", sizeof("word") - 1);
    assert_always(sv1.size == sizeof("word") - 1);
    assert_always(strncmp(sv1.data, "word", sv1.size) == 0);

    sv2 = cra_string_view_from_cstr("abc", sizeof("abc") - 1);
    assert_always(!cra_string_view_equals(&sv1, &sv2));
    assert_always(!cra_string_view_equals_cstr(&sv1, "abc", sizeof("abc") - 1));

    // slice

    sv1 = cra_string_view_from_cstr("hello world", sizeof("hello world") - 1);
    sv2 = cra_string_view_slice_left(&sv1, 0);
    assert_always(sv2.size == 0);
    assert_always(sv1.size == sizeof("hello world") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, "hello world", sizeof("hello world") - 1));
    sv2 = cra_string_view_slice_left(&sv1, 5);
    assert_always(sv2.size == 5);
    assert_always(cra_string_view_equals_cstr(&sv2, "hello", sizeof("hello") - 1));
    assert_always(sv1.size == sizeof("hello world") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, "hello world", sizeof("hello world") - 1));
    sv2 = cra_string_view_slice_left(&sv1, 1000);
    assert_always(sv1.size == sizeof("hello world") - 1);
    assert_always(sv2.size == sizeof("hello world") - 1);
    assert_always(cra_string_view_equals(&sv1, &sv2));

    sv1 = cra_string_view_from_cstr("abcdefg", sizeof("abcdefg") - 1);
    sv2 = cra_string_view_slice_right(&sv1, 0);
    assert_always(sv2.size == 0);
    assert_always(sv1.size == sizeof("abcdefg") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, "abcdefg", sizeof("abcdefg") - 1));
    sv2 = cra_string_view_slice_right(&sv1, 3);
    assert_always(sv2.size == 3);
    assert_always(cra_string_view_equals_cstr(&sv2, "efg", sizeof("efg") - 1));
    assert_always(sv1.size == sizeof("abcdefg") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, "abcdefg", sizeof("abcdefg") - 1));
    sv2 = cra_string_view_slice_right(&sv1, 1000);
    assert_always(sv1.size == sizeof("abcdefg") - 1);
    assert_always(sv2.size == sizeof("abcdefg") - 1);
    assert_always(cra_string_view_equals(&sv1, &sv2));

    sv1 = cra_string_view_from_cstr("hello world", sizeof("hello world") - 1);
    sv2 = cra_string_view_slice(&sv1, 4, 4);
    assert_always(sv2.size == 0);
    assert_always(sv1.size == sizeof("hello world") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, "hello world", sizeof("hello world") - 1));
    sv2 = cra_string_view_slice(&sv1, 4, 8);
    assert_always(sv2.size == 4);
    assert_always(cra_string_view_equals_cstr(&sv2, "o wo", sizeof("o wo") - 1));
    assert_always(sv1.size == sizeof("hello world") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, "hello world", sizeof("hello world") - 1));
    sv2 = cra_string_view_slice(&sv1, 4, 1000);
    assert_always(sv1.size == sizeof("hello world") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, "hello world", sizeof("hello world") - 1));
    assert_always(sv2.size == sv1.size - 4);
    assert_always(cra_string_view_equals_cstr(&sv2, "o world", sizeof("o world") - 1));

    // trim

    sv1 = cra_string_view_from_cstr(" \t\n  \t\t\v\fgood morning\r\n", sizeof(" \t\n  \t\t\v\fgood morning\r\n") - 1);
    sv2 = cra_string_view_trim_left(&sv1);
    assert_always(sv2.size == sizeof("good morning\r\n") - 1);
    assert_always(cra_string_view_equals_cstr(&sv2, "good morning\r\n", sizeof("good morning\r\n") - 1));
    assert_always(sv1.size == sizeof(" \t\n  \t\t\v\fgood morning\r\n") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, " \t\n  \t\t\v\fgood morning\r\n",
                                              sizeof(" \t\n  \t\t\v\fgood morning\r\n") - 1));
    sv2 = cra_string_view_trim_right(&sv1);
    assert_always(sv2.size == sizeof(" \t\n  \t\t\v\fgood morning") - 1);
    assert_always(
      cra_string_view_equals_cstr(&sv2, " \t\n  \t\t\v\fgood morning", sizeof(" \t\n  \t\t\v\fgood morning") - 1));
    assert_always(sv1.size == sizeof(" \t\n  \t\t\v\fgood morning\r\n") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, " \t\n  \t\t\v\fgood morning\r\n",
                                              sizeof(" \t\n  \t\t\v\fgood morning\r\n") - 1));
    sv2 = cra_string_view_trim(&sv1);
    assert_always(sv2.size == sizeof("good morning") - 1);
    assert_always(cra_string_view_equals_cstr(&sv2, "good morning", sizeof("good morning") - 1));
    assert_always(sv1.size == sizeof(" \t\n  \t\t\v\fgood morning\r\n") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, " \t\n  \t\t\v\fgood morning\r\n",
                                              sizeof(" \t\n  \t\t\v\fgood morning\r\n") - 1));

    // starts with & ends with

    sv1 = cra_string_view_from_cstr("hello world", sizeof("hello world") - 1);
    assert_always(cra_string_view_starts_with_cstr(&sv1, "hello", sizeof("hello") - 1));
    assert_always(!cra_string_view_starts_with_cstr(&sv1, "world", sizeof("world") - 1));
    sv2 = cra_string_view_from_cstr("hel", sizeof("hel") - 1);
    assert_always(cra_string_view_starts_with(&sv1, &sv2));
    sv2 = cra_string_view_from_cstr("l", sizeof("l") - 1);
    assert_always(!cra_string_view_starts_with(&sv1, &sv2));
    assert_always(cra_string_view_ends_with_cstr(&sv1, "world", sizeof("world") - 1));
    assert_always(!cra_string_view_ends_with_cstr(&sv1, "hello", sizeof("hello") - 1));
    sv2 = cra_string_view_from_cstr("ld", sizeof("ld") - 1);
    assert_always(cra_string_view_ends_with(&sv1, &sv2));
    sv2 = cra_string_view_from_cstr("l", sizeof("l") - 1);
    assert_always(!cra_string_view_ends_with(&sv1, &sv2));

    // find & rfind

    size_t index = 0;
    sv1 = cra_string_view_from_cstr("YYY k kk kkl kkls kklsd  fdsllkklsdl",
                                    sizeof("YYY k kk kkl kkls kklsd  fdsllkklsdl") - 1);
    assert_always(cra_string_view_find_cstr(&sv1, "kklsd", sizeof("kklsd") - 1, &index));
    assert_always(index == 18);
    assert_always(!cra_string_view_find_cstr(&sv1, "ok", sizeof("ok") - 1, &index));
    assert_always(cra_string_view_find_cstr(&sv1, "", 0, &index));
    assert_always(index == 0);
    sv1 = cra_string_view_from_cstr("HELLO WORLD", sizeof("HELLO WORLD") - 1);
    sv2 = cra_string_view_from_cstr(" WOR", sizeof(" WOR") - 1);
    assert_always(cra_string_view_find(&sv1, &sv2, &index));
    assert_always(index == 5);
    sv2 = cra_string_view_from_cstr("NOOOOO", sizeof("NOOOOO") - 1);
    assert_always(!cra_string_view_find(&sv1, &sv2, &index));
    sv2 = cra_string_view_from_cstr("", 0);
    assert_always(cra_string_view_find(&sv1, &sv2, &index));
    assert_always(index == 0);

    sv1 = cra_string_view_from_cstr("YYY k kk kkl kkls kklsd  fdsllkklsdl",
                                    sizeof("YYY k kk kkl kkls kklsd  fdsllkklsdl") - 1);
    assert_always(cra_string_view_rfind_cstr(&sv1, "kklsd", sizeof("kklsd") - 1, &index));
    assert_always(index == 30);
    assert_always(!cra_string_view_rfind_cstr(&sv1, "ok", sizeof("ok") - 1, &index));
    assert_always(cra_string_view_rfind_cstr(&sv1, "", 0, &index));
    assert_always(index == 0);
    sv1 = cra_string_view_from_cstr("abc abcd abc bc c", sizeof("abc abcd abc bc c") - 1);
    sv2 = cra_string_view_from_cstr("abc", sizeof("abc") - 1);
    assert_always(cra_string_view_rfind(&sv1, &sv2, &index));
    assert_always(index == 9);
    sv2 = cra_string_view_from_cstr("NOOOOO", sizeof("NOOOOO") - 1);
    assert_always(!cra_string_view_rfind(&sv1, &sv2, &index));
    sv2 = cra_string_view_from_cstr("", 0);
    assert_always(cra_string_view_rfind(&sv1, &sv2, &index));
    assert_always(index == 0);

    // contains

    sv1 = cra_string_view_from_cstr("goooooooooooo^_^ooooooooooood", sizeof("goooooooooooo^_^ooooooooooood") - 1);
    assert_always(cra_string_view_contains_cstr(&sv1, "^_^", sizeof("^_^") - 1));
    assert_always(!cra_string_view_contains_cstr(&sv1, "hello", sizeof("hello") - 1));
    sv2 = cra_string_view_from_cstr("_", 1);
    assert_always(cra_string_view_contains(&sv1, &sv2));
    sv2 = cra_string_view_from_cstr("x", 1);
    assert_always(!cra_string_view_contains(&sv1, &sv2));

    // split

    sv1 = cra_string_view_from_cstr("真TM好", sizeof("真TM好") - 1);
    sv2 = cra_string_view_split_cstr(&sv1, "TM", sizeof("TM") - 1);
    assert_always(cra_string_view_equals_cstr(&sv2, "真", sizeof("真") - 1));
    assert_always(cra_string_view_equals_cstr(&sv1, "好", sizeof("好") - 1));
    sv1 = cra_string_view_from_cstr("abcdefgh", sizeof("abcdefgh") - 1);
    sv2 = cra_string_view_split_cstr(&sv1, "x", sizeof("x") - 1);
    assert_always(sv1.size == sizeof("abcdefgh") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, "abcdefgh", sizeof("abcdefgh") - 1));
    assert_always(sv2.size == 0);
    assert_always(sv2.data == NULL);
    sv2 = cra_string_view_split_cstr(&sv1, "", 0);
    assert_always(sv1.size == sizeof("abcdefgh") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, "abcdefgh", sizeof("abcdefgh") - 1));
    assert_always(sv2.size == 0);
    assert_always(sv2.data != NULL);

    CraStringView sv3 = CRA_STRING_VIEW_INIT_CONST("BBBaaaHHH");
    sv1 = cra_string_view_from_cstr("B", 1);
    sv2 = cra_string_view_split(&sv3, &sv1);
    assert_always(sv2.size == 0);
    assert_always(cra_string_view_equals_cstr(&sv2, "", 0));
    assert_always(sv3.size == sizeof("BBaaaHHH") - 1);
    assert_always(cra_string_view_equals_cstr(&sv3, "BBaaaHHH", sizeof("BBaaaHHH") - 1));

    sv1 = cra_string_view_from_cstr("hello string view!", sizeof("hello string view!") - 1);
    sv2 = cra_string_view_split_left(&sv1, 0);
    assert_always(sv1.size == sizeof("hello string view!") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, "hello string view!", sizeof("hello string view!") - 1));
    assert_always(sv2.size == 0);
    sv2 = cra_string_view_split_left(&sv1, 5);
    assert_always(sv1.size == sizeof("hello string view!") - 1 - 5);
    assert_always(cra_string_view_equals_cstr(&sv1, " string view!", sizeof(" string view!") - 1));
    assert_always(sv2.size == 5);
    assert_always(cra_string_view_equals_cstr(&sv2, "hello", sizeof("hello") - 1));
    sv2 = cra_string_view_split_left(&sv1, 100000);
    assert_always(sv1.size == 0);
    assert_always(sv2.size == sizeof(" string view!") - 1);
    assert_always(cra_string_view_equals_cstr(&sv2, " string view!", sizeof(" string view!") - 1));

    sv1 = cra_string_view_from_cstr("hello string view!", sizeof("hello string view!") - 1);
    sv2 = cra_string_view_split_right(&sv1, 0);
    assert_always(sv1.size == sizeof("hello string view!") - 1);
    assert_always(cra_string_view_equals_cstr(&sv1, "hello string view!", sizeof("hello string view!") - 1));
    assert_always(sv2.size == 0);
    sv2 = cra_string_view_split_right(&sv1, 5);
    assert_always(sv1.size == sizeof("hello string view!") - 1 - 5);
    assert_always(cra_string_view_equals_cstr(&sv1, "hello string ", sizeof("hello string ") - 1));
    assert_always(sv2.size == 5);
    assert_always(cra_string_view_equals_cstr(&sv2, "view!", sizeof("view!") - 1));
    sv2 = cra_string_view_split_right(&sv1, 100000);
    assert_always(sv1.size == 0);
    assert_always(sv2.size == sizeof("hello string ") - 1);
    assert_always(cra_string_view_equals_cstr(&sv2, "hello string ", sizeof("hello string ") - 1));

    // print string view

    sv1 = cra_string_view_from_cstr("hello string view!", sizeof("hello string view!") - 1);
    printf(CRA_SV_FMT "\n", CRA_SV_ARG(sv1));

    // utf8 len

    assert_always(cra_string_view_is_valid_utf8_cstr("hello", sizeof("hello") - 1));
    assert_always(cra_string_view_is_valid_utf8_cstr("你好", sizeof("你好") - 1));
    assert_always(cra_string_view_is_valid_utf8_cstr("¢", sizeof("¢") - 1));
    assert_always(cra_string_view_is_valid_utf8_cstr("€", sizeof("€") - 1));
    assert_always(cra_string_view_is_valid_utf8_cstr("😀", sizeof("😀") - 1));
    assert_always(cra_string_view_is_valid_utf8_cstr("🫰🏽", sizeof("🫰🏽") - 1));

    char str1[] = { 0xC0, 0x80, 0 }; // overlong encoding of U+0000
    assert_always(!cra_string_view_is_valid_utf8_cstr(str1, sizeof(str1) - 1));
    char str2[] = { 0xC1, 0xBF, 0 }; // overlong encoding
    assert_always(!cra_string_view_is_valid_utf8_cstr(str2, sizeof(str2) - 1));
    char str3[] = { 0xE0, 0x80, 0x80, 0 }; // overlong encoding
    assert_always(!cra_string_view_is_valid_utf8_cstr(str3, sizeof(str3) - 1));
    char str4[] = { 0xED, 0xA0, 0x80, 0 }; // UTF-16 surrogate U+D800
    assert_always(!cra_string_view_is_valid_utf8_cstr(str4, sizeof(str4) - 1));
    char str5[] = { 0xF0, 0x80, 0x80, 0x80, 0 }; // overlong encoding
    assert_always(!cra_string_view_is_valid_utf8_cstr(str5, sizeof(str5) - 1));
    char str6[] = { 0xF4, 0x90, 0x80, 0x80, 0 }; // > U+10FFFF
    assert_always(!cra_string_view_is_valid_utf8_cstr(str6, sizeof(str6) - 1));
    char str7[] = { 0xF5, 0x80, 0x80, 0x80, 0 }; // invalid
    assert_always(!cra_string_view_is_valid_utf8_cstr(str7, sizeof(str7) - 1));
    char str8[] = { 0x80, 0 }; // single continuation byte
    assert_always(!cra_string_view_is_valid_utf8_cstr(str8, sizeof(str8) - 1));
    char str9[] = { 0xE2, 0x82, 0 }; // cut off
    assert_always(!cra_string_view_is_valid_utf8_cstr(str9, sizeof(str9) - 1));
    char stra[] = { 0xE2, 0x28, 0xA1, 0 }; // continuation byte error
    assert_always(!cra_string_view_is_valid_utf8_cstr(stra, sizeof(stra) - 1));

    // utf8 len
    size_t bytes_overrun = 0;
    sv1 = cra_string_view_from_cstr("中文aabb笑脸", sizeof("中文aabb笑脸") - 1);
    size_t len = cra_string_view_utf8_len(&sv1, &bytes_overrun);
    assert_always(bytes_overrun == 0);
    assert_always(len == 8);
    // "🫰🏽" = "🫰" + "🏽"
    // get_utf8_str_len("🫰🏽") = 2 != 1
    sv1 = cra_string_view_from_cstr("🫰🏽", sizeof("🫰🏽") - 1);
    len = cra_string_view_utf8_len(&sv1, &bytes_overrun);
    assert_always(bytes_overrun == 0);
    assert_always(len == 2); // X

    sv1 = cra_string_view_from_cstr("中文aabb笑脸", sizeof("中文aabb笑脸") - 1);
    while (sv1.size > 0)
    {
        len = CRA_SV_BYTES_FOR_UTF8[(uint8_t)sv1.data[0]];
        sv2 = cra_string_view_split_left(&sv1, len);
        printf("%zu : "CRA_SV_FMT "\n", len, CRA_SV_ARG(sv2));
    }
}

int
main(void)
{
    test_string_view();

    cra_memory_leak_report();
    return 0;
}
