

#ifndef MGUARD_PRODUCER_HPP
#define MGUARD_PRODUCER_HPP

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/util/time.hpp>

using namespace ndn::time_literals;

namespace mguard {
namespace producer {
namespace tlv {

}

class Error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};


class Producer
{

};

} //namespace producer
} //namespace mguard

#endif // MGUARD_PRODUCER_HPP