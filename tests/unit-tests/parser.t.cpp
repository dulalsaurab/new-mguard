#include "../test-common.hpp"

#include <controller/parser.hpp>

using namespace ndn;

namespace mguard {
namespace parser {
namespace tests {

BOOST_FIXTURE_TEST_SUITE(TestParser, mguard::tests::IdentityTimeFixture)

BOOST_AUTO_TEST_CASE(Constructor)
{
    // NOTE: a list of attributes in the ABE policy is only surrounded with () if there is more than one attribute.
    // if it is just a single attribute added to the policy, there are no parentheses

    PolicyParser parser("tests/unit-tests/parser-resources/available_streams");
    PolicyDetail result;

    // policy 1
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

    // policy 2
    result = parser.parsePolicy("tests/unit-tests/parser-resources/policy2");
    BOOST_CHECK(result.policyIdentifier == "2");
    BOOST_CHECK(result.abePolicy == "(/ndn/org/md2k/mguard/dd40c/phone/accelerometer OR /ndn/org/md2k/mguard/dd40c/phone/gyroscope OR /ndn/org/md2k/mguard/dd40c/phone/battery OR /ndn/org/md2k/mguard/dd40c/phone/gps OR /ndn/org/md2k/mguard/dd40c/data_analysis/gps_episodes_and_semantic_location)");
    for (const std::string &stream : result.streams) {
        BOOST_CHECK(
                stream == "/ndn/org/md2k/mguard/dd40c/phone/accelerometer" ||
                stream == "/ndn/org/md2k/mguard/dd40c/data_analysis/gps_episodes_and_semantic_location" ||
                stream == "/ndn/org/md2k/mguard/dd40c/phone/gyroscope" ||
                stream == "/ndn/org/md2k/mguard/dd40c/phone/battery" ||
                stream == "/ndn/org/md2k/mguard/dd40c/phone/gps"
        );
    }
    for (const std::string &requester : result.requesters) {
        BOOST_CHECK(
                requester == "/ndn/org/md2k/A" ||
                requester == "/ndn/org/md2k/B" ||
                requester == "/ndn/org/md2k/C" ||
                requester == "/ndn/org/md2k/D" ||
                requester == "/ndn/org/md2k/E" ||
                requester == "/ndn/org/md2k/F"
        );
    }

    // policy 3
    result = parser.parsePolicy("tests/unit-tests/parser-resources/policy3");
    BOOST_CHECK(result.policyIdentifier == "3");
    BOOST_CHECK(result.abePolicy == "/ndn/org/md2k/mguard/dd40c/phone/battery");
    for (const std::string &stream : result.streams) {
        BOOST_CHECK(
                stream == "/ndn/org/md2k/mguard/dd40c/phone/battery"
        );
    }
    for (const std::string &requester : result.requesters) {
        BOOST_CHECK(
                requester == "/ndn/org/md2k/A" ||
                requester == "/ndn/org/md2k/B" ||
                requester == "/ndn/org/md2k/C" ||
                requester == "/ndn/org/md2k/D" ||
                requester == "/ndn/org/md2k/E"
        );
    }

    // policy 4
    result = parser.parsePolicy("tests/unit-tests/parser-resources/policy4");
    BOOST_CHECK(result.policyIdentifier == "4");
    BOOST_CHECK(result.abePolicy == "(/ndn/org/md2k/mguard/dd40c/phone/accelerometer OR /ndn/org/md2k/mguard/dd40c/phone/gyroscope OR /ndn/org/md2k/mguard/dd40c/phone/battery OR /ndn/org/md2k/mguard/dd40c/data_analysis/gps_episodes_and_semantic_location)");
    for (const std::string &stream : result.streams) {
        BOOST_CHECK(
                stream == "/ndn/org/md2k/mguard/dd40c/phone/accelerometer" ||
                stream == "/ndn/org/md2k/mguard/dd40c/data_analysis/gps_episodes_and_semantic_location" ||
                stream == "/ndn/org/md2k/mguard/dd40c/phone/gyroscope" ||
                stream == "/ndn/org/md2k/mguard/dd40c/phone/battery"
        );
    }
    for (const std::string &requester : result.requesters) {
        BOOST_CHECK(
                requester == "/ndn/org/md2k/A" ||
                requester == "/ndn/org/md2k/B" ||
                requester == "/ndn/org/md2k/C" ||
                requester == "/ndn/org/md2k/D" ||
                requester == "/ndn/org/md2k/E"
        );
    }

    // policy 5
    result = parser.parsePolicy("tests/unit-tests/parser-resources/policy5");
    BOOST_CHECK(result.policyIdentifier == "5");
    BOOST_CHECK(result.abePolicy == "/ndn/org/md2k/mguard/dd40c/phone/accelerometer");
    for (const std::string &stream : result.streams) {
        BOOST_CHECK(
                stream == "/ndn/org/md2k/mguard/dd40c/phone/accelerometer"
        );
    }
    for (const std::string &requester : result.requesters) {
        BOOST_CHECK(
                requester == "/ndn/org/md2k/E" ||
                requester == "/ndn/org/md2k/F"
        );
    }

    // policy 6
    // we do not have implementation for wildcards within stream names

    // policy 7
    // we do not have an accelerometer attribute

    // policy 8
    result = parser.parsePolicy("tests/unit-tests/parser-resources/policy8");
    BOOST_CHECK(result.policyIdentifier == "8");
    BOOST_CHECK(result.abePolicy == "((/ndn/org/md2k/mguard/dd40c/phone/accelerometer OR /ndn/org/md2k/mguard/dd40c/phone/gyroscope OR /ndn/org/md2k/mguard/dd40c/phone/battery OR /ndn/org/md2k/mguard/dd40c/phone/gps OR /ndn/org/md2k/mguard/dd40c/data_analysis/gps_episodes_and_semantic_location) AND /ndn/org/md2k/ATTRIBUTE/location/work)");
    for (const std::string &stream : result.streams) {
        BOOST_CHECK(
                stream == "/ndn/org/md2k/mguard/dd40c/phone/accelerometer" ||
                stream == "/ndn/org/md2k/mguard/dd40c/data_analysis/gps_episodes_and_semantic_location" ||
                stream == "/ndn/org/md2k/mguard/dd40c/phone/gyroscope" ||
                stream == "/ndn/org/md2k/mguard/dd40c/phone/battery" ||
                stream == "/ndn/org/md2k/mguard/dd40c/phone/gps"
        );

    }
    for (const std::string &requester : result.requesters) {
        BOOST_CHECK(
                requester == "/ndn/org/md2k/A" ||
                requester == "/ndn/org/md2k/B" ||
                requester == "/ndn/org/md2k/C" ||
                requester == "/ndn/org/md2k/D" ||
                requester == "/ndn/org/md2k/E"
        );
    }

    // policy 9
    result = parser.parsePolicy("tests/unit-tests/parser-resources/policy9");
    BOOST_CHECK(result.policyIdentifier == "9");
    BOOST_CHECK(result.abePolicy == "(/ndn/org/md2k/mguard/dd40c/phone/accelerometer AND /ndn/org/md2k/ATTRIBUTE/location/work AND (/ndn/org/md2k/ATTRIBUTE/activity/eating OR /ndn/org/md2k/ATTRIBUTE/activity/drinking OR /ndn/org/md2k/ATTRIBUTE/activity/sleeping OR /ndn/org/md2k/ATTRIBUTE/activity/unknown))");
    for (const std::string &stream : result.streams) {
        BOOST_CHECK(
                stream == "/ndn/org/md2k/mguard/dd40c/phone/accelerometer"
        );

    }
    for (const std::string &requester : result.requesters) {
        BOOST_CHECK(
                requester == "/ndn/org/md2k/A"
        );
    }

    // policy 10
    result = parser.parsePolicy("tests/unit-tests/parser-resources/policy10");
    BOOST_CHECK(result.policyIdentifier == "10");
    BOOST_CHECK(result.abePolicy == "(/ndn/org/md2k/mguard/dd40c/phone/accelerometer AND /ndn/org/md2k/ATTRIBUTE/location/work AND (/ndn/org/md2k/ATTRIBUTE/activity/eating OR /ndn/org/md2k/ATTRIBUTE/activity/drinking OR /ndn/org/md2k/ATTRIBUTE/activity/sleeping OR /ndn/org/md2k/ATTRIBUTE/activity/unknown) AND /ndn/org/md2k/ATTRIBUTE/smoking/yes)");
    for (const std::string &stream : result.streams) {
        BOOST_CHECK(
                stream == "/ndn/org/md2k/mguard/dd40c/phone/accelerometer"
        );

    }
    for (const std::string &requester : result.requesters) {
        BOOST_CHECK(
                requester == "/ndn/org/md2k/A"
        );
    }
}
BOOST_AUTO_TEST_SUITE_END()
}
}
}
