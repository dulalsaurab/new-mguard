#include "../test-common.hpp"

#include <controller/parser.hpp>

using namespace ndn;

namespace mguard {
namespace parser {
namespace tests {

BOOST_FIXTURE_TEST_SUITE(TestParser, mguard::tests::IdentityTimeFixture)

BOOST_AUTO_TEST_CASE(Constructor)
{
    PolicyParser parser("tests/unit-tests/parser-resources/available_streams");
    PolicyDetail result;

    result = parser.parsePolicy("tests/unit-tests/parser-resources/policy1");
    BOOST_CHECK(result.policyIdentifier == "1");
    BOOST_CHECK(result.abePolicy == "((/ndn/org/md2k/mguard/dd40c/phone/accelerometer AND /ndn/org/md2k/ATTRIBUTE/location/gym) OR (/ndn/org/md2k/mguard/dd40c/phone/battery AND /ndn/org/md2k/ATTRIBUTE/location/commuting))");
    for (const std::string &stream : result.streams) {
        BOOST_CHECK(
                stream == "/ndn/org/md2k/mguard/dd40c/phone/accelerometer" ||
                stream == "/ndn/org/md2k/mguard/dd40c/phone/battery"
                );
    }
    for (const std::string &requester : result.requesters) {
        BOOST_CHECK(requester == "/ndn/org/md2k/A");
    }

    result = parser.parsePolicy("tests/unit-tests/parser-resources/policy1");
    BOOST_CHECK(result.policyIdentifier == "1");
    BOOST_CHECK(result.abePolicy == "((/ndn/org/md2k/mguard/dd40c/phone/accelerometer AND /ndn/org/md2k/ATTRIBUTE/location/gym) OR (/ndn/org/md2k/mguard/dd40c/phone/battery AND /ndn/org/md2k/ATTRIBUTE/location/commuting))");
    for (const std::string &stream : result.streams) {
        BOOST_CHECK(
                stream == "/ndn/org/md2k/mguard/dd40c/phone/accelerometer" ||
                stream == "/ndn/org/md2k/mguard/dd40c/phone/battery"
                );
    }
    for (const std::string &requester : result.requesters) {
        BOOST_CHECK(requester == "/ndn/org/md2k/A");
    }

}
BOOST_AUTO_TEST_SUITE_END()
}
}
}
