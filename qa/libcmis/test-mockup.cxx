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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

#define SERVER_URL string( "http://mockup/binding" )
#define SERVER_USERNAME string( "tester" )
#define SERVER_PASSWORD string( "somepass" )

#include "ws-session.hxx"

using namespace std;

class MockupTest : public CppUnit::TestFixture
{
    public:
        void mockupTest( );

        CPPUNIT_TEST_SUITE( MockupTest );
        CPPUNIT_TEST( mockupTest );
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( MockupTest );

void MockupTest::mockupTest( )
{
    list< libcmis::RepositoryPtr > actual = WSSession::getRepositories( SERVER_URL, SERVER_USERNAME, SERVER_PASSWORD );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ), actual.size( ) );
}
