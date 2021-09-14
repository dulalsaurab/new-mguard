#ifndef MGUARD_DATA_ADAPTER_HPP
#define MGUARD_DATA_ADAPTER_HPP

#include "publisher.hpp"
#include "pre-processor.hpp"
#include "file-processor.hpp"
#include "util/stream.hpp"

#include <ndn-cxx/face.hpp>
#include <PSync/full-producer.hpp>

#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/cache-producer.hpp>

#include <unordered_map>

namespace mguard {
namespace tlv {

enum
{
  mGuardContent = 128,
  DataRow = 129
};
} // namespace tlv


class DataAdapter
{

public:
  DataAdapter(ndn::Face& face, ndn::security::KeyChain& keyChain, 
                        const ndn::Name& attrAuthorityPrefix, const ndn::Name& producerPrefix);

  /*
    read the CSV file corresponding to the names
    names will be provided by pre-processor
  */
  // ~ DataAdapter() {}
  void 
  readData(util::Stream& stream);

  ndn::Name
  makeDataName(ndn::Name streamName, std::string timestamp);

  bool
  makeDataContent(std::vector<std::string> data, util::Stream &stream);

  template<ndn::encoding::Tag TAG>
  size_t
  wireEncode(ndn::EncodingImpl<TAG>& block) const;

  const ndn::Block&
  wireEncode();


// communication
  void
  run();

  void
  stop();

  void
  setInterestFilter(const ndn::Name& name, const bool loopback = false);

  void
  processInterest(const ndn::Name& name, const ndn::Interest& interest);

  void
  onRegistrationSuccess(const ndn::Name& name);

  void
  onRegistrationFailed(const ndn::Name& name);
  
  void
  sendApplicationNack(const ndn::Name& name);

private:
  ndn::Face& m_face;
  ndn::KeyChain& m_keyChain;

  mutable ndn::Block m_wire;
  // DataPreprocessor m_preProcessor;
  FileProcessor m_fileProcessor;
  // AttributeMappingFileProcessor m_attributeMappingFileProcessor;
  std::string m_tempRow;
  ndn::Name m_attrAuthorityPrefix;
  ndn::Name m_producerPrefix;
  ndn::security::Certificate m_producerCert;
  ndn::security::Certificate m_authorityCert;
  ndn::nacabe::KpAttributeAuthority m_kpAttributeAuthority;
  ndn::nacabe::CacheProducer m_producer;

  std::unordered_map<ndn::Name, std::shared_ptr<ndn::Data>> m_dataBuffer; //need to limit the size of the buffer
};

} // mguard

#endif // MGUARD_DATA_ADAPTER_HPP