/*
 *
 *  Copyright (c) 2011
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stddef.h>
#include <libcryptsetup.h>
#include "includes.h"
#include "tcplay_support.h"

static int _close_mapper( const char * mapper )
{
	int r = 1 ;

	struct crypt_device * cd ;

	tc_api_task task ;

	/*
	 * zuluCryptTrueCryptOrVeraCryptVolume() is defined in status.c
	 */
	if( zuluCryptTrueCryptOrVeraCryptVolume( mapper ) ){

		if( tc_api_init( 0 ) == TC_OK ){

			task = tc_api_task_init( "unmap" ) ;

			if( task != 0 ){

				mapper = mapper + StringLastIndexOfChar_1( mapper,'/' ) + 1 ;

				tc_api_task_set( task,"map_name",mapper ) ;

				r = tc_api_task_do( task ) ;

				tc_api_task_uninit( task ) ;
			}
		}
	}else{
		if( crypt_init_by_name( &cd,mapper ) == 0 ){

			r = crypt_deactivate( cd,mapper ) ;

			crypt_free( cd ) ;
		}
	}

	return r ;
}

int zuluCryptCloseMapper( const char * mapper )
{
	int i ;

	for( i = 0 ; i < 3 ; i++ ){

		if( _close_mapper( mapper ) == 0 ){

			return 0 ;
		}else{
			sleep( 1 ) ;
		}

	}

	return 1 ;
}
