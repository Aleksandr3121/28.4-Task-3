#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <string>
#include <chrono>
#include <deque>
#include <string_view>

using namespace std;

class Restaurant {
public:
    Restaurant() : countSuccessfulDeliveries(0) {}

    void StartJop() {
        thread waiter(&Restaurant::Waiter, this);
        thread kitchen(&Restaurant::Kitchen, this);
        thread courier(&Restaurant::Courier, this);
        waiter.detach();
        kitchen.detach();
        courier.detach();
        while (countSuccessfulDeliveries < 10) {
            this_thread::sleep_for(1s);
        }
    }

private:
    void Waiter() {
        while (countSuccessfulDeliveries < 10) {
            this_thread::sleep_for(chrono::seconds{5 + rand() % 5});
            Client client = {rand() % 1000, dishes[rand() % 5]};
            mCout.lock();
            cout << "An order has been received from a client " << client.id << ": " << client.order << endl;
            mCout.unlock();
            mClients.lock();
            clients.push_back(move(client));
            mClients.unlock();
        }
    }

    void Kitchen() {
        while (countSuccessfulDeliveries < 10) {
            while (clients.empty()) {
                this_thread::sleep_for(50ms);
            }
            Client client = move(clients.front());
            mCout.lock();
            cout << "The cooking of the dish " << client.order << " for the client " << client.id << " has begun"
                 << endl;
            mCout.unlock();
            mClients.lock();
            clients.pop_front();
            mClients.unlock();
            this_thread::sleep_for(chrono::seconds{5 + rand() % 10});
            mDeliveries.lock();
            deliveries.push_back(move(client));
            mDeliveries.unlock();
        }
    }

    void Courier() {
        while (countSuccessfulDeliveries < 10) {
            while (deliveries.empty()) {
                this_thread::sleep_for(50ms);
            }
            mDeliveries.lock();
            int countDeliveries = deliveries.size();
            deliveries.clear();
            mDeliveries.unlock();
            mCout.lock();
            cout << "Delivery started" << endl;
            mCout.unlock();
            this_thread::sleep_for(30s);
            mCout.lock();
            cout << "Delivery is completed" << endl;
            mCout.unlock();
            countSuccessfulDeliveries += countDeliveries;
        }
    }

    struct Client {
        int id;
        string_view order;
    };

    int countSuccessfulDeliveries;
    deque<Client> clients;
    mutex mClients;
    vector<Client> deliveries;
    mutex mDeliveries;
    static const vector<string> dishes;
    mutex mCout;
};

const vector<string> Restaurant::dishes = {"pizza", "soup", "steak", "salad", "sushi"};

int main() {
    Restaurant restaurant;
    restaurant.StartJop();
    return 0;
}
