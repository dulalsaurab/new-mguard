#ifndef MGUARD_DATA_ADAPTER_HPP
#define MGUARD_DATA_ADAPTER_HPP

#include "publisher.hpp"
#include "file-processor.hpp"
#include "util/stream.hpp"

#include <PSync/full-producer.hpp>
#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/cache-producer.hpp>

#include <unordered_map>

namespace mguard {

class DataAdapter
{

public:
  DataAdapter(ndn::Face& face, const ndn::Name& producerPrefix, const ndn::Name& aaPrefix);
  
  void
  run();

  void
  stop();

  ndn::Name
  makeDataName(ndn::Name streamName, std::string timestamp);
  
  void
  publishDataUnit(util::Stream& stream);

private:
  ndn::KeyChain m_keyChain;
  ndn::Face& m_face;
  FileProcessor m_fileProcessor;
  std::string m_tempRow;
  ndn::Name m_producerPrefix;
  ndn::security::Certificate m_producerCert;
  ndn::security::Certificate m_ABE_authorityCert;
  mguard::Publisher m_publisher;
};

} // mguard

#endif // MGUARD_DATA_ADAPTER_HPP