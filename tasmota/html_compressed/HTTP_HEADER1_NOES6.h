/////////////////////////////////////////////////////////////////////
// compressed by tools/unishox/compress-html-uncompressed.py
/////////////////////////////////////////////////////////////////////

const size_t HTTP_HEADER1_SIZE = 425;
const char HTTP_HEADER1_COMPRESSED[] PROGMEM = "\x3D\x0F\xE1\x10\x98\x1D\x19\x0C\x64\x85\x50\xD0\x8F\xC3\xD0\x55\x0D\x09\x05\x7C"
                             "\x3C\x7C\x3D\x87\xD7\x8F\x62\x0C\x2B\xF7\x8F\x87\xB0\xF6\x1F\x87\xA0\xA7\x62\x1F"
                             "\x87\xA0\xD7\x56\x83\x15\x7F\xF3\xA3\xE1\xF6\x2E\x8C\x1D\x67\x3E\x7D\x90\x21\x52"
                             "\xEB\x1A\xCF\x87\xB0\xCF\x58\xF8\xCC\xFD\x1E\xC4\x1E\x75\x3E\xA3\xE1\xEC\x1F\xD1"
                             "\x28\x51\xF0\x46\x67\xA1\xB3\xAC\x7F\x44\xA1\x47\x56\xF6\xD6\xD8\x47\x5F\x83\xB0"
                             "\x99\xF0\xE4\x3A\x88\x5F\x9F\xCE\xBF\x07\x61\x58\xE0\x99\xF3\xB0\xF6\x1D\x87\xE1"
                             "\xE9\x5B\x41\x33\xF0\xFA\xF2\x3A\xD1\xF5\xE3\xD0\xEC\x04\x19\x67\xA7\x83\xFE\x8C"
                             "\xA3\xF0\xCE\xFE\x8D\x87\xCE\x16\x10\x47\x50\x54\x75\x56\x1D\x54\x30\xEA\x18\x19"
                             "\xF0\xFB\x3E\xCF\x06\x05\xF0\x75\xB9\xC9\x8E\x3B\xBE\x3B\xC7\xB7\xEE\x85\xFF\x90"
                             "\x98\x18\xB1\xAF\xA8\xE8\x3C\xE8\x98\x4C\x6B\xEA\x21\xC6\x45\xA2\x1D\xDF\x1D\xE3"
                             "\xC1\xEE\x04\x4C\x38\xD5\xE0\x4F\xC3\x8D\x42\xDF\xCC\x8B\xCC\x26\x1D\x67\xC1\x27"
                             "\x0D\xF0\xC3\xBB\xA7\x78\xF6\xB1\xC7\x77\x4E\xF1\xD2\x8C\x86\x33\xE1\xDD\x04\x69"
                             "\x07\xC3\xE1\xF7\x4C\xD9\x47\xD9\xDA\x3E\xC6\x5F\xBC\x3F\x9F\x10\xFB\x3C\xC1\x06"
                             "\x70\x23\xE3\xE3\xE1\x1D\xD3\x07\x78\xF6\x8F\xEF\x09\x83\xE7\x4B\x10\x42\x66\x6F"
                             "\xA8\x82\xDF\x53\xE7\xF3\xBA\x7D\x85\x96\x21\xF6\x75\x18\x3B\xC7\x83\xDC";

#define  HTTP_HEADER1       Decompress(HTTP_HEADER1_COMPRESSED,HTTP_HEADER1_SIZE).c_str()