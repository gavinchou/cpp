#ifndef _MD5_H
#define _MD5_H

//#pragma warning(disable:4786)

#include <string>
#include <vector>

using namespace std;

/*!
 * Manage MD5.
 */
class CMD5
{
  public:
  std::vector<unsigned char> get_hex();

  private:
#define uint8  unsigned char
#define uint32 unsigned long int

    struct md5_context
    {
      uint32 total[2];
      uint32 state[4];
      uint8 buffer[64];
    };

    void md5_starts( struct md5_context *ctx );
    void md5_process( struct md5_context *ctx, uint8 data[64] );
    void md5_update( struct md5_context *ctx, uint8 *input, uint32 length );
    void md5_finish( struct md5_context *ctx, uint8 digest[16] );

  public:
    //! construct a CMD5 from any buffer
    void GenerateMD5(unsigned char* buffer,int bufferlen);

    //! construct a CMD5
    CMD5();

    //! construct a md5src from char *
    CMD5(const char * md5src);

    //! construct a CMD5 from a 16 bytes md5
    CMD5(unsigned long* md5src);

    //! add a other md5
    CMD5 operator +(CMD5 adder);

    //! just if equal
    bool operator ==(CMD5 cmper);

    //! give the value from equer
    // void operator =(CMD5 equer);

    //! to a string
    string ToString();

    unsigned long m_data[4];
};
#endif /* md5.h */


/*
 * RFC 1321 compliant MD5 implementation,
 * by Christophe Devine <devine@cr0.net>;
 * this program is licensed under the GPL.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define GET_UINT32(n,b,i)                                       \
{                                                               \
  (n) = (uint32) ((uint8 *) b)[(i)]                           \
    | (((uint32) ((uint8 *) b)[(i)+1]) <<  8)                 \
    | (((uint32) ((uint8 *) b)[(i)+2]) << 16)                 \
    | (((uint32) ((uint8 *) b)[(i)+3]) << 24);                \
}

#define PUT_UINT32(n,b,i)                                       \
{                                                               \
  (((uint8 *) b)[(i)]  ) = (uint8) (((n)      ) & 0xFF);      \
  (((uint8 *) b)[(i)+1]) = (uint8) (((n) >>  8) & 0xFF);      \
  (((uint8 *) b)[(i)+2]) = (uint8) (((n) >> 16) & 0xFF);      \
  (((uint8 *) b)[(i)+3]) = (uint8) (((n) >> 24) & 0xFF);      \
}

//extern pthread_mutex_t mutexMemory;

void CMD5::md5_starts( struct md5_context *ctx )
{
  ctx->total[0] = 0;
  ctx->total[1] = 0;
  ctx->state[0] = 0x67452301;
  ctx->state[1] = 0xEFCDAB89;
  ctx->state[2] = 0x98BADCFE;
  ctx->state[3] = 0x10325476;
}

void CMD5::md5_process( struct md5_context *ctx, uint8 data[64] )
{
  uint32 A, B, C, D, X[16];

  GET_UINT32( X[0],  data,  0 );
  GET_UINT32( X[1],  data,  4 );
  GET_UINT32( X[2],  data,  8 );
  GET_UINT32( X[3],  data, 12 );
  GET_UINT32( X[4],  data, 16 );
  GET_UINT32( X[5],  data, 20 );
  GET_UINT32( X[6],  data, 24 );
  GET_UINT32( X[7],  data, 28 );
  GET_UINT32( X[8],  data, 32 );
  GET_UINT32( X[9],  data, 36 );
  GET_UINT32( X[10], data, 40 );
  GET_UINT32( X[11], data, 44 );
  GET_UINT32( X[12], data, 48 );
  GET_UINT32( X[13], data, 52 );
  GET_UINT32( X[14], data, 56 );
  GET_UINT32( X[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define P(a,b,c,d,k,s,t)                                \
  {                                                       \
    a += F(b,c,d) + X[k] + t; a = S(a,s) + b;           \
  }

  A = ctx->state[0];
  B = ctx->state[1];
  C = ctx->state[2];
  D = ctx->state[3];

#define F(x,y,z) (z ^ (x & (y ^ z)))

  P( A, B, C, D,  0,  7, 0xD76AA478 );
  P( D, A, B, C,  1, 12, 0xE8C7B756 );
  P( C, D, A, B,  2, 17, 0x242070DB );
  P( B, C, D, A,  3, 22, 0xC1BDCEEE );
  P( A, B, C, D,  4,  7, 0xF57C0FAF );
  P( D, A, B, C,  5, 12, 0x4787C62A );
  P( C, D, A, B,  6, 17, 0xA8304613 );
  P( B, C, D, A,  7, 22, 0xFD469501 );
  P( A, B, C, D,  8,  7, 0x698098D8 );
  P( D, A, B, C,  9, 12, 0x8B44F7AF );
  P( C, D, A, B, 10, 17, 0xFFFF5BB1 );
  P( B, C, D, A, 11, 22, 0x895CD7BE );
  P( A, B, C, D, 12,  7, 0x6B901122 );
  P( D, A, B, C, 13, 12, 0xFD987193 );
  P( C, D, A, B, 14, 17, 0xA679438E );
  P( B, C, D, A, 15, 22, 0x49B40821 );

#undef F

#define F(x,y,z) (y ^ (z & (x ^ y)))

  P( A, B, C, D,  1,  5, 0xF61E2562 );
  P( D, A, B, C,  6,  9, 0xC040B340 );
  P( C, D, A, B, 11, 14, 0x265E5A51 );
  P( B, C, D, A,  0, 20, 0xE9B6C7AA );
  P( A, B, C, D,  5,  5, 0xD62F105D );
  P( D, A, B, C, 10,  9, 0x02441453 );
  P( C, D, A, B, 15, 14, 0xD8A1E681 );
  P( B, C, D, A,  4, 20, 0xE7D3FBC8 );
  P( A, B, C, D,  9,  5, 0x21E1CDE6 );
  P( D, A, B, C, 14,  9, 0xC33707D6 );
  P( C, D, A, B,  3, 14, 0xF4D50D87 );
  P( B, C, D, A,  8, 20, 0x455A14ED );
  P( A, B, C, D, 13,  5, 0xA9E3E905 );
  P( D, A, B, C,  2,  9, 0xFCEFA3F8 );
  P( C, D, A, B,  7, 14, 0x676F02D9 );
  P( B, C, D, A, 12, 20, 0x8D2A4C8A );

#undef F

#define F(x,y,z) (x ^ y ^ z)

  P( A, B, C, D,  5,  4, 0xFFFA3942 );
  P( D, A, B, C,  8, 11, 0x8771F681 );
  P( C, D, A, B, 11, 16, 0x6D9D6122 );
  P( B, C, D, A, 14, 23, 0xFDE5380C );
  P( A, B, C, D,  1,  4, 0xA4BEEA44 );
  P( D, A, B, C,  4, 11, 0x4BDECFA9 );
  P( C, D, A, B,  7, 16, 0xF6BB4B60 );
  P( B, C, D, A, 10, 23, 0xBEBFBC70 );
  P( A, B, C, D, 13,  4, 0x289B7EC6 );
  P( D, A, B, C,  0, 11, 0xEAA127FA );
  P( C, D, A, B,  3, 16, 0xD4EF3085 );
  P( B, C, D, A,  6, 23, 0x04881D05 );
  P( A, B, C, D,  9,  4, 0xD9D4D039 );
  P( D, A, B, C, 12, 11, 0xE6DB99E5 );
  P( C, D, A, B, 15, 16, 0x1FA27CF8 );
  P( B, C, D, A,  2, 23, 0xC4AC5665 );

#undef F

#define F(x,y,z) (y ^ (x | ~z))

  P( A, B, C, D,  0,  6, 0xF4292244 );
  P( D, A, B, C,  7, 10, 0x432AFF97 );
  P( C, D, A, B, 14, 15, 0xAB9423A7 );
  P( B, C, D, A,  5, 21, 0xFC93A039 );
  P( A, B, C, D, 12,  6, 0x655B59C3 );
  P( D, A, B, C,  3, 10, 0x8F0CCC92 );
  P( C, D, A, B, 10, 15, 0xFFEFF47D );
  P( B, C, D, A,  1, 21, 0x85845DD1 );
  P( A, B, C, D,  8,  6, 0x6FA87E4F );
  P( D, A, B, C, 15, 10, 0xFE2CE6E0 );
  P( C, D, A, B,  6, 15, 0xA3014314 );
  P( B, C, D, A, 13, 21, 0x4E0811A1 );
  P( A, B, C, D,  4,  6, 0xF7537E82 );
  P( D, A, B, C, 11, 10, 0xBD3AF235 );
  P( C, D, A, B,  2, 15, 0x2AD7D2BB );
  P( B, C, D, A,  9, 21, 0xEB86D391 );

#undef F

  ctx->state[0] += A;
  ctx->state[1] += B;
  ctx->state[2] += C;
  ctx->state[3] += D;
}

void CMD5::md5_update( struct md5_context *ctx, uint8 *input, uint32 length )
{
  uint32 left, fill;

  if( ! length ) return;

  left = ( ctx->total[0] >> 3 ) & 0x3F;
  fill = 64 - left;

  ctx->total[0] += length <<  3;
  ctx->total[1] += length >> 29;

  ctx->total[0] &= 0xFFFFFFFF;
  ctx->total[1] += ctx->total[0] < length << 3;

  if( left && length >= fill )
  {
    memcpy( (void *) (ctx->buffer + left), (void *) input, fill );
    md5_process( ctx, ctx->buffer );
    length -= fill;
    input  += fill;
    left = 0;
  }

  while( length >= 64 )
  {
    md5_process( ctx, input );
    length -= 64;
    input  += 64;
  }

  if( length )
  {
    memcpy( (void *) (ctx->buffer + left), (void *) input, length );
  }
}

static uint8 md5_padding[64] =
{
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void CMD5::md5_finish( struct md5_context *ctx, uint8 digest[16] )
{
  uint32 last, padn;
  uint8 msglen[8];

  PUT_UINT32( ctx->total[0], msglen, 0 );
  PUT_UINT32( ctx->total[1], msglen, 4 );

  last = ( ctx->total[0] >> 3 ) & 0x3F;
  padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

  md5_update( ctx, md5_padding, padn );
  md5_update( ctx, msglen, 8 );

  PUT_UINT32( ctx->state[0], digest,  0 );
  PUT_UINT32( ctx->state[1], digest,  4 );
  PUT_UINT32( ctx->state[2], digest,  8 );
  PUT_UINT32( ctx->state[3], digest, 12 );
}

void CMD5::GenerateMD5(unsigned char* buffer,int bufferlen)
{
  struct md5_context context;
  md5_starts (&context);
  md5_update (&context, buffer, bufferlen);
  md5_finish (&context,(unsigned char*)m_data);
}

CMD5::CMD5()
{
  for(int i=0;i<4;i++)
    m_data[i]=0;
}

CMD5::CMD5(unsigned long* md5src)
{
  memcpy(m_data,md5src,16);
}
int _httoi(const char *value)
{
  struct CHexMap
  {
    char chr;
    int value;
  };
  const int HexMapL = 16;
  CHexMap HexMap[HexMapL] =
  {
    {'0', 0}, {'1', 1},
    {'2', 2}, {'3', 3},
    {'4', 4}, {'5', 5},
    {'6', 6}, {'7', 7},
    {'8', 8}, {'9', 9},
    {'a', 10}, {'b', 11},
    {'c', 12}, {'d', 13},
    {'e', 14}, {'f', 15}
  };
  //pthread_mutex_lock(&mutexMemory);
  char *mstr = strdup(value);
  //pthread_mutex_unlock(&mutexMemory);
  char *s = mstr;
  int result = 0;
  if (*s == '0' && *(s + 1) == 'X') s += 2;
  bool firsttime = true;
  while (*s != '/0')
  {
    bool found = false;
    for (int i = 0; i < HexMapL; i++)
    {
      if (*s == HexMap[i].chr)
      {
        if (!firsttime) result <<= 4;
        result |= HexMap[i].value;
        found = true;
        break;
      }
    }
    if (!found) break;
    s++;
    firsttime = false;
  }
  //pthread_mutex_lock(&mutexMemory);
  free(mstr);
  //pthread_mutex_unlock(&mutexMemory);
  return result;
}


CMD5::CMD5(const char* md5src)
{
  if (strcmp(md5src,"")==0)
  {
    for(int i=0;i<4;i++)
      m_data[i]=0;
    return;
  }
  for(int j = 0; j < 16; j++ )
  {
    char buf[10];
    strncpy(buf,md5src,2);
    md5src+=2;
    ((unsigned char*)m_data)[j] = _httoi(buf);
  }
}

CMD5 CMD5::operator +(CMD5 adder)
{
  unsigned long m_newdata[4];
  for(int i=0;i<4;i++)
    m_newdata[i]=m_data[i]^(adder.m_data[i]);
  return CMD5(m_newdata);
}

bool CMD5::operator ==(CMD5 cmper)
{
  return (memcmp(cmper.m_data ,m_data,16)==0);
}

//void CMD5::operator =(CMD5 equer)
//{
//  memcpy(m_data,equer.m_data ,16);
//}

string CMD5::ToString()
{
  char output[33];
  for(int j = 0; j < 16; j++ )
  {
    sprintf( output + j * 2, "%02x", ((unsigned char*)m_data)[j]);
  }
  return string(output);
}

std::vector<unsigned char> CMD5::get_hex() {
  std::vector<unsigned char> ret;
  ret.reserve(16);
  for(int j = 0; j < 16; j++ ) {
    ret.push_back(((unsigned char*)m_data)[j]);
//     sprintf( output + j * 2, "%02x", ((unsigned char*)m_data)[j]);
  }
  return ret;
}

void GenUrl(const std::string& bucket, const std::string& object, std::string& url) {
    url.append("bs://");
    url.append(bucket);
    url.append("/");
    url.append(object);
}




// =========================================================================
const int MD5_LEN = 16;
const int BUCKET_SHARD_NUM = 1024;

// original wrong one
int32_t get_shard_key0(const std::string &bucket, const std::string &object) {

  std::string url;// = "bs://zyb-hiphotos/3801213fb80e7bec5e56593c292eb9389a506b5a.jpg";
  GenUrl(bucket, object, url);

  CMD5 iMD5;
  iMD5.GenerateMD5((unsigned char*)url.data(), url.size());
  std::string md5sum = iMD5.ToString();

  unsigned char* p = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(md5sum.data()));
  std::vector<unsigned char> md5_raw = iMD5.get_hex();

  unsigned char temp[MD5_LEN/4] = {};
  for (size_t i = 0; i < 4; i++) {
//     temp[i] = p[i] ^ p[i + 4] ^ p[i + 8] ^ p[i + 12];
//     temp[i] = p[i * 4] ^ p[i * 4 + 1] ^ p[i * 4 + 2] ^ p[i * 4 + 3];
    temp[i] = md5_raw[i] ^ md5_raw[i + 4] ^ md5_raw[i + 8] ^ md5_raw[i + 12];
//     temp[i] = md5_raw[i * 4] ^ md5_raw[i * 4 + 1] ^ md5_raw[i * 4 + 2] ^ md5_raw[i * 4 + 3];
  }
  uint32_t shard_key = 0;
  for (size_t i = 0; i < 4; i++) {
    shard_key << 4;
    shard_key += static_cast<uint32_t>(temp[i]);
  }

  return static_cast<int32_t>(shard_key % BUCKET_SHARD_NUM);

}

// the corrected original one
int32_t get_shard_key1(const std::string &bucket, const std::string &object) {

  std::string url;// = "bs://zyb-hiphotos/3801213fb80e7bec5e56593c292eb9389a506b5a.jpg";
  GenUrl(bucket, object, url);

  CMD5 iMD5;
  iMD5.GenerateMD5((unsigned char*)url.data(), url.size());
  std::string md5sum = iMD5.ToString();

  unsigned char* p = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(md5sum.data()));
  std::vector<unsigned char> md5_raw = iMD5.get_hex();

  unsigned char temp[MD5_LEN/4] = {};
  for (size_t i = 0; i < 4; i++) {
//     temp[i] = p[i] ^ p[i + 4] ^ p[i + 8] ^ p[i + 12];
//     temp[i] = p[i * 4] ^ p[i * 4 + 1] ^ p[i * 4 + 2] ^ p[i * 4 + 3];
    temp[i] = md5_raw[i] ^ md5_raw[i + 4] ^ md5_raw[i + 8] ^ md5_raw[i + 12];
//     temp[i] = md5_raw[i * 4] ^ md5_raw[i * 4 + 1] ^ md5_raw[i * 4 + 2] ^ md5_raw[i * 4 + 3];
  }
  uint32_t shard_key = 0;
  for (size_t i = 0; i < 4; i++) {
//     shard_key << 4;
//     shard_key = shard_key << 4;
    shard_key = shard_key << 8;
    shard_key += static_cast<uint32_t>(temp[i]);
  }

  return static_cast<int32_t>(shard_key % BUCKET_SHARD_NUM);
}

// use all bits
int32_t get_shard_key2(const std::string &bucket, const std::string &object) {
  std::string url;// = "bs://zyb-hiphotos/3801213fb80e7bec5e56593c292eb9389a506b5a.jpg";
  GenUrl(bucket, object, url);

  CMD5 iMD5;
  iMD5.GenerateMD5((unsigned char*)url.data(), url.size());

  std::vector<unsigned char> md5_raw = iMD5.get_hex();

  std::vector<unsigned char> md5_bin(128, 0);

  for (int i = 0; i < 16; ++i) {
    unsigned char c = md5_raw[i];
    for (int j = 0; j < 8; ++j) {
      if (c & 0x01) {
        md5_bin[8 * i + j] = 1;
      } else {
        md5_bin[8 * i + j] = 0;
      }
      c >>= 1;
    }
  }

  int32_t shard_key = 0;
  int cnt0 = 0;
  int cnt1 = 0;
  for (int i = 0; i < 128; ++i) {
    if (md5_bin[i]) {
      ++cnt1;
    } else {
      ++cnt0;
    }
    if (i % 13 == 0) {
      if (cnt1 > cnt0) {
        // cur bit is 1
        shard_key |= 1;
      }
      shard_key <<= 1;
      cnt0 = 0;
      cnt1 = 0;
    }
  }
  if (cnt1 > cnt0) {
    // cur bit is 1
    shard_key |= 1;
  }

  return shard_key;
}

// 2016-03-16 fuqiang
int32_t get_shard_key3(const std::string &bucket, const std::string &object) {
  std::string url;// = "bs://zyb-hiphotos/3801213fb80e7bec5e56593c292eb9389a506b5a.jpg";
  GenUrl(bucket, object, url);

  CMD5 iMD5;
  iMD5.GenerateMD5((unsigned char*)url.data(), url.size());
  std::string md5sum = iMD5.ToString();
//   std::cout << md5sum << std::endl;

  std::vector<unsigned char> md5_raw = iMD5.get_hex();
  char tmp[16];
  for (int i = 0; i < 16; ++i) {
    tmp[i] = md5_raw[i];
  }
  uint32_t* tmp1 = reinterpret_cast<uint32_t*>(tmp);
//   return static_cast<int32_t>((md5_raw[0]^md5_raw[1]^md5_raw[2]^md5_raw[3]) % BUCKET_SHARD_NUM);
  return static_cast<int32_t>((tmp1[0]^tmp1[1]^tmp1[2]^tmp1[3]) % BUCKET_SHARD_NUM);

}

int32_t GetShardKey(const std::string &bucket, const std::string &object) {

//   return get_shard_key0(bucket, object);
//   return get_shard_key1(bucket, object);
//      return get_shard_key2(bucket, object);
  // 20160316 32 bit
  return get_shard_key3(bucket, object);
}

#include <cstdlib>
#include <ctime>

std::string get_random_str(int ascii_start = 33, int ascii_end = 126) {
  int rand_len = rand() % 50;
  rand_len += 1;
  char c;
  char* buf = new char[rand_len + 1];
  buf[rand_len] = '\0';
  for (int i = 0; i < rand_len; ++i) {
    // 33 - 126, visible chars
    c = rand() % (ascii_end - ascii_start);
    c += ascii_start;
    buf[i] = c;
  }
  std::string str = buf;
  delete[] buf;
  return str;
}

void test_md5() {
  std::string str = "附近的刷卡了附近打瞌睡了发动机阿卡死了";
  CMD5 iMD5;
  char buf[1024];
  for (int i = 0; i < 10; ++i) {
    snprintf(buf, 1024, "%d", i);
    str = buf;
    iMD5.GenerateMD5((unsigned char*)str.data(), str.size());
    std::string md5 = iMD5.ToString();
    std::cout << md5 << std::endl;
  }
}

void test_rand_str() {
  for (int i = 0; i < 10; ++i) {
    std::cout << get_random_str(97, 122) << std::endl;
  }
}

#include <vector>
#include <algorithm>
void test_shard_key() {
  std::vector<int> counters(1024, 0);
  std::string bucket = "zyb-hiphotos";
  std::string object;
  for (int i = 0; i < 1000000; ++i) {
//      bucket = get_random_str();
     object = get_random_str();
     int shard_index = GetShardKey(bucket, object);
     if (shard_index < 0) {
       std::cout << "!!!!!!!!!!!!!boom!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
     }
     ++counters[shard_index];
  }
//   sort(counters.begin(), counters.end());
  int max = 0;
  for (auto each : counters) {
    if (max < each) {
      max = each;
    }
  }
  max /= 160;
  for (int i = 0; i < 1024; ++i) {
    std::cout << i << ": " << counters[i];// << std::endl;
    for (int j = 0; j < counters[i]/max; ++j) {
      std::cout << "=";
    }
    std::cout << std::endl;
  }
}


void test_single_shard_key() {
  std::string bucket = "test_bucket";
  std::string object = "test_object";
//   std::cout << (-100) % 13 << std::endl;
  std::cout << GetShardKey(bucket, object) << std::endl;
}

int main(void) {
//   test_shard_key();
  test_single_shard_key();
//   test_md5();
//   for (int i = 0; i < 100; ++i) {
//     std::cout << get_random_str() << std::endl;
//   }
  return 0;
}

