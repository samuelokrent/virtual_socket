#include <iostream>
#include <string>
#include "../src/protocol.h"

#include "TestSuite.h"
#include "TestCaller.h"
#include "TestRunner.h"
#include "TestResult.h"
#include "TestResultCollector.h"
#include "TextTestProgressListener.h"
#include "CompilerOutputter.h"
#include "TestFixture.h"

using std::string;
using CppUnit::TestSuite;
using CppUnit::TestCaller;
using CppUnit::TestRunner;
using CppUnit::TestFixture;


class ProtocolTest : public TestFixture {

	public:
		
		ProtocolTest() {};

		void testConnectRequest() {
			CPPUNIT_ASSERT(
				p.makeConnectRequest("hello") ==
				"{\"id\":\"hello\",\"type\":\"connect\"}\n"
			);
		}

        void testRegisterRequest() {
            CPPUNIT_ASSERT(
                p.makeRegisterRequest("hello") ==
				"{\"id\":\"hello\",\"type\":\"register\"}\n"
            );
        }

        void testResponse() {

			// A successful respone
            CPPUNIT_ASSERT(
                p.makeResponse(true, "", "hostname", "") ==
				"{\"status\":\"ok\"}\n"
            );

			// An error response
			CPPUNIT_ASSERT(
                p.makeResponse(false, "some error", "", "") ==
				"{\"error_message\":\"some error\",\"status\":\"error\"}\n"
            );
        }

		static CppUnit::Test * suite() {
			TestSuite *testSuite = new TestSuite ("ProtocolTest");
			testSuite->addTest (new TestCaller<ProtocolTest>  
	  			("testConnectRequest", &ProtocolTest::testConnectRequest));
			testSuite->addTest (new TestCaller<ProtocolTest>
                ("testRegisterRequest", &ProtocolTest::testRegisterRequest));
			testSuite->addTest (new TestCaller<ProtocolTest>
                ("testResponseRequest", &ProtocolTest::testResponse));
			return testSuite;
		}

	private:
		
		Protocol p;

};

int main( int argc, char* argv[] ) {
	std::string testPath = (argc > 1) ? std::string(argv[1]) : "";

	CppUnit::TestResult controller;
	CppUnit::TestResultCollector result;
	controller.addListener( &result );

	// Add a listener that print dots as test run.
	CppUnit::TextTestProgressListener progress;
	controller.addListener( &progress );

	// Add the top suite to the test runner
	CppUnit::TestRunner runner;
	runner.addTest(ProtocolTest::suite());
	try
	{
		std::cout << "Running "  <<  testPath;
		runner.run( controller, testPath );

		std::cerr << std::endl;

		// Print test in a compiler compatible format.
		CppUnit::CompilerOutputter outputter( &result, std::cerr );
		outputter.write();
	}
	catch ( std::invalid_argument &e )  // Test path not resolved
	{
		std::cerr  <<  std::endl
				<<  "ERROR: "  <<  e.what()
				<< std::endl;
		return 0;
	}

	return result.wasSuccessful() ? 0 : 1;
 }
