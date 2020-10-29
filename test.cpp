#include "db_client.hpp"
int main() {
    mysql_client::db_client my_db_client;
    if (!my_db_client.init()) {
        return -1;
    }
    std::string name;
    if (my_db_client.get_name(1, name)) {
        std::cout << "name = " << name << std::endl;
    }
    return 0;
}