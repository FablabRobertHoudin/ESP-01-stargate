#include "Strip.h"
    
    Strip::Strip(int len, String name) {
        _len = len;
        _name = name;
        data = (byte*)malloc(3*len);
        //Serial.println("new strip '"+_name+"'"); Serial.print("malloc: "); Serial.println((int)data);
      }

    void Strip::resize(int len) {
      data = (byte*)realloc(data, 3*len);
    }

    void Strip::fill(byte wR, byte wG, byte wB, int start) { // start=1: no zero start=0
      byte *ptr = data;
      Serial.println("strip '"+_name+"' filling");
      for (int i = 0; i < _len; i++) {
        *ptr++ = (byte) (wG * start / _len) & 0xFF;
        *ptr++ = (byte) (wR * start / _len) & 0xFF;
        *ptr++ = (byte) (wB * start / _len) & 0xFF;
        start++;
      }
    }
    
    void Strip::writeGRB(int pos, int offset) { // write to data !
      pos = 90 - pos; // compensation position
      while (pos<0) pos += LEN;
      while (pos >= LEN) pos -= LEN;
      //Serial.print("writeGRB "); Serial.println(pos);
      int l = _len - offset;
      int last = pos + l; // last position
      if (last < LEN) { // OK one write
        memcpy(GRB+(pos*3), data+(offset*3), l*3);
      } else {
        int n1 = LEN-pos;
        memcpy(GRB+(pos*3), data+(offset*3), n1*3);
        memcpy(GRB, data+((offset+n1)*3), (l-n1)*3);
      }
    }

