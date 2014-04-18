/**
* Copyright (c) 2013, Ford Motor Company
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following
* disclaimer in the documentation and/or other materials provided with the
* distribution.
*
* Neither the name of the Ford Motor Company nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SRC_COMPONENTS_TIME_MANAGER_INCLUDE_TIME_MANAGER_MEDIA_MANAGER_H_
#define SRC_COMPONENTS_TIME_MANAGER_INCLUDE_TIME_MANAGER_MEDIA_MANAGER_H_


#include "utils/message_queue.h"
#include "utils/threads/thread.h"
#include "utils/singleton.h"
#include "utils/threads/thread_delegate.h"

#include "application_manager_observer.h"
#include "transport_manager_observer.h"
#include "protocol_handler_observer.h"

#include "application_manager/application_manager_impl.h"
#include "transport_manager/transport_manager_impl.h"
#include "protocol_handler/protocol_handler_impl.h"
#include "metric.h"
#include "utils/shared_ptr.h"

#include "log4cxx/logger.h"
#include <string>

namespace time_tester {
class TimeManager: public utils::Singleton<TimeManager>  {

  public:
#ifdef ENABLE_LOG
    static log4cxx::LoggerPtr logger_;
#endif // ENABLE_LOG

    TimeManager();
    ~TimeManager();
    void Init(protocol_handler::ProtocolHandlerImpl* ph);
    void Stop();
    void SendMetric(utils::SharedPtr<Metric> metric);
  private:
    class Streamer : public threads::ThreadDelegate {
      public:
        /*
         * Default constructor
         *
         * @param server  Server pointer
         */
        explicit Streamer(TimeManager* const server);

        /*
         * Destructor
         */
        ~Streamer();

        /*
         * Function called by thread on start
         */
        void threadMain();

        /*
         * Function called by thread on exit
         */
        bool exitThreadMain();

        /*
         * Checks if server is ready
         *
         * @return TRUE if socket is ready otherwise FALSE
         */
        bool is_ready() const;

        /*
         * Starts server
         *
         */
        void start();

        /*
         * Stops server
         *
         */
        void stop();

        /*
         * Sends data to connected client
         *
         * @param block Pointer to the data
         */
        bool send(const std::string &msg);

      private:
        TimeManager* const server_;
        int32_t new_socket_fd_;
        bool is_first_loop_;
        volatile bool is_client_connected_;
        volatile bool stop_flag_;

        DISALLOW_COPY_AND_ASSIGN(Streamer);
    };

    ApplicationManagerObserver app_observer;
    TransportManagerObserver tm_observer;
    ProtocolHandlerObserver ph_observer;

    int16_t port_;
    std::string ip_;
    int32_t                                       socket_fd_;
    bool                                          is_ready_;
    threads::Thread*                              thread_;
    Streamer*                                     streamer_;
    MessageQueue<utils::SharedPtr<Metric> > messages_;

    DISALLOW_COPY_AND_ASSIGN(TimeManager);
};

}  // namespace time_manager
#endif  // SRC_COMPONENTS_TIME_MANAGER_INCLUDE_TIME_MANAGER_MEDIA_MANAGER_H_
