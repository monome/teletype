#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "match_token.h"
#include "teletype.h"

const size_t kMaxTokenLength = 32;

%%{
    machine scanner; # declare our ragel machine
    write data;      # write any ragel data here
}%%

error_t scanner(const char *data, tele_command_t *out,
                char error_msg[TELE_ERROR_MSG_LENGTH]) {
    const size_t len = strlen(data);

    // required ragel declarations
    int cs;                       // machine state
    int act;                      // used with '=>'
    const char* ts;               // token start
    const char* te;               // token end

    const char* p = data;         // pointer to data
    const char* pe = data + len;  // pointer to end of data
    const char* eof = pe;         // pointer to eof
    (void)scanner_en_main;        // fix unused variable warning
    (void)act;                    // fix unused variable warning

    // reset outputs
    error_msg[0] = 0;
    out->length = 0;
    out->separator = -1;

    %%{
        separator = [ \n\t];
        pre_separator = ': ';
        sub_seperator = '; ';
        invalid_pre = ':';
        invalid_sub = ';';

        token = any+ -- (separator
                         | pre_separator | sub_seperator
                         | invalid_pre | invalid_sub);

        action token {
            // token matched

            // copy the matched token to buf
            char buf[kMaxTokenLength];
            size_t len = te-ts;
            if (len > kMaxTokenLength) len = kMaxTokenLength;
            memcpy(buf, ts, len);
            buf[len] = '\0';

            tele_data_t tele_data;
            if (match_token(buf, len, &tele_data)) {
                // if we have a match, copy data to the the command
                out->data[out->length] = tele_data;

                // increase the command length
                out->length++;

                // if the command length is now too long, abort
                if (out->length >= COMMAND_MAX_LENGTH) return E_LENGTH;
            }
            else {
                // can't match the token, fail
                strcpy(error_msg, buf);
                return E_PARSE;
            }
        }

        action pre_separator {
            // ';' pre separator matched

            // it's a PRE_SEP, we need to record it's position
            // (validate checks for too many PRE_SEP tokens)
            out->data[out->length].tag = PRE_SEP;
            out->data[out->length].value = 0;
            out->separator = out->length;

            // increase the command length
            out->length++;

            // if the command length is now too long, abort
            if (out->length >= COMMAND_MAX_LENGTH) return E_LENGTH;
        }

        action sub_separator {
            // ':' mod separator matched
            out->data[out->length].tag = SUB_SEP;
            out->data[out->length].value = 0;

            // increase the command length
            out->length++;

            // if the command length is now too long, abort
            if (out->length >= COMMAND_MAX_LENGTH) return E_LENGTH;
        }

        action invalid_pre {
            return E_NEED_SPACE_PRE_SEP;
        }

        action invalid_sub {
            return E_NEED_SPACE_SUB_SEP;
        }

        main := |*
            separator;
            pre_separator => pre_separator;
            sub_seperator => sub_separator;
            invalid_pre => invalid_pre;
            invalid_sub => invalid_sub;
            token => token;
        *|;

        write init; # write initialisation
        write exec; # run the machine
    }%%

    if (cs == scanner_error || cs < scanner_first_final) {
        strcpy(error_msg, "SCAN!");
        return E_PARSE;
    }

    return E_OK;
}
