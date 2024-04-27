
#include "email.h"
#include <ctype.h>
#include <string.h>

u32 find_email(FILE * in) {
    const u8 * em1 = "._%+-:", * em2 = ".-_:";
    u32 pos = ftell(in);
    u32 user_len = 0, domain_len = 0;
    for (int c; (c = fgetc(in)) != EOF;) {
        If(user_len > 64, goto bail)
        If(isalnum(c) || strchr(em1, c), user_len++)
        else If(c == '@', break) else goto bail;
    }
    for (int c; (c = fgetc(in)) != EOF;) {
        If(isalnum(c) || strchr(em2, c), domain_len++) else If(c == '.', break)
    }
    If(user_len && domain_len, return ftell(in) - pos)
    bail: fseek(in, pos, SEEK_SET); return 0;
}

u32 find_url(FILE * in) {
    u32 pos = ftell(in);
    // short path: check for localhost
    u8 * localhost = "localhost", buf[9];
    fread(buf, 1, 9, in);
    If(!memcmp(buf, localhost, 9), return 9)
    fseek(in, pos, SEEK_SET);
    // long path:
    u32 protocol = 0, domain = 0, domain_letters = 0, valid_domain = 0, path = 0, valid_path = 0;
    for (int c; (c = fgetc(in)) != EOF;) {
        // Odd.
        If(protocol > 8, goto bail) protocol++;
        If(isalpha(c), continue) else If(c == ':' && fgetc(in) == '/' && fgetc(in) == '/', protocol += 2; break)
        protocol = 0; fseek(in, pos, SEEK_SET); break;
    }
    for (int c; (c = fgetc(in)) != EOF;) {
        If (isalpha(c), domain_letters++; domain++; continue)
        If (isdigit(c) || c == '-' || c == '_' || c == '[' || c == ']', domain++; continue)
        If (c == '.' || c == ':' || c == '@',
            If(c != ':', domain_letters = 0) valid_domain = 1; domain++; continue)
        If (domain == 0 || domain_letters == 0, goto bail)
        ungetc(c, in); break;
    }
    If (!valid_domain, goto bail)
    const char * path_chars = "$-_.+!*'(),;/?:@=&#%|\\^~[]`";
    for (int c; (c = fgetc(in)) != EOF;) {
        If (c == '/' || c == '\\', valid_path = 1)
        If (isalnum(c) || strchr(path_chars, c), path++; continue)
        break;
    }
    If (!valid_path, goto bail)
    If (domain && (protocol || path), return ftell(in) - pos)
    bail:;
        fseek(in, pos, SEEK_SET);
        return 0;
}

