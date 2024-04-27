
#include "email.h"
#include <ctype.h>
#include <string.h>

u32 find_email(FILE * in) {
    const u8 * em1 = "._%+-:", * em2 = ".-_:";
    u32 pos = ftell(in);
    u32 user_len = 0, domain_len = 0;
    for (int c; (c = fgetc(in)) != EOF;) {
        if (user_len > 64) goto bail;
        if (isalnum(c) || strchr(em1, c)) {
            user_len++; continue;
        } else if (c == '@') break;
        else goto bail;
    }
    for (int c; (c = fgetc(in)) != EOF;) {
        if (isalnum(c) || strchr(em2, c)) { domain_len++; continue; }
        if (isspace(c) || c == EOF) break;
    }
    if(user_len && domain_len)
        return ftell(in) - pos;
    bail:;
        fseek(in, pos, SEEK_SET);
        return 0;
}

u32 find_url(FILE * in) {
    u32 pos = ftell(in);
    // short path: check for localhost
    u8 * localhost = "localhost", buf[9];
    fread(buf, 1, 9, in);
    if (!memcmp(buf, localhost, 9))
        return 9;
    fseek(in, pos, SEEK_SET);
    // long path:
    u32 protocol = 0, domain = 0, domain_letters = 0, valid_domain = 0, path = 0, valid_path = 0;
    for (int c; (c = fgetc(in)) != EOF;) {
        // Odd.
        if (protocol > 8) goto bail;
        protocol++;
        if (isalpha(c)) continue;
        else if (c == ':' && fgetc(in) == '/' && fgetc(in) == '/') {
            protocol += 2; break;
        }
        protocol = 0;
        fseek(in, pos, SEEK_SET);
        break;
    }
    for (int c; (c = fgetc(in)) != EOF;) {
        if (isalpha(c)) { domain_letters++; domain++; continue; }
        if (isdigit(c) || c == '-' || c == '_' || c == '[' || c == ']') {
            domain++; continue;
        }
        if (c == '.' || c == ':' || c == '@') {
            if(c != ':') domain_letters = 0;
            valid_domain = 1; domain++; continue;
        }
        if (domain == 0 || domain_letters == 0) goto bail;
        ungetc(c, in);
        break;
    }
    if (!valid_domain) goto bail;
    const char * path_chars = "$-_.+!*'(),;/?:@=&#%|\\^~[]`";
    for (int c; (c = fgetc(in)) != EOF;) {
        if (c == '/' || c == '\\') valid_path = 1;
        if (isalnum(c) || strchr(path_chars, c)) {
            path++; continue;
        }
        break;
    }
    if (!valid_path) goto bail;
    if(domain && (protocol || path))
        return ftell(in) - pos;
    bail:;
        fseek(in, pos, SEEK_SET);
        return 0;
}

