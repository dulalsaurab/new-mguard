
#ifndef MGUARD_DATA_ADAPTER_HPP
#define MGUARD_DATA_ADAPTER_HPP

#include "publisher.hpp"
#include "pre-processor.hpp"
#include "file-processor.hpp"

#include <ndn-cxx/face.hpp>
#include <PSync/full-producer.hpp>

#include<nac-abe/attribute-authority.hpp>
#include <nac-abe/cache-producer.hpp>

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
  DataAdapter(ndn::Face& face, const ndn::Name& attrAuthorityPrefix, const ndn::Name& producerPrefix);

  /*
    read the CSV file corresponding to the names
    names will be provided by pre-processor
  */
  // ~ DataAdapter() {}
  bool 
  readData(std::vector<std::string> streamNames);

  ndn::Name
  makeDataName(ndn::Name streamName, std::string timestamp);

  void
  makeDataContent(std::vector<std::string> data, ndn::Name streamName);

  template<ndn::encoding::Tag TAG>
  size_t
  wireEncode(ndn::EncodingImpl<TAG>& block) const;

  const ndn::Block&
  wireEncode();

private:
  ndn::Face& m_face;
  ndn::KeyChain m_keyChain;
  ndn::Scheduler m_scheduler;

  mutable ndn::Block m_wire;
  // DataPreprocessor m_preProcessor;
  FileProcessor m_fileProcessor;
  // AttributeMappingFileProcessor m_attributeMappingFileProcessor;
  std::string m_tempRow;

  ndn::nacabe::KpAttributeAuthority m_kpAttributeAuthority;
  ndn::nacabe::CacheProducer m_producer;
  ndn::Name m_attrAuthorityPrefix;
  ndn::Name m_producerPrefix;
  ndn::security::Certificate m_producerCert;
  ndn::security::Certificate m_authorityCert;
  ndn::security::Certificate m_ownerCert;
};

} // mguard

#endif // MGUARD_DATA_ADAPTER_HPP