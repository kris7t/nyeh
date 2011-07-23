#include "Net.hxx"

struct PacketHeader {
    void fromNet() {
        len = be64toh(len);
    }
    void toNet() {
        len = htobe64(len);
    }

    uint8_t pid;
    uint64_t len;
} NOALIGN;

PacketReader::PacketReader(int id) {
    readers()[id] = this;
};

PacketReader& PacketReader::get(int id) {
    return *readers()[id];
}

std::map<int, PacketReader *>& PacketReader::readers() {
    static std::map<int, PacketReader *> vect;
    return vect;
}

void xmemcpy(void * dst_, const void * src_, size_t p, size_t size, size_t len) {
    size_t l;
    if (len+p <= size)
        l = len;
    else
        l = size-p;

    char * dst = static_cast<char *>(dst_);
    const char * src = static_cast<const char *>(src_);
    memcpy(dst, src+p, l);
    memcpy(dst+l, src, len-l);
}

NetSocket::NetSocket(const std::string & host, int port)
    : buf(NULL), bufl(0), bufp(0), read(0), size(sizeof(PacketHeader)), is_head(true) {

    addrinfo hints, * res, * p;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), boost::lexical_cast<std::string>(port).c_str(),
                    &hints, &res))
        throw std::string("getaddrinfo");
    p = res;
    for (;p; p = p->ai_next) {
        int fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd == -1) continue;
        if (connect(fd, p->ai_addr, p->ai_addrlen)) {
            close(fd);
            continue;
        }
        sock = fd;
        return;
    }
    throw std::string("no socket contactable");
}

NetSocket::NetSocket(int fd)
    : buf(NULL), bufl(0), bufp(0), read(0), size(sizeof(PacketHeader)), is_head(true) {
    sock = fd;
}

NetSocket::~NetSocket() {
    free(buf);
    close(sock);
}

Packet_ NetSocket::recv() {
    char tbuf[4096];
    while (true) {
        errno = 0;
        ssize_t ret = ::recv(sock, &tbuf, 4096, MSG_DONTWAIT);
        if (ret == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("recv");
            throw std::string("recv err.");
        }

        if (ret > 0) {
            size_t newsize = std::max(read+ret, size);
            if (bufl < newsize) {
                buf = static_cast<char *>(realloc(buf, newsize));
                memcpy(buf+bufl, buf, std::min(bufp, newsize-bufl));
                if (newsize - bufl < bufp)
                    memcpy(buf, buf + newsize - bufl, bufp-(newsize-bufl));

                // data before bufp
                // |a1a2a3a4[p b1]b2b3b4|         bef
                // |a1a2a3a4[p b1]b2b3b4[l e0]e1e2|
                // |a1a2a3a4[p b1]b2b3b4[l a1]a2a3|
                // |a4a2a3a4[p b1]b2b3b4[l a1]a2a3|
                // |a4e0e1e2[p b1]b2b3b4[l a1]a2a3| aft

                // |a1a2[p b1]b2|
                // |a1a2[p b1]b2[l e1]e2e3e4e5|
                // |a1a2[p b1]b2[l a1]a2e3e4e5|

                bufl = read+ret;
            }

            int st = (bufp+read) % bufl;
            int l = std::min(static_cast<size_t>(ret), bufl - st);
            memcpy(buf + st, tbuf, l);
            memcpy(buf, tbuf + l, ret-l);
            read += ret;
        }

        while (read >= size) {
            if (is_head) {
                PacketHeader hdr;
                xmemcpy(&hdr, buf, bufp, bufl, sizeof(PacketHeader));
                hdr.fromNet();
                id = hdr.pid;
                bufp = (bufp + size) % bufl;
                read -= size;
                size = hdr.len;
                is_head = false;
            } else {
                CharVect ptr(size);
                xmemcpy((char *) &ptr[0], buf, bufp, bufl, size);
                bufp = (bufp + size) % bufl;
                read -= size;
                size = sizeof(PacketHeader);
                is_head = true;
                PacketReader& x = PacketReader::get(id);
                return x.read(ptr);
            }
        }
        if (ret <= 0) return Packet_();
    }
}

void NetSocket::send(const Packet& pak) {
    PacketHeader hdr;
    hdr.pid = pak.pid();
    Char_ ptr = pak.write();
    hdr.len = ptr->size();
    hdr.toNet();

    ::send(sock, &hdr, sizeof(PacketHeader), 0);
    ::send(sock, &(*ptr)[0], ptr->size(), 0);
}

NetServer::NetServer(int port) {
    addrinfo hints, * res, * p;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(NULL, boost::lexical_cast<std::string>(port).c_str(), &hints, &res))
        throw std::string("getaddrinfo");
    p = res;
    for (;p; p = p->ai_next) {
        int fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd == -1) continue;
        if (bind(fd, p->ai_addr, p->ai_addrlen)) {
            close(fd);
            continue;
        }
        if (listen(fd, 1)) {
            close(fd);
            continue;
        }
        if (fcntl(fd, F_SETFL, O_NONBLOCK, 1)) {
            close(fd);
            continue;
        }
        sock = fd;
        return;
    }
    throw std::string("no socket listenable");
}

NetServer::~NetServer() {
    close(sock);
}

NetSocket_ NetServer::accept() const {
    errno = 0;
    sockaddr sa;
    socklen_t sl = sizeof(sockaddr);
    int newfd = ::accept(sock, &sa, &sl);
    if (newfd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return NetSocket_();
        throw std::string("accept fail");
    }
    return NetSocket_(new NetSocket(newfd));
}

//
NetConnection::NetConnection(int port) : srv(new NetServer(port)) {}
NetConnection::NetConnection(const std::string & host, int port)
    : sock_(new NetSocket(host, port)), srv(NULL) {}
NetConnection::~NetConnection() {
    delete srv;
}
