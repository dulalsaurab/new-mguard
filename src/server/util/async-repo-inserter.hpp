/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2022-
 *
 * This implementation is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Author: Saurab Dulal <dulal.saurab@gmail.com>
 */

#ifndef MGUARD_UTIL_ASYNC_REPO_INSERTER_HPP
#define MGUARD_UTIL_ASYNC_REPO_INSERTER_HPP

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <queue>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/data.hpp>
#include <ndn-cxx/util/scheduler.hpp>


namespace mguard {
namespace util {
namespace bp = boost::asio::ip;

class AsyncRepoInserter : boost::noncopyable
{
public:
  using AsyncRepoError = boost::system::error_code;
  using AsyncConnectHandler = std::function<void(const AsyncRepoError&)>;
  using AsyncWriteHandler = std::function<void(const ndn::Data&, const AsyncRepoError&)>;

  explicit
  AsyncRepoInserter(boost::asio::io_service& io);

  void
  AsyncConnectToRepo(std::string repoHost, std::string repoPort, const AsyncConnectHandler& connectHandler);

  void
  AsyncWriteDataToRepo(const ndn::Data& data, const AsyncWriteHandler& writeHandler);

private:
  boost::asio::io_service& m_io;
  bp::tcp::resolver m_resolv;
  std::shared_ptr<bp::tcp::socket> m_socket;
};

} // util
} // mguard

#endif
