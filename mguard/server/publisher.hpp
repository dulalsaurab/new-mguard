

// #ifndef MGUARD_PUBLISHER_HPP
// #define MGUARD_PUBLISHER_HPP

// #include <PSync/full-producer.hpp>

// #include <string>
// #include <iostream>

// #include <ndn-cxx/face.hpp>
// #include <ndn-cxx/util/random.hpp>
// #include <ndn-cxx/util/scheduler.hpp>
// #include <ndn-cxx/util/time.hpp>

// using namespace ndn::time_literals;

// namespace mguard {
// namespace publisher {

// struct SyncDataInfo
// {
//   ndn::Name prefix;
//   uint64_t highSeq;
//   uint64_t lowSeq;
// };

// typedef std::function<void(const std::vector<SyncDataInfo>& updates)> SyncUpdateCallback;

// namespace tlv {

// }

// class Error : public std::runtime_error
// {
// public:
//   using std::runtime_error::runtime_error;
// };


// class Publisher
// {

// public:
//   Publisher(ndn::Face& face, const ndn::Name& syncPrefix,
//            const ndn::Name& userPrefix,
//            ndn::time::milliseconds syncInterestLifetime,
//            const SyncUpdateCallback& syncUpdateCallback);

//   void
//   run();

//   void 
//   stop();

//   void
//   addUserNode(const ndn::Name& userPrefix); //might need to change the name "user" here

//   void
//   publishUpdate(const ndn::Name& userPrefix);

//   void
//   onSyncUpdate(const std::vector<psync::MissingDataInfo>& updates);

//   void 
//   printSyncUPdate(const std::vector<SyncDataInfo> updates);

// private:
//   std::shared_ptr<psync::FullProducer> m_syncLogic;
//   ndn::Face& m_face;
//   SyncUpdateCallback m_syncUpdateCallback;

// };

// } //namespace publisher
// } //namespace mguard

// #endif // MGUARD_PUBLISHER_HPP