diff --git a/src/user/subscriber.cpp b/src/user/subscriber.cpp
index 93a6ba7..4eed52c 100644
--- a/src/user/subscriber.cpp
+++ b/src/user/subscriber.cpp
@@ -3,6 +3,8 @@

 #include <nac-abe/attribute-authority.hpp>

+#include <boost/algorithm/string.hpp>
+
 #include <iostream>

 NDN_LOG_INIT(mguard.subscriber);
@@ -70,7 +72,7 @@ Subscriber::run(bool runSync)
 {
   try {
     NDN_LOG_INFO("Starting Face");
-
+
     if (runSync) {
       m_psync_consumer.sendHelloInterest();
       // sleep some time for sync to kick in
@@ -81,7 +83,7 @@ Subscriber::run(bool runSync)
   }
   catch (const std::exception& ex)
   {
-    NDN_LOG_ERROR("Face error: " << ex.what());
+    NDN_LOG_ERROR("Face error: " << ex.what());
     NDN_THROW(Error(ex.what()));
   }
 }
@@ -100,7 +102,7 @@ Subscriber::expressInterest(const ndn::Name& name, bool canBePrefix, bool mustBe
   ndn::Interest interest(name);
   interest.setCanBePrefix(false);
   interest.setMustBeFresh(mustBeFresh); //set true if want data explicit from producer.
-  interest.setInterestLifetime(160_ms);
+  // interest.setInterestLifetime(160_ms);

   m_face.expressInterest(interest,
                          bind(&Subscriber::onData, this, _1, _2),
@@ -123,10 +125,14 @@ Subscriber::onTimeout(const ndn::Interest& interest)
   NDN_LOG_INFO("Interest: " << interestName << " timed out ");
   // one time re-transmission
   auto it = m_retransmissionCount.find(interest.getName());
-
+
+ // TODO: can make this better
+ //  bool b = boost::algorithm::contains(interestName.toUri(), "controller");
+
   if (it == m_retransmissionCount.end()) {
     NDN_LOG_INFO("Re-transmitting interest: " << interest.getName() << " retransmission count: " << 1);
     m_retransmissionCount.emplace(interestName, 1); // will
+    expressInterest(interestName);
     return;
   }
   if (it->second <= 3) {
@@ -210,7 +216,7 @@ Subscriber::wireDecode(const ndn::Block& wire)
   }
   if (val != wire.elements_end() && val->type() == mguard::tlv::mGuardPublisher)
   {
-    std::vector<ndn::Name> tempNameBuffer;
+    std::vector<ndn::Name> tempNameBuffer;
     NDN_LOG_DEBUG ("Received data from publisher");
     val->parse();
     for (auto it = val->elements_begin(); it != val->elements_end(); ++it) {
@@ -230,7 +236,7 @@ Subscriber::wireDecode(const ndn::Block& wire)

       m_abe_consumer.consume(dataName.getPrefix(-1), bind(&Subscriber::abeOnData, this, _1),
                              bind(&Subscriber::abeOnError, this, _1));
-      NDN_LOG_DEBUG("data names: " << dataName);
+      NDN_LOG_DEBUG("data names: " << dataName);
     }
   }
 }
@@ -250,4 +256,4 @@ Subscriber::abeOnError(const std::string& errorMessage)


 } // subscriber
-} // mguard
\ No newline at end of file
+} // mguard
