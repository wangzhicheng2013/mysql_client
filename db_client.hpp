#include "db_base.hpp"
namespace mysql_client {
struct db_config {
    const char *db_ip_ = "192.168.5.131";
    const char *db_name_ = "test";
    const char *db_user_ = "mysql";
    const char *db_password_ = "123456";
    const char *set_encode_ = "SET NAMES UTF8;";
    unsigned int db_port_ = 3306;
};
class db_client {
public:
    bool init() {
        base_.load_config(config_.db_ip_, 
                          config_.db_name_,
                          config_.db_user_, 
                          config_.db_password_,
                          config_.db_port_);
        return base_.connect();
    }
    void set_config(const db_config &config) {
        config_ = config;
    }
    bool get_name(int id, std::string &name) {
        name.clear();
        snprintf(sql_, sizeof(sql_), "select name from name where id = %d", id);
        if (base_.load_query_sql(sql_, query_res_) && ("NULL" != query_res_[0])) {
            name = query_res_[0];
            return true;
        }
        return false;
    }
private:
    db_base base_;
    db_config config_;
private:
    char sql_[128] = "";
    std::vector<std::string>query_res_;
};
}