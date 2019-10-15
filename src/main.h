#ifndef CPPECHO_H
#define CPPECHO_H

class cppecho : public server::handler {
    void on_message(connection_ptr con, std::string msg) {
        con->write(msg);
    }
};

#endif // CPPECHO_H
