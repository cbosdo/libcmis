/* libcmis
 * Version: MPL 1.1 / GPLv2+ / LGPLv2+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2011 SUSE <cbosdonnat@suse.com>
 *
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPLv2+"), or
 * the GNU Lesser General Public License Version 2 or later (the "LGPLv2+"),
 * in which case the provisions of the GPLv2+ or the LGPLv2+ are applicable
 * instead of those above.
 */

#include <map>
#include <string>
#include <stdlib.h> 

#include <libcmis/session-factory.hxx>

#include "internals.hxx"
#include "session-factory.h"

using namespace std;

namespace
{
    size_t const CRED_MAX_LEN = 1024;

    class WrapperAuthProvider : public libcmis::AuthProvider
    {
        private:
            libcmis_authenticationCallback m_callback;

        public:
            WrapperAuthProvider( libcmis_authenticationCallback callback ) :
                m_callback( callback )
            {
            }
            virtual ~WrapperAuthProvider( ) { };

            virtual bool authenticationQuery( string& username, string& password );
    };

    bool WrapperAuthProvider::authenticationQuery( string& username, string& password )
    {
        /* NOTE: As I understand this, the callback is responsible for
         * filling the correct username and password (possibly using
         * the passed values as defaults in some dialog or so). But then
         * there is no guarantee that the new username/password will
         * not be longer than the present one, in which case it will
         * not fit into the available space! For now, use a buffer size
         * big enough for practical purposes.
         *
         * It might be a better idea to change the callback's signature
         * to bool ( * )( char** username, char** password )
         * and make it the callee's responsibility to reallocate the
         * strings if it needs to.
         */
        char user[CRED_MAX_LEN];
        strncpy(user, username.c_str( ), sizeof( user ) );
        user[min( username.size( ), CRED_MAX_LEN )] = '\0';
        char pass[CRED_MAX_LEN];
        strncpy(pass, password.c_str( ), sizeof( pass ) );
        pass[min( password.size( ), CRED_MAX_LEN )] = '\0';

        bool result = m_callback( user, pass );

        // Update the username and password with the input
        string newUser( user );
        username.swap( newUser );

        string newPass( pass );
        password.swap( newPass );

        return result;
    }
}

void libcmis_setAuthenticationCallback( libcmis_authenticationCallback callback )
{
    libcmis::AuthProviderPtr provider( new WrapperAuthProvider( callback ) );
    libcmis::SessionFactory::setAuthenticationProvider( provider );
}

void libcmis_setProxySettings( char* proxy, char* noProxy,
        char* proxyUser, char* proxyPass )
{
    libcmis::SessionFactory::setProxySettings( string( proxy ), string( noProxy ),
            string( proxyUser ), string( proxyPass ) );
}

const char* libcmis_getProxy( )
{
    return libcmis::SessionFactory::getProxy( ).c_str();
}

const char* libcmis_getNoProxy( )
{
    return libcmis::SessionFactory::getNoProxy( ).c_str();
}

const char* libcmis_getProxyUser( )
{
    return libcmis::SessionFactory::getProxyUser( ).c_str();
}

const char* libcmis_getProxyPass( )
{
    return libcmis::SessionFactory::getProxyPass( ).c_str();
}

libcmis_SessionPtr libcmis_createSession(
        char* bindingUrl,
        char* repositoryId,
        char* username,
        char* password,
        libcmis_OAuth2DataPtr oauth2,
        bool  verbose,
        libcmis_ErrorPtr error )
{
    libcmis_SessionPtr session = NULL;

    try
    {
        libcmis::OAuth2DataPtr oauth2Handle;
        if ( oauth2 != NULL )
            oauth2Handle = oauth2->handle;

        libcmis::Session* handle = libcmis::SessionFactory::createSession( bindingUrl, username,
                password, repositoryId, oauth2Handle, verbose );
        session = new libcmis_session( );
        session->handle = handle;
    }
    catch ( const libcmis::Exception& e )
    {
        // Set the error handle
        if ( error != NULL )
            error->handle = new libcmis::Exception( e );
    }

    return session;
}

libcmis_RepositoryPtr* libcmis_getRepositories(
        char* bindingUrl,
        char* username,
        char* password,
        bool  verbose,
        libcmis_ErrorPtr error )
{
    libcmis_RepositoryPtr* repositories = NULL;
    try
    {
        list< libcmis::RepositoryPtr > repos = libcmis::SessionFactory::getRepositories(
               bindingUrl, username, password, verbose );

        repositories = new libcmis_RepositoryPtr[ repos.size() ];
        list< libcmis::RepositoryPtr >::iterator it = repos.begin( );
        for ( int i = 0; it != repos.end( ); ++it, ++i )
        {
            libcmis_RepositoryPtr repository = new libcmis_repository( );
            repository->handle = *it;
            repositories[i] = repository;
        }
    }
    catch ( const libcmis::Exception& e )
    {
        // Set the error handle
        if ( error != NULL )
            error->handle = new libcmis::Exception( e );
    }
    return repositories;
}
