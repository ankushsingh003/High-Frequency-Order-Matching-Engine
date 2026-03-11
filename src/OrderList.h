#pragma once
#include "Order.h"


class OrderList {
public:
    OrderList() : head_(nullptr), tail_(nullptr) {}

    void push_back(Order* order) {
        order->prev = tail_;
        order->next = nullptr;
        if (tail_) {
            tail_->next = order;
        } else {
            head_ = order;
        }
        tail_ = order;
    }

    void remove(Order* order) {
        if (order->prev) {
            order->prev->next = order->next;
        } else {
            head_ = order->next;
        }

        if (order->next) {
            order->next->prev = order->prev;
        } else {
            tail_ = order->prev;
        }
    }

    void pop_front() {
        if (head_) {
            Order* next = head_->next;
            if (next) {
                next->prev = nullptr;
            } else {
                tail_ = nullptr;
            }
            head_ = next;
        }
    }

    Order* front() const { return head_; }
    bool empty() const { return head_ == nullptr; }

private:
    Order* head_;
    Order* tail_;
};
