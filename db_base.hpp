#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <mysql++.h>
namespace mysql_client {
class db_base  {
public:
    db_base() = default;
    ~db_base() {
        disconnect();
    }
public:
    void load_config(const char *db_ip, 
                     const char *db_name,
                     const char *db_user,
                     const char *db_password,
                     unsigned int db_port) {
        db_ip_ = db_ip;
        db_name_ = db_name;
        db_user_ = db_user;
        db_password_ = db_password;
        db_port_ = db_port;
    }
    void set_encode(const char *str) {
        set_encode_ = str;
    }
    bool load_no_query_sql(const char *sql) {
        if (!is_connected) {
            if (!connect()) {
                return false;
            }
        }
        try {
            db_query_->reset();
            int ret = db_query_->exec(sql);
            if (ret <= 0) {
                std::cerr << sql << " load failed." << std::endl;
                return false;
            }
            return true;
        }
        catch (mysqlpp::BadQuery &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
        catch (mysqlpp::BadConversion &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
        catch (std::exception &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
        return false;
    }
    bool load_query_sql(const char *sql, std::vector<std::string>&vec, int limit = -1) {
        vec.clear();
        if (!is_connected) {
            if (!connect()) {
                return false;
            }
        }
        try {
            db_query_->reset();
            *db_query_ << sql;
            mysqlpp::StoreQueryResult res = db_query_->store();
            if (res.size() <= 0) {
                std::cerr << sql << " load failed." << std::endl;
                return false;
            }
            int row_num = 0;
            int col_num = 0;
            for (mysqlpp::StoreQueryResult::iterator it = res.begin();it != res.end();++it) {
                row_num++;
                if (limit != -1 && row_num > limit) {
                    break;
                }
                mysqlpp::Row row(*it);
                col_num = row.size();
                for (int i = 0;i < col_num;i++) {
                    vec.emplace_back(row[i]);
                }
            }
            return true;
        }
        catch (mysqlpp::BadQuery &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
        catch (mysqlpp::BadConversion &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
        catch (std::exception &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
        return false;
    }
    bool commit() {
        return load_no_query_sql("commit;");
    }
    bool rollback() {
        return load_no_query_sql("rollback;");
    }
    bool connect() {
        bool succ = false;
        db_conn_ = std::unique_ptr<mysqlpp::Connection>(new mysqlpp::Connection(true));
        if (!db_conn_) {
            return false;
        }
        try {
            if (!db_conn_->connect(db_name_, db_ip_, db_user_, db_password_, db_port_)) {
                std::cerr << "db ip:" << db_ip_ << " db name:" << db_name_ << " db user:" << db_user_ << " db password:" << db_password_ << " db port:" << db_port_ << std::endl;
                return false;
            }
            db_query_ = std::unique_ptr<mysqlpp::Query>(new mysqlpp::Query(db_conn_->query()));
            if (!db_query_) {
                return false;
            }
            db_query_->reset();
            db_query_->exec("set autocommit = 1;");
            db_query_->exec(set_encode_);
            is_connected = true;
            succ = true;
        }
        catch (mysqlpp::BadQuery &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
        catch (mysqlpp::BadConversion &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
        catch (std::exception &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
        return succ;
    }
    void disconnect() {
        is_connected = false;
        try {
            if (!db_conn_ || !db_query_) {
                return;
            }
            db_query_->reset();
            db_query_->exec("commit");
            db_conn_->disconnect();
        }
        catch (mysqlpp::BadQuery &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
        catch (mysqlpp::BadConversion &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
        catch (std::exception &e) {
            std::cerr << "db base exception:" << e.what() << std::endl;
        }
    }
private:
    const char *db_ip_ = "192.168.5.131";
    const char *db_name_ = "test";
    const char *db_user_ = "mysql";
    const char *db_password_ = "123456";
    const char *set_encode_ = "SET NAMES UTF8;";
    unsigned int db_port_ = 3306;

private:
    std::unique_ptr<mysqlpp::Connection>db_conn_;
    std::unique_ptr<mysqlpp::Query>db_query_;
private:
    bool is_connected = false;
};
}