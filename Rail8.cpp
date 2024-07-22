#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <map>

using namespace std;

const int GROUP_DISCOUNT_THRESHOLD = 5; // Number of passengers for group discount
const double GROUP_DISCOUNT_PERCENTAGE = 0.10; // 10% discount

struct Passenger {
    string name;
    int age;
    char gender;
    string berthPreference;
    int seatNumber;
    string mealPreference;
    string shareWithGender; // Gender preference for seat sharing
    string emergencyContact;
    string berthType; // Add berthType to store the berth type
};

struct Ticket {
    int ticketId;
    string pnr;
    Passenger passenger;
    bool insurance;
};

class User {
public:
    string username;
    string password;
    vector<int> bookedTickets;

    User(string u, string p) : username(u), password(p) {}
};

class ReservationSystem {
private:
    vector<User> users;
    vector<Ticket> tickets;
    int ticketID = 1;
    queue<int> lowerBerthSeats;
    queue<int> middleBerthSeats;
    queue<int> upperBerthSeats;
    queue<int> racSeats;
    queue<int> waitingListSeats;
    map<int, int> waitlistMap;

    string generatePNR() {
        static int pnrCounter = 1;
        return "PNR" + to_string(pnrCounter++);
    }

public:
    ReservationSystem() {
        // Initialize seat queues
        for (int i = 1; i <= 10; ++i) lowerBerthSeats.push(i);
        for (int i = 11; i <= 20; ++i) middleBerthSeats.push(i);
        for (int i = 21; i <= 30; ++i) upperBerthSeats.push(i);
        for (int i = 31; i <= 35; ++i) racSeats.push(i);
        for (int i = 36; i <= 40; ++i) waitingListSeats.push(i);
    }

    void registerUser() {
        string username, password;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
        users.push_back(User(username, password));
        cout << "User registered successfully!" << endl;
    }

    bool loginUser(string username, string password) {
        for (const User &user : users) {
            if (user.username == username && user.password == password) {
                return true;
            }
        }
        return false;
    }

    void bookTicket(string username) {
        vector<Passenger> passengers;
        int numPassengers;
        cout << "Enter number of passengers: ";
        cin >> numPassengers;

        for (int i = 0; i < numPassengers; ++i) {
            Passenger passenger;
            cout << "Enter passenger " << i + 1 << " details:" << endl;
            cout << "Name: ";
            cin.ignore();
            getline(cin, passenger.name);
            cout << "Age: ";
            cin >> passenger.age;
            cout << "Gender (M/F): ";
            cin >> passenger.gender;
            passenger.gender = toupper(passenger.gender);
            cout << "Berth Preference (LB/MB/UB): ";
            cin >> passenger.berthPreference;
            transform(passenger.berthPreference.begin(), passenger.berthPreference.end(), passenger.berthPreference.begin(), ::toupper);

            // Meal Preference
            cout << "Meal Preference (Veg/Non-Veg): ";
            cin >> passenger.mealPreference;
            transform(passenger.mealPreference.begin(), passenger.mealPreference.end(), passenger.mealPreference.begin(), ::toupper);

            // Seat Sharing
            cout << "Do you want to share seat with (M/F): ";
            cin >> passenger.shareWithGender;

            // Emergency Contact
            cout << "Enter Emergency Contact: ";
            cin.ignore();
            getline(cin, passenger.emergencyContact);

            if (passenger.age < 5) {
                cout << "No need for ticket reservation for age below 5." << endl;
                return;
            }

            passengers.push_back(passenger);
        }

        double totalPrice = numPassengers * 100.0; // Assume each ticket costs 100 for simplicity
        if (numPassengers > GROUP_DISCOUNT_THRESHOLD) {
            totalPrice *= (1 - GROUP_DISCOUNT_PERCENTAGE);
            cout << "Group discount applied! Total price: " << totalPrice << endl;
        } else {
            cout << "Total price: " << totalPrice << endl;
        }

        for (auto &passenger : passengers) {
            // Insurance option
            char insOption;
            cout << "Do you want travel insurance? (Y/N): ";
            cin >> insOption;
            bool insurance = (toupper(insOption) == 'Y');

            // Assign seat based on berth preference and age
            if (passenger.age >= 60) {
                if (!lowerBerthSeats.empty()) {
                    passenger.seatNumber = lowerBerthSeats.front();
                    passenger.berthType = "Lower Berth";
                    lowerBerthSeats.pop();
                    cout << "Allocated lower berth for senior citizen." << endl;
                } else {
                    cout << "No lower berth available for senior citizen." << endl;
                    return;
                }
            } else if (passenger.gender == 'F' && passenger.shareWithGender == "F") {
                if (!lowerBerthSeats.empty()) {
                    passenger.seatNumber = lowerBerthSeats.front();
                    passenger.berthType = "Lower Berth";
                    lowerBerthSeats.pop();
                    cout << "Allocated lower berth for female passenger." << endl;
                } else {
                    cout << "No female-only lower berth available." << endl;
                    return;
                }
            } else {
                if (passenger.berthPreference == "LB" && !lowerBerthSeats.empty()) {
                    passenger.seatNumber = lowerBerthSeats.front();
                    passenger.berthType = "Lower Berth";
                    lowerBerthSeats.pop();
                } else if (passenger.berthPreference == "MB" && !middleBerthSeats.empty()) {
                    passenger.seatNumber = middleBerthSeats.front();
                    passenger.berthType = "Middle Berth";
                    middleBerthSeats.pop();
                } else if (passenger.berthPreference == "UB" && !upperBerthSeats.empty()) {
                    passenger.seatNumber = upperBerthSeats.front();
                    passenger.berthType = "Upper Berth";
                    upperBerthSeats.pop();
                } else {
                    cout << "Preferred berth not available. Allocating LB." << endl;
                    if (!lowerBerthSeats.empty()) {
                        passenger.seatNumber = lowerBerthSeats.front();
                        passenger.berthType = "Lower Berth";
                        lowerBerthSeats.pop();
                    } else if (!middleBerthSeats.empty()) {
                        passenger.seatNumber = middleBerthSeats.front();
                        passenger.berthType = "Middle Berth";
                        middleBerthSeats.pop();
                    } else if (!upperBerthSeats.empty()) {
                        passenger.seatNumber = upperBerthSeats.front();
                        passenger.berthType = "Upper Berth";
                        upperBerthSeats.pop();
                    } else if (!racSeats.empty()) {
                        passenger.seatNumber = racSeats.front();
                        passenger.berthType = "RAC";
                        racSeats.pop();
                    } else if (!waitingListSeats.empty()) {
                        passenger.seatNumber = waitingListSeats.front();
                        passenger.berthType = "Waiting List";
                        waitingListSeats.pop();
                        waitlistMap[passenger.seatNumber] = ticketID;
                    } else {
                        cout << "No seats available." << endl;
                        return;
                    }
                }
            }

            string pnr = generatePNR();
            Ticket t = {ticketID++, pnr, passenger, insurance};
            tickets.push_back(t);
            for (User &user : users) {
                if (user.username == username) {
                    user.bookedTickets.push_back(t.ticketId);
                    break;
                }
            }
            cout << "Ticket booked successfully, Ticket ID: " << t.ticketId << ", PNR: " << t.pnr << endl;
        }
    }

    void cancelTicket(int ticketID) {
        auto it = find_if(tickets.begin(), tickets.end(), [ticketID](const Ticket &t) { return t.ticketId == ticketID; });
        if (it != tickets.end()) {
            tickets.erase(it);
            cout << "Ticket cancelled successfully." << endl;
        } else {
            cout << "Ticket ID not found." << endl;
        }
    }

    void printBookedTickets() {
        cout << "Booked Tickets:" << endl;
        for (const Ticket &t : tickets) {
            cout << "Ticket ID: " << t.ticketId << ", PNR: " << t.pnr << endl;
        }
    }

    void printAvailability() {
        cout << "Available Lower Berth Seats: " << lowerBerthSeats.size() << endl;
        cout << "Available Middle Berth Seats: " << middleBerthSeats.size() << endl;
        cout << "Available Upper Berth Seats: " << upperBerthSeats.size() << endl;
        cout << "Available RAC Seats: " << racSeats.size() << endl;
        cout << "Available Waiting List Seats: " << waitingListSeats.size() << endl;
    }

    void searchTicketById(int ticketID) {
        auto it = find_if(tickets.begin(), tickets.end(), [ticketID](const Ticket &t) { return t.ticketId == ticketID; });
        if (it != tickets.end()) {
            cout << "Ticket Found - Ticket ID: " << it->ticketId << ", PNR: " << it->pnr << endl;
        } else {
            cout << "Ticket ID not found." << endl;
        }
    }

    void searchTicketsByName(string name) {
        cout << "Tickets for passenger: " << name << endl;
        for (const Ticket &t : tickets) {
            if (t.passenger.name == name) {
                cout << "Ticket ID: " << t.ticketId << ", PNR: " << t.pnr << endl;
            }
        }
    }

    void trackTrain() {
        cout << "Train is on time." << endl;
    }

    void printMealPreferences() {
        cout << "Meal Preferences:" << endl;
        for (const Ticket &t : tickets) {
            cout << "Passenger Name: " << t.passenger.name << ", Meal Preference: " << t.passenger.mealPreference << endl;
        }
    }

    void printTicketDetails(int ticketID) {
        auto it = find_if(tickets.begin(), tickets.end(), [ticketID](const Ticket &t) { return t.ticketId == ticketID; });
        if (it != tickets.end()) {
            cout << "Ticket Details - Ticket ID: " << it->ticketId << ", PNR: " << it->pnr << endl;
            cout << "Passenger Name: " << it->passenger.name << ", Age: " << it->passenger.age << ", Gender: " << it->passenger.gender << endl;
            cout << "Seat Number: " << it->passenger.seatNumber << ", Berth Type: " << it->passenger.berthType << endl;
            cout << "Meal Preference: " << it->passenger.mealPreference << ", Emergency Contact: " << it->passenger.emergencyContact << endl;
            cout << "Insurance: " << (it->insurance ? "Yes" : "No") << endl;
        } else {
            cout << "Ticket ID not found." << endl;
        }
    }
};

int main() {
    ReservationSystem system;
    bool userWish = true;
    int choice;
    string username, password;
    string name;

    do {
        cout << "\nRailway Reservation System Menu:\n";
        cout << "1. Register User\n";
        cout << "2. Login User\n";
        cout << "3. Book Ticket\n";
        cout << "4. Cancel Ticket\n";
        cout << "5. Print Booked Tickets\n";
        cout << "6. Print Available Tickets\n";
        cout << "7. Search Ticket by ID\n";
        cout << "8. Search Tickets by Name\n";
        cout << "9. Track Train\n";
        cout << "10. Print Meal Preferences\n";
        cout << "11. Print Ticket Details\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            system.registerUser();
            break;
        case 2:
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            if (system.loginUser(username, password)) {
                cout << "Login successful!" << endl;
            } else {
                cout << "Invalid username or password." << endl;
            }
            break;
        case 3:
            cout << "Enter username: ";
            cin >> username;
            system.bookTicket(username);
            break;
        case 4:
            int ID;
            cout << "Enter Ticket ID: ";
            cin >> ID;
            system.cancelTicket(ID);
            break;
        case 5:
            system.printBookedTickets();
            break;
        case 6:
            system.printAvailability();
            break;
        case 7:
            cout << "Enter Ticket ID: ";
            cin >> ID;
            system.searchTicketById(ID);
            break;
        case 8:
            cout << "Enter passenger name: ";
            cin.ignore();
            getline(cin, name);
            system.searchTicketsByName(name);
            break;
        case 9:
            system.trackTrain();
            break;
        case 10:
            system.printMealPreferences();
            break;
        case 11:
            cout << "Enter Ticket ID: ";
            cin >> ID;
            system.printTicketDetails(ID);
            break;
        case 0:
            userWish = false;
            break;
        default:
            cout << "Invalid choice, please try again." << endl;
            break;
        }
    } while (userWish);

    return 0;
}
