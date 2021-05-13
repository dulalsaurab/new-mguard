

#ifndef MGUARD_CONSUMER_HPP
#define MGUARD_CONSUMER_HPP

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/util/time.hpp>

using namespace ndn::time_literals;

namespace mguard {
namespace consumer {
namespace tlv {

}

class Error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};


class Consumer
{

};

} //namespace consumer
} //namespace mguard

#endif // MGUARD_CONSUMER_HPP