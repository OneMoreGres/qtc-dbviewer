// $Id: DbTypes.h 20 2006-10-10 08:24:36Z tb $
// Copyright 2006 Timo Bingmann
//
// Conversion tables to display human-readable names for database column types.
//

#ifndef _DbTypes_H_
#define _DbTypes_H_

#include <QObject>

class DbTypes {
  public:
    static inline QString getName (QString driver, int t) {
      if (driver == QStringLiteral ("QMYSQL") || driver == QStringLiteral ("QMYSQL3")) {
        return getNameMYSQL (t);
      }
      if (driver  == QStringLiteral ("QPSQL") || driver == QStringLiteral ("PSQL7")) {
        return getNamePSQL (t);
      }
      if (driver  == QStringLiteral ("QSQLITE") || driver == QStringLiteral ("QSQLITE2")) {
        return getNameSQLITE (t);
      }

      return QObject::tr ("Type %1").arg (t);
    }

  protected:
    static inline QString getNameSQLITE (int t) {
      switch (t) {
        case 1:   return QStringLiteral ("integer");
        case 2:   return QStringLiteral ("float");
        case 3:   return QStringLiteral ("text");
        case 4:   return QStringLiteral ("blob");
        case 5:   return QStringLiteral ("null");

        default:
          return QObject::tr ("Type %1").arg (t);
      }
    }

    static inline QString getNameMYSQL (int t) {
      switch (t) {
        case 0:   return QStringLiteral ("decimal");
        case 1:   return QStringLiteral ("tiny");
        case 2:   return QStringLiteral ("short");
        case 3:   return QStringLiteral ("integer");
        case 4:   return QStringLiteral ("float");
        case 5:   return QStringLiteral ("double");
        case 6:   return QStringLiteral ("null");
        case 7:   return QStringLiteral ("timestamp");
        case 8:   return QStringLiteral ("longlong");
        case 9:   return QStringLiteral ("int24");
        case 10:  return QStringLiteral ("date");
        case 11:  return QStringLiteral ("time");
        case 12:  return QStringLiteral ("datetime");
        case 13:  return QStringLiteral ("year");
        case 14:  return QStringLiteral ("newdate");
        case 247: return QStringLiteral ("enum");
        case 248: return QStringLiteral ("set");
        case 249: return QStringLiteral ("tinytext");
        case 250: return QStringLiteral ("mediumtext");
        case 251: return QStringLiteral ("longtext");
        case 252: return QStringLiteral ("text");
        case 253: return QStringLiteral ("varchar");
        case 254: return QStringLiteral ("char");

        default:
          return QObject::tr ("Type %1").arg (t);
      }
    }

    static inline QString getNamePSQL (int t) {
      switch (t) {
        case 16:    return QStringLiteral ("bool");
        case 17:    return QStringLiteral ("bytea");
        case 18:    return QStringLiteral ("char");
        case 19:    return QStringLiteral ("name");
        case 20:    return QStringLiteral ("int8");
        case 21:  return QStringLiteral ("int2");
        case 22:  return QStringLiteral ("int2vector");
        case 23:  return QStringLiteral ("int4");
        case 24:  return QStringLiteral ("regproc");
        case 25:  return QStringLiteral ("text");
        case 26:  return QStringLiteral ("oid");
        case 27:  return QStringLiteral ("tid");
        case 28:  return QStringLiteral ("xid");
        case 29:  return QStringLiteral ("cid");
        case 30:  return QStringLiteral ("oidvector");
        case 71:  return QStringLiteral ("pg_type");
        case 75:  return QStringLiteral ("pg_attribute");
        case 81:  return QStringLiteral ("pg_proc");
        case 83:  return QStringLiteral ("pg_class");
        case 86:  return QStringLiteral ("pg_shadow");
        case 87:  return QStringLiteral ("pg_group");
        case 88:  return QStringLiteral ("pg_database");
        case 90:  return QStringLiteral ("pg_tablespace");
        case 210: return QStringLiteral ("smgr");
        case 600: return QStringLiteral ("point");
        case 601: return QStringLiteral ("lseg");
        case 602: return QStringLiteral ("path");
        case 603: return QStringLiteral ("box");
        case 604: return QStringLiteral ("polygon");
        case 628: return QStringLiteral ("line");
        case 629: return QStringLiteral ("_line");
        case 650: return QStringLiteral ("cidr");
        case 651: return QStringLiteral ("_cidr");
        case 700: return QStringLiteral ("float4");
        case 701: return QStringLiteral ("float8");
        case 702: return QStringLiteral ("abstime");
        case 703: return QStringLiteral ("reltime");
        case 704: return QStringLiteral ("tinterval");
        case 705: return QStringLiteral ("unknown");
        case 718: return QStringLiteral ("circle");
        case 719: return QStringLiteral ("_circle");
        case 790: return QStringLiteral ("money");
        case 791: return QStringLiteral ("_money");
        case 829: return QStringLiteral ("macaddr");
        case 869: return QStringLiteral ("inet");
        case 1000:  return QStringLiteral ("_bool");
        case 1001:  return QStringLiteral ("_bytea");
        case 1002:  return QStringLiteral ("_char");
        case 1003:  return QStringLiteral ("_name");
        case 1005:  return QStringLiteral ("_int2");
        case 1006:  return QStringLiteral ("_int2vector");
        case 1007:  return QStringLiteral ("_int4");
        case 1008:  return QStringLiteral ("_regproc");
        case 1009:  return QStringLiteral ("_text");
        case 1010:  return QStringLiteral ("_tid");
        case 1011:  return QStringLiteral ("_xid");
        case 1012:  return QStringLiteral ("_cid");
        case 1013:  return QStringLiteral ("_oidvector");
        case 1014:  return QStringLiteral ("_bpchar");
        case 1015:  return QStringLiteral ("_varchar");
        case 1016:  return QStringLiteral ("_int8");
        case 1017:  return QStringLiteral ("_point");
        case 1018:  return QStringLiteral ("_lseg");
        case 1019:  return QStringLiteral ("_path");
        case 1020:  return QStringLiteral ("_box");
        case 1021:  return QStringLiteral ("_float4");
        case 1022:  return QStringLiteral ("_float8");
        case 1023:  return QStringLiteral ("_abstime");
        case 1024:  return QStringLiteral ("_reltime");
        case 1025:  return QStringLiteral ("_tinterval");
        case 1027:  return QStringLiteral ("_polygon");
        case 1028:  return QStringLiteral ("_oid");
        case 1033:  return QStringLiteral ("aclitem");
        case 1034:  return QStringLiteral ("_aclitem");
        case 1040:  return QStringLiteral ("_macaddr");
        case 1041:  return QStringLiteral ("_inet");
        case 1042:  return QStringLiteral ("bpchar");
        case 1043:  return QStringLiteral ("varchar");
        case 1082:  return QStringLiteral ("date");
        case 1083:  return QStringLiteral ("time");
        case 1114:  return QStringLiteral ("timestamp");
        case 1115:  return QStringLiteral ("_timestamp");
        case 1182:  return QStringLiteral ("_date");
        case 1183:  return QStringLiteral ("_time");
        case 1184:  return QStringLiteral ("timestamptz");
        case 1185:  return QStringLiteral ("_timestamptz");
        case 1186:  return QStringLiteral ("interval");
        case 1187:  return QStringLiteral ("_interval");
        case 1231:  return QStringLiteral ("_numeric");
        case 1266:  return QStringLiteral ("timetz");
        case 1270:  return QStringLiteral ("_timetz");
        case 1560:  return QStringLiteral ("bit");
        case 1561:  return QStringLiteral ("_bit");
        case 1562:  return QStringLiteral ("varbit");
        case 1563:  return QStringLiteral ("_varbit");
        case 1700:  return QStringLiteral ("numeric");
        case 1790:  return QStringLiteral ("refcursor");
        case 2201:  return QStringLiteral ("_refcursor");
        case 2202:  return QStringLiteral ("regprocedure");
        case 2203:  return QStringLiteral ("regoper");
        case 2204:  return QStringLiteral ("regoperator");
        case 2205:  return QStringLiteral ("regclass");
        case 2206:  return QStringLiteral ("regtype");
        case 2207:  return QStringLiteral ("_regprocedure");
        case 2208:  return QStringLiteral ("_regoper");
        case 2209:  return QStringLiteral ("_regoperator");
        case 2210:  return QStringLiteral ("_regclass");
        case 2211:  return QStringLiteral ("_regtype");
        case 2249:  return QStringLiteral ("record");
        case 2275:  return QStringLiteral ("cstring");
        case 2276:  return QStringLiteral ("any");
        case 2277:  return QStringLiteral ("anyarray");
        case 2278:  return QStringLiteral ("void");
        case 2279:  return QStringLiteral ("trigger");
        case 2280:  return QStringLiteral ("language_handler");
        case 2281:  return QStringLiteral ("internal");
        case 2282:  return QStringLiteral ("opaque");
        case 2283:  return QStringLiteral ("anyelement");

        default:
          return QObject::tr ("Type %1").arg (t);
      }
    }
};

#endif // _DbTypes_H_
