/* Copyright 2014 the unarr project authors (see AUTHORS file).
   License: GPLv3 */

#include "unarr.h"

#define FailIf(cond, msg, ...) if (cond) { fprintf(stderr, msg "\n", __VA_ARGS__); goto CleanUp; } step++

int main(int argc, char *argv[])
{
    ar_stream *stream = NULL;
    ar_archive *ar = NULL;
    int count = 1;
    int step = 1;

    FailIf(argc < 2, "Syntax: %s <filename.rar>", argv[0]);

    stream = ar_open_file(argv[1]);
    FailIf(!stream, "Error: File \"%s\" not found!", argv[1]);

    ar = ar_open_rar_archive(stream);
    FailIf(!ar, "Error: File \"%s\" is no valid RAR archive!", argv[1]);

    printf("Parsing \"%s\":\n", argv[1]);
    while (ar_parse_entry(ar)) {
        size_t size = ar_entry_get_size(ar);
        printf("%02d. %s (@%Iu)\n", count++, ar_entry_get_name(ar), ar_entry_get_offset(ar));
        while (size > 0) {
            unsigned char buffer[1024];
            if (!ar_entry_uncompress(ar, buffer, min(size, sizeof(buffer))))
                break;
            size -= min(size, sizeof(buffer));
        }
    }
    FailIf(!ar_at_eof(ar), "Error: Failed to parse entry %d!", count);
    step = 0;

CleanUp:
    if (ar)
        ar_close_archive(ar);
    if (stream)
        ar_close(stream);
    return step;
}
