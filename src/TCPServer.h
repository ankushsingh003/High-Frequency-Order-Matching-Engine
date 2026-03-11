#pragma once
#include <asio.hpp>
#include <iostream>
#include "Order.h"
#include "SPSCQueue.h"

using asio::ip::tcp;

class TCPServer {
public:
    TCPServer(asio::io_context& io_context, short port, SPSCQueue<Order>& orderQueue)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          orderQueue_(orderQueue) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](std::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::cout << "Client connected!" << std::endl;
                    
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    SPSCQueue<Order>& orderQueue_;
};
