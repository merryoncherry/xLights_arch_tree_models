// Copyright (c) 2014 framerik <framerik@gmail.com>
// All rights reserved
//
// This library is dual-licensed: you can redistribute it and/or modify it under the terms
// of the GNU General Public License version 2 as published by the Free Software Foundation.
// For the terms of this license, see <http://www.gnu.org/licenses/>.
//
// You are free to use this library under the terms of the GNU General Public License, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// If you find this project useful and wish to donate you can do with an
// Amazon Gift Cards - E-mail Delivery https://www.amazon.it/gp/product/B005VG4G3U/gcrnsts

#include "wxhttpserver.h"

HttpRequest::HttpRequest(HttpConnection &connection) :
	_connection(connection)
{
}

HttpRequest::HttpRequest(HttpConnection &connection, const wxString &input) :
	_connection(connection)
{
	Parse(input);
}

void HttpRequest::Parse(const wxString &input)
{
    _data = "";

    // Find first row and parse
    auto rstart = input.Find('\n');
    if (rstart <= 0) {
        return;
    }
    wxArrayString htokens = wxSplit(input.Mid(0, rstart), ' ');

    if (htokens.Count() > 0)
        _method = htokens[0].Trim(true).Trim(false);
    if (htokens.Count() > 1)
        _uri = htokens[1].Trim(true).Trim(false);
    if (htokens.Count() > 2)
        _version = htokens[2].Trim(true).Trim(false);

    do {
        ++rstart;
        auto nxt = rstart;
        while (nxt < input.Length()) {
            if (input[nxt] == '\n')
                break;
            ++nxt;
        }
        // No more newlines found
        if (nxt >= input.Length()) {
            rstart = input.Length();
            break;
        }
        // Check for a blank line indicating start of data section
        if ((nxt == rstart) || (nxt - rstart == 1 && input[rstart] == '\r')) {
            rstart = nxt + 1;
            break;
        }

        // An HTTP header
        wxString hstr = input.Mid(rstart, nxt-rstart);
        int index = hstr.Find(':');
        if (index != -1)
        {
            wxString key = hstr.Mid(0, index);
            key.Trim(true).Trim(false);

            wxString value = hstr.Mid(index + 1);
            value.Trim(true).Trim(false);

            _headers.Add(key, value);
        }
        rstart = nxt;
    } while (1);
    _data = wxString(input, rstart, input.Length() - rstart);
}
