/* toxrtp_error.c
 *
 * Tox DHT bootstrap server daemon.
 *
 *  Copyright (C) 2014 Tox project All Rights Reserved.
 *
 *  This file is part of Tox.
 *
 *  Tox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Tox.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "toxrtp_error.h"
#include "toxrtp_helper.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct rtp_error_s {
    char* _message;
    int   _id;

} rtp_error_t;

static rtp_error_t* _register = NULL;
static size_t _it = 0;

void t_rtperr_register ( int _id, const char* _info )
{
    size_t _info_size = strlen ( _info );

    if ( !_register ) {
        _register = calloc ( sizeof ( rtp_error_t ), 1 );
    } else {
        _register = realloc ( _register, sizeof ( rtp_error_t ) * ( _it + 1 ) );
    }
    assert(_register);


    rtp_error_t* _current = & _register[_it];

    _current->_id = _id;
    _current->_message = calloc ( sizeof(char), _info_size );
    assert(_current->_message);

    t_memcpy ( (uint8_t*)_current->_message, (const uint8_t*)_info, _info_size );
    _it ++;
}

const char* t_rtperr ( int _errno )
{
    if ( !_register )
        return "Unregistered";

    uint32_t _i;

    for ( _i = _it; _i--; ) {
        if ( _register[_i]._id == _errno ) {
            return _register[_i]._message;
        }
    }

    return "Invalid error id!";
}

void t_rtperr_print ( const char* _val, ... )
{
    va_list _args;
    va_start ( _args, _val );
    vfprintf ( stderr, _val, _args );
    va_end ( _args );
}
