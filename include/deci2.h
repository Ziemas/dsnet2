#ifndef DECI2_H_
#define DECI2_H_

#define BIT(b) (1 << b)

enum DESC_TYPE {
    DESC_KBD = BIT (0),
    DESC_DEV = BIT (1),
    DESC_LISTEN = BIT (2),
    DESC_ACCEPT = BIT (3),
    DESC_CONNECT = BIT (4),
    DESC_COMPORT = BIT (5),
    DESC_NETDEV = BIT (6),
};

#define DESC_WRITEABLE (DESC_DEV | DESC_ACCEPT | DESC_CONNECT | DESC_COMPORT | DESC_NETDEV)

enum DS_PROTO {
    PROTO_ANY = 0,
    PROTO_DCMP = 1,
    PROTO_I0TTYP = 272,
    PROTO_I1TTYP = 273,
    PROTO_I2TTYP = 274,
    PROTO_I3TTYP = 275,
    PROTO_I4TTYP = 276,
    PROTO_I5TTYP = 277,
    PROTO_I6TTYP = 278,
    PROTO_I7TTYP = 279,
    PROTO_I8TTYP = 280,
    PROTO_I9TTYP = 281,
    PROTO_IKTTYP = 287,
    PROTO_DRFP0 = 288,
    PROTO_DRFP1 = 289,
    PROTO_DRFP2 = 290,
    PROTO_DRFP3 = 291,
    PROTO_DRFP4 = 292,
    PROTO_ISDBGP = 304,
    PROTO_ITDBGP = 320,
    PROTO_ILOADP = 336,
    PROTO_E0TTYP = 528,
    PROTO_E1TTYP = 529,
    PROTO_E2TTYP = 530,
    PROTO_E3TTYP = 531,
    PROTO_E4TTYP = 532,
    PROTO_E5TTYP = 533,
    PROTO_E6TTYP = 534,
    PROTO_E7TTYP = 535,
    PROTO_E8TTYP = 536,
    PROTO_E9TTYP = 537,
    PROTO_EKTTYP = 543,
    PROTO_ESDBGP = 560,
    PROTO_ETDBGP = 576,
    PROTO_ELOADP = 592,
    PROTO_NETMP = 1024,
    PROTO_NTTYP = 1040,
    PROTO_DTTYP = 1041,
    PROTO_CTTYP = 1042,
    PROTO_STTYP = 1043,
    PROTO_SYSTEMP = 1280,
    PROTO_IP = 1296,
};

struct deci2_hdr {
    unsigned short length;
    unsigned short reserved;
    unsigned short protocol;
    unsigned char source;
    unsigned char destination;
};

enum DCMP_TYPE {
    DCMP_TYPE_CONNECT = 0,
    DCMP_TYPE_ECHO = 1,
    DCMP_TYPE_STATUS = 2,
    DCMP_TYPE_ERROR = 3,
};

struct dcmp_hdr {
    unsigned char type;
    unsigned char code;
    unsigned short unused;
};

enum DCMP_CONNECT {
    DCMP_CODE_CONNECT = 0,
    DCMP_CODE_CONNECTR = 1,
    DCMP_CODE_DISCONNECT = 2,
    DCMP_CODE_DISCONNECTR = 3,
};

enum DCMP_ERR {
    DCMP_ERR_INVALDEST = 1,
    DCMP_ERR_ALREADYCON = 2,
    DCMP_ERR_NOTCONNECT = 3,
};

struct dcmp_connect {
    struct dcmp_hdr hdr;
    unsigned char result;
    unsigned char unused[3];
};

enum DCMP_ECHO {
    DCMP_CODE_ECHO = 0,
    DCMP_CODE_ECHOR = 1,
};

struct dcmp_echo {
    struct dcmp_hdr hdr;
    unsigned short identifier;
    unsigned short sequence;
    unsigned char data[0];
};

enum DCMP_ERROR {
    DCMP_CODE_NOROUTE = 0,
    DCMP_CODE_NOPROTO = 1,
    DCMP_CODE_LOCKED = 2,
    DCMP_CODE_NOSPACE = 3,
    DCMP_CODE_INVALHEAD = 4,
    DCMP_CODE_NOCONNECT = 5,
};

struct dcmp_error {
    struct dcmp_hdr hdr;
    struct dcmp_hdr orig_hdr;
    unsigned char bytes[16];
};

#endif // DECI2_H_
