#ifndef Strip_h
#define Strip_h

#include <Arduino.h>

#define LEN 60
extern byte GRB[3 * LEN];

class Strip
{
  private:
    int _len; // length of strip
    String _name; // name
    static byte* _GRB;

  public:
    byte *data;
    Strip(int len, String name);
    //~Strip();
    void resize(int len);
    void fill(byte wR, byte wG, byte wB, int start=1);
    void writeGRB(int pos, int offset = 0);

};

#endif

