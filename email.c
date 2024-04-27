
#include "email.h"
#include <ctype.h>
#include <string.h>

u32 find_email(FILE * in) {
    u32 pos = ftell(in);
    u32 user_len = 0, domain_len = 0;
    for (int c; (c = fgetc(in)) != EOF;) {
        if (user_len > 64) {
            // Odd.
            fseek(in, pos, SEEK_SET);
            return 0;
        }
        if (isalnum(c) || c == '.' || c == '_' || c == '%' || c == '+' || c == '-' || c == ':') {
            user_len++;
            continue;
        } else if (c == '@') {
            break;
        } else {
            fseek(in, pos, SEEK_SET);
            return 0;
        }
    }
    for (int c; (c = fgetc(in)) != EOF;) {
        if (isalnum(c) || c == '.' || c == '-' || c == '_' || c == ':') {
            domain_len++;
            continue;
        }
        if (c == '\n' || c == ' ' || c == '\t' || c == EOF) {
            break;
        }
    }
    if(user_len && domain_len)
        return ftell(in) - pos;
    fseek(in, pos, SEEK_SET);
    return 0;
}

u32 find_url(FILE * in) {
    u32 pos = ftell(in);
    // short path: check for localhost
    u8 * localhost = "localhost";
    u8 buf[9];
    fread(buf, 1, 9, in);
    if (!memcmp(buf, localhost, 9)) {
        return 9;
    }
    fseek(in, pos, SEEK_SET);
    // long path:
    u32 protocol = 0, domain = 0, domain_letters = 0, valid_domain = 0, path = 0, valid_path = 0;
    for (int c; (c = fgetc(in)) != EOF;) {
        if (protocol > 8) {
            // Odd.
            fseek(in, pos, SEEK_SET);
            return 0;
        }
        if (isalpha(c)) {
            protocol++;
            continue;
        } else if (c == ':') {
            if (fgetc(in) == '/' && fgetc(in) == '/') {
                protocol += 3;
                break;
            }
        }
        protocol = 0;
        fseek(in, pos, SEEK_SET);
        break;
    }
    for (int c; (c = fgetc(in)) != EOF;) {
        if (isalpha(c)) {
            domain_letters++;
            domain++;
            continue;
        }
        if (isdigit(c) || c == '-' || c == '_' || c == '[' || c == ']') {
            domain++;
            continue;
        }
        if (c == '.' || c == ':' || c == '@') {
            valid_domain = 1;
            if(c != ':') domain_letters = 0;
            domain++;
            continue;
        }
        if (domain == 0 || domain_letters == 0) {
            fseek(in, pos, SEEK_SET);
            return 0;
        }
        ungetc(c, in);
        break;
    }
    if (!valid_domain) {
        fseek(in, pos, SEEK_SET);
        return 0;
    }
    const char * path_chars = "$-_.+!*'(),;/?:@=&#%|\\^~[]`";
    for (int c; (c = fgetc(in)) != EOF;) {
        if (c == '/' || c == '\\') valid_path = 1;
        if (isalnum(c) || strchr(path_chars, c)) {
            path++;
            continue;
        }
        break;
    }
    if (!valid_path) {
        fseek(in, pos, SEEK_SET);
        return 0;
    }
    if(domain && (protocol || path)) {
        return ftell(in) - pos;
    }
    fseek(in, pos, SEEK_SET);
    return 0;
}

