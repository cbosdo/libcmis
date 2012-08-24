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

#include "ws-objectservice.hxx"
#include "ws-requests.hxx"
#include "ws-session.hxx"

using namespace std;

ObjectService::ObjectService( WSSession* session ) :
    m_session( session ),
    m_url( )
{
    m_url = session->getServiceUrl( "ObjectService" );
}

ObjectService::ObjectService( const ObjectService& copy ) :
    m_session( copy.m_session ),
    m_url( copy.m_url )
{
}

ObjectService::~ObjectService( )
{
}

ObjectService& ObjectService::operator=( const ObjectService& copy )
{
    if ( this != &copy )
    {
        m_session = copy.m_session;
        m_url = copy.m_url;
    }

    return *this;
}

libcmis::ObjectPtr ObjectService::getObject( string repoId, string id ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object;

    GetObject request( repoId, id );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        GetObjectResponse* response = dynamic_cast< GetObjectResponse* >( resp );
        if ( response != NULL )
            object = response->getObject( );
    }

    return object;
}

libcmis::ObjectPtr ObjectService::updateProperties( string repoId, libcmis::Object* toUpdate ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object;

    if ( toUpdate != NULL )
    {
        UpdateProperties request( repoId, toUpdate->getId( ), toUpdate->getProperties( ), toUpdate->getChangeToken( ) );
        vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
        if ( responses.size( ) == 1 )
        {
            SoapResponse* resp = responses.front( ).get( );
            UpdatePropertiesResponse* response = dynamic_cast< UpdatePropertiesResponse* >( resp );
            if ( response != NULL )
            {
                string id = response->getObjectId( );
                object = getObject( repoId, id );
                if ( id == toUpdate->getId( ) )
                    toUpdate->refresh( );
            }
        }
    }

    return object;
}
